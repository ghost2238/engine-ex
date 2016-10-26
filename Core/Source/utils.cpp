#include "utils.h"

namespace EngineEx
{

	#define LOG_D(...) Log::Debug(LogModule::Utils, __VA_ARGS__);
	#define LOG_E(...) Log::Error(LogModule::Utils, __VA_ARGS__);
	#define LOG_T(...) Log::Trace(LogModule::Utils, __VA_ARGS__);
	#define LOG_I(...) Log::Info(LogModule::Utils, __VA_ARGS__);

	FunctionAnalyzer::FunctionAnalyzer(DWORD entryPoint, std::vector<_DecodedInst> disassembled)
	{
		this->instructionCount = disassembled.size();
		this->entryPoint = entryPoint;

		int int3Count = 0;

		for (unsigned int i = 0;i < instructionCount;i++)
		{
			this->disassembled.push_back(disassembled[i]);
			auto mnemonic = (char*)disassembled[i].mnemonic.p;
			if (equals(mnemonic, "INT 3"))
			{
				// TODO: This needs to be more strict.
				// Checking if there is no jump to an instruction beyond the last RET found should be enough
				Log::Debug(LogModule::Utils, "Found function end at 0x%x.", (DWORD)(entryPoint + disassembled[i].offset));
				this->endOfFunction = (DWORD)(entryPoint + disassembled[i].offset);
				break;
			}
		}
	}

	std::vector<_DecodedInst> FunctionAnalyzer::FindByMnemonic(char* mnemonic)
	{
		std::vector<_DecodedInst> found;
		for (auto& instruction : this->disassembled)
		{
			if (strcmp(mnemonic, (char*)instruction.mnemonic.p) == 0)
				found.push_back(instruction);
		}
		return found;
	}

	_DecodeResult DisAssemble(byte* bytes, int length, std::vector<_DecodedInst>& disassembledCode)
	{
		unsigned int instructionCount = 0;
		_DecodedInst* disassembled = new _DecodedInst[length];
		auto result = distorm_decode(0, bytes, length, Decode32Bits, disassembled, length, &instructionCount);
		if (result != DECRES_SUCCESS)
		{
			Log::Error(LogModule::Utils, "diStorm was unable to disassemble code");
			return result;
		}
		for (unsigned int i = 0;i < instructionCount;i++)
			disassembledCode.push_back(disassembled[i]);

		return result;
	}

	std::vector<std::string>* GetAsm(std::vector<_DecodedInst> instructions, int start, int count)
	{
		Log::Trace(LogModule::Utils, "GetAsm");
		auto lines = new std::vector<std::string>();
		unsigned int end = start + count;
		if (end > instructions.size()-1)
			end = instructions.size()-1;

		Log::Trace(LogModule::Utils, "start: %d, end: %d, size: %d", start, end, instructions.size());
		for (unsigned int i = start;i < end; i++)
		{
			lines->push_back(format("%-12s | %s%s%s", (char*)instructions[i].instructionHex.p,
				(char*)instructions[i].mnemonic.p, instructions[i].operands.length != 0 ? " " : "",
				(char*)instructions[i].operands.p));
		}
		return lines;
	}

	bool IsBranch(_DecodedInst instruction)
	{
		const char* instr = (const char*)instruction.mnemonic.p;
		if (instr[0] == 'J' || strstr(instr, "CALL"))
			return true;
		return false;
	}

	std::vector<std::string>* GetAsm(byte* bytes, int size)
	{
		auto lines = new std::vector<std::string>();

		std::vector<_DecodedInst>* disassembled = new std::vector<_DecodedInst>();
		DisAssemble(bytes, size, *disassembled);
		lines = GetAsm(*disassembled, 0, disassembled->size());

		return lines;
	}

	MemoryPatch::MemoryPatch(int size, DWORD address)
	{
		this->size = size;
		this->data = new byte[size];
		this->address = address;
		this->currentPos = 0;
	}

	MemoryPatch::MemoryPatch(int size)
	{
		this->size = size;
		this->data = new byte[size];
		this->address = Allocate(size);
		this->currentPos = 0;
	}

	std::vector<std::string>* MemoryPatch::GetAsm()
	{
		return EngineEx::GetAsm(this->data, this->currentPos);
	}

	void MemoryPatch::Add(byte b)
	{
		this->data[this->currentPos++] = b;
	}

	void MemoryPatch::AddAdress(DWORD adress)
	{
		*(PDWORD)&this->data[this->currentPos] = adress;
		this->currentPos += 4;
	}

	void MemoryPatch::Push(DWORD adress)
	{
		this->Add(0x68); // PUSH
		this->AddAdress(adress);
	}

	void MemoryPatch::Call(DWORD adress)
	{
		this->Add(AsmX86::call);
		this->AddAdress(adress - this->address - this->currentPos - 4);
	}

	void MemoryPatch::JmpAbs(DWORD adress)
	{
		this->Add(AsmX86::jmp_abs);
		this->AddAdress(adress - this->address - this->currentPos - 4);
	}

	void MemoryPatch::Jmp(DWORD adress)
	{
		this->Add(AsmX86::jmp);
		this->AddAdress(adress - this->address - this->currentPos - 4);
	}

	// This should be called when hooking a function (begin/before hook) and being in the injected setup code.
	// We use the already pushed arguments and push them again, so that we can restore the stack later and not have the handler function do the cleanup.
	// Keep in mind that all non SP registers can be used since we have saved those.
	void MemoryPatch::PushStackArgs(CallingConvention call, int count)
	{
		if (call == CallingConvention::stdcall) 
		{
			for (int x = 0; x < count;x++)
			{
				int i = 4 * count;

				this->Add(0x8B); // MOV EAX, DWORD PTR SS : [ESP + i]
				this->Add(0x44);
				this->Add(0xE4);
				this->Add(i);
				this->Add(0x50);
			}
		}
	}

	void MemoryPatch::SaveRegisters(DWORD to)
	{
		// EAX
		this->Add(0xA3);
		this->AddAdress(to);
		// ECX
		this->Add(0x89);
		this->Add(0x0D);
		this->AddAdress(to + 4);
		// EDX
		this->Add(0x89);
		this->Add(0x15);
		this->AddAdress(to + 8);
		// EBX
		this->Add(0x89);
		this->Add(0x1D);
		this->AddAdress(to + 12);
		// ESP
		this->Add(0x89);
		this->Add(0x25);
		this->AddAdress(to + 16);
		// EBP
		this->Add(0x89);
		this->Add(0x2D);
		this->AddAdress(to + 20);
		// ESI
		this->Add(0x89);
		this->Add(0x35);
		this->AddAdress(to + 24);
		// EDI
		this->Add(0x89);
		this->Add(0x3D);
		this->AddAdress(to + 28);
	}

	void MemoryPatch::RestoreRegisters(DWORD from)
	{
		// EAX
		this->Add(0xA1);
		this->AddAdress(from);
		// ECX
		this->Add(0x8B);
		this->Add(0x0D);
		this->AddAdress(from + 4);
		// EDX
		this->Add(0x8B);
		this->Add(0x15);
		this->AddAdress(from + 8);
		// EBX
		this->Add(0x8B);
		this->Add(0x1D);
		this->AddAdress(from + 12);
		// ESP
		this->Add(0x8B);
		this->Add(0x25);
		this->AddAdress(from + 16);
		// EBP
		this->Add(0x8B);
		this->Add(0x2D);
		this->AddAdress(from + 20);
		// ESI
		this->Add(0x8B);
		this->Add(0x35);
		this->AddAdress(from + 24);
		// EDI
		this->Add(0x8B);
		this->Add(0x3D);
		this->AddAdress(from + 28);
	}

	bool MemoryPatch::Write()
	{
		if (this->currentPos < this->size)
			this->warn = format("Using only %d bytes but allocated %d", this->currentPos, this->size);
		if (this->currentPos > this->size)
		{
			this->error = format("Using %d bytes but allocated only %d", this->currentPos, this->size);
			return false;
		}
		SafeWrite(this->address, this->data, this->size);

		return true;
	}

	int getMinOffset(const unsigned char* codePtr, unsigned int jumpPatchSize)
	{
		std::vector<_DecodedInst> instructions;

		auto bytes = SafeRead((DWORD)codePtr, 20);
		DisAssemble(bytes, 20, instructions);

		auto instructionCount = instructions.size();

		unsigned int offset = 0;
		for (unsigned int i = 0; offset < jumpPatchSize && i < instructionCount; ++i)
		{
			if (IsBranch(instructions[i])) return -1;
			offset += instructions[i].size;
		}
		// if we were unable to disassemble enough instructions we fail
		if (offset < jumpPatchSize) return -1;
		return offset;
	}

	bool requiresAbsJump(uintptr_t from, uintptr_t to)
	{
		uintptr_t jmpDistance = from > to ? from - to : to - from;
		return jmpDistance <= 0x7FFF0000 ? false : true;
	};

	void ResumeMainThread()
	{
		ResumeThread(GetMainThread());
	}

	void SuspendMainThread()
	{
		SuspendThread(GetMainThread());
	}

	HANDLE GetMainThread()
	{
		HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
		if (hThreadSnapshot == INVALID_HANDLE_VALUE) {
			return 0;
		}
		THREADENTRY32 tEntry;
		tEntry.dwSize = sizeof(THREADENTRY32);
		DWORD result = 0;
		DWORD currentPID = GetCurrentProcessId();
		for (BOOL success = Thread32First(hThreadSnapshot, &tEntry);
			!result && success && GetLastError() != ERROR_NO_MORE_FILES;
			success = Thread32Next(hThreadSnapshot, &tEntry))
		{
			if (tEntry.th32OwnerProcessID == currentPID) {
				result = tEntry.th32ThreadID;
			}
		}

		return OpenThread(THREAD_SUSPEND_RESUME, false, result);
	}

	// String functions
	void split(const std::string &s, char delim, std::vector<std::string> &elems) {
		std::stringstream ss;
		ss.str(s);
		std::string item;
		while (std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
	}

	std::vector<std::string> split(const std::string &s, char delim) {
		std::vector<std::string> elems;
		split(s, delim, elems);
		return elems;
	}

	bool equals(const std::string &a, const std::string &b)
	{
		return strcmp(a.c_str(), b.c_str()) == 0;
	}

	/*bool equals(const std::string &a, char* b)
	{
		return strcmp(a.c_str(), b) == 0;
	}*/

	inline std::string format(const char* fmt, ...) {
		int size = 512;
		char* buffer = 0;
		buffer = new char[size];
		va_list vl;
		va_start(vl, fmt);
		int nsize = vsnprintf(buffer, size, fmt, vl);
		if (size <= nsize) {
			delete[] buffer;
			buffer = 0;
			buffer = new char[nsize + 1];
			nsize = vsnprintf(buffer, size, fmt, vl);
		}
		std::string ret(buffer);
		va_end(vl);
		delete[] buffer;
		return ret;
	}

}
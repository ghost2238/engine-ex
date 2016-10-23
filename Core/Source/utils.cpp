#include "utils.h"

namespace EngineEx
{

	FunctionAnalyzer::FunctionAnalyzer(DWORD entryPoint, std::vector<_DecodedInst> disassembled, unsigned int instructionCount)
	{
		this->instructionCount = instructionCount;
		this->entryPoint = entryPoint;

		int int3Count = 0;

		for (unsigned int i = 0;i < instructionCount;i++)
		{
			this->disassembled.push_back(disassembled[i]);
			auto mnemonic = (char*)disassembled[i].mnemonic.p;
			if (strcmp(mnemonic, "INT 3") == 0)
			{
				// TODO: This needs to be more strict.
				// Checking if there is no jump to an instruction beyond the last RET found should be enough
				Log::Debug(LogModule::Utils, "Found function end at 0x%x.\n", (DWORD)(entryPoint + disassembled[i].offset));
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
	_DecodedInst FunctionAnalyzer::FindFirstMnemonic(char* mnemonic)
	{

	}

	DisAssembler::DisAssembler()
	{

	}

	_DecodeResult DisAssembler::DisAssemble(byte* bytes, int length, unsigned int& instructions, std::vector<_DecodedInst>& disassembledCode)
	{
		unsigned int instructionCount = 0;
		_DecodedInst* disassembled = new _DecodedInst[length];
		auto result = distorm_decode(0, bytes, length, Decode32Bits, disassembled, length, &instructionCount);
		if (result != DECRES_SUCCESS)
		{
			this->error = "diStorm was unable to disassemble code.\n";
			Log::Error(LogModule::Utils, "diStorm was unable to disassemble code");
		}
		instructions = instructionCount;
		for (unsigned int i = 0;i < instructionCount;i++)
			disassembledCode.push_back(disassembled[i]);

		return result;
	}

	std::vector<std::string>* DisAssembler::GetAsm(std::vector<_DecodedInst> instructions, int start, int count)
	{
		Log::Trace(LogModule::Utils, "GetAsm");
		auto lines = new std::vector<std::string>();
		int end = start + count;
		if (end > instructions.size()-1)
			end = instructions.size()-1;

		Log::Trace(LogModule::Utils, "start: %d, end: %d, size: %d", start, end, instructions.size());
		for (int i = start;i < end; i++)
		{
			lines->push_back(string_format("%-12s | %s%s%s", (char*)instructions[i].instructionHex.p,
				(char*)instructions[i].mnemonic.p, instructions[i].operands.length != 0 ? " " : "",
				(char*)instructions[i].operands.p));
		}
		return lines;
	}

	std::vector<std::string>* DisAssembler::GetAsm(byte* bytes, int size)
	{
		auto lines = new std::vector<std::string>();

		unsigned int instructions;
		std::vector<_DecodedInst>* disassembled = new std::vector<_DecodedInst>();
		
		DisAssembler::DisAssemble(bytes, size, instructions, *disassembled);
		lines = this->GetAsm(*disassembled, 0, size);

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
		this->address = AllocateSpace(size);
		this->currentPos = 0;
	}

	std::vector<std::string>* MemoryPatch::GetAsm()
	{
		auto disasm = new DisAssembler();
		return disasm->GetAsm(this->data, this->size);
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
		this->Add(0xE8);
		this->AddAdress(adress - this->address - this->currentPos - 4);
	}
	void MemoryPatch::Jmp(DWORD adress)
	{
		this->Add(0xE9);
		this->AddAdress(adress - this->address - this->currentPos - 4);
	}

	bool MemoryPatch::Write()
	{
		if (this->currentPos < this->size)
			this->warn = string_format("Using only %d bytes but allocated %d", this->currentPos, this->size);
		if (this->currentPos > this->size)
		{
			this->error = string_format("Using %d bytes but allocated only %d", this->currentPos, this->size);
			return false;
		}
		SafeWrite(this->address, this->data, this->size);
		return true;
	}


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

	std::string string_format(const char* fmt, ...) {
		int size = 512;
		char* buffer = 0;
		buffer = new char[size];
		va_list vl;
		va_start(vl, fmt);
		int nsize = vsnprintf(buffer, size, fmt, vl);
		if (size <= nsize) { //fail delete buffer and try again
			delete[] buffer;
			buffer = 0;
			buffer = new char[nsize + 1]; //+1 for /0
			nsize = vsnprintf(buffer, size, fmt, vl);
		}
		std::string ret(buffer);
		va_end(vl);
		delete[] buffer;
		return ret;
	}

}
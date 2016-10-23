#include "hookmanager.h"

namespace EngineEx
{
	std::map<uintptr_t, Hook> HookManager::hooks;
	std::set<uintptr_t> HookManager::freeTrampolines;
	std::map<uintptr_t, MemoryPatch> HookManager::patches;

	#define LOG_D(...) Log::Debug(LogModule::Hooking, __VA_ARGS__);
	#define LOG_E(...) Log::Error(LogModule::Hooking, __VA_ARGS__);
	#define LOG_T(...) Log::Trace(LogModule::Hooking, __VA_ARGS__);
	#define LOG_I(...) Log::Info(LogModule::Hooking, __VA_ARGS__);

	const unsigned int NearJumpPatchSize = sizeof(int) + 1;
	const unsigned int AbsJumpPatchSize = sizeof(uintptr_t) * 2 + 2;
	// max trampoline size = longest instruction (6) starting 1 byte before jump patch boundary
	const unsigned int MaxTrampolineSize = AbsJumpPatchSize - 1 + 6;
	const unsigned int MaxTotaltTrampolineSize = MaxTrampolineSize + AbsJumpPatchSize;

	void HookManager::Init()
	{
		LOG_T("Initializing");
		int trampolineBufferSize = 4096;

		auto trampolineBuffer_ = Allocate(trampolineBufferSize);
		if (trampolineBuffer_ == NULL)
		{
			LOG_E("Unable to allocate trampoline memory!");
			return;
		}
		LOG_T("Allocated trampoline buffer")

		for (uintptr_t i = (uintptr_t)trampolineBuffer_; i<(uintptr_t)trampolineBuffer_ + trampolineBufferSize; i += MaxTotaltTrampolineSize)
			HookManager::freeTrampolines.insert(i);

		LOG_T("Filled trampoline buffer with %d elements.", HookManager::freeTrampolines.size());
	}

	// Called by generated code
	
	int count;
	DWORD lastTick;
	void __stdcall HookManager::LogFunc(char* text)
	{
		count++;
		DWORD now = GetTickCount();
		if ((now - lastTick) > 500)
		{
			printf("%s: has been called %d times since monitoring started.\n", text, count);
			lastTick = now;
		}
	}

	uintptr_t HookManager::GetFreeTrampoline()
	{
		if (HookManager::freeTrampolines.empty())
		{
			LOG_E("No trampoline space left!");
			return 0;
		}
		std::set<uintptr_t>::iterator it = freeTrampolines.begin();
		uintptr_t result = *it;
		return result;
	}

	bool HookManager::IsAlreadyHooked(DWORD originalFunc)
	{
		for (auto& kv : HookManager::hooks)
		{
			if (kv.second.originalFunc == originalFunc)
			{
				LOG_T("0x%x is already hooked by 0x%x.", originalFunc, kv.second.hookFunc);
				return true;
			}
		}
		return false;
	}

	Hook* HookManager::CreateHook(DWORD originalFunc, DWORD handlerFunc)
	{
		for (auto& kv : HookManager::hooks)
		{
			if (kv.second.originalFunc == originalFunc)
			{
				LOG_D("0x%x is already hooked by 0x%x, returning that hook.", originalFunc, kv.second.hookFunc);
				return &kv.second;
			}
		}

		if (originalFunc == 0)
			LOG_E("Original function provided is null");
		if (handlerFunc == 0)
			LOG_E("Original function provided is null");

		if (originalFunc == 0 || handlerFunc == 0) return NULL;

		LOG_T("Creating a hook, original: 0x%x, handler: 0x%x", originalFunc, handlerFunc);

		bool useAbsJump = false;
		int offset = 0;
		if (useAbsJump || requiresAbsJump((uintptr_t)originalFunc, (uintptr_t)handlerFunc))
		{
			offset = getMinOffset((const unsigned char*)originalFunc, AbsJumpPatchSize);
			useAbsJump = true;
		}
		else offset = getMinOffset((const unsigned char*)originalFunc, NearJumpPatchSize);
		// error while determining offset?
		if (offset == -1) return NULL;

		LOG_T("Allocing");
		DWORD oldProtect = 0;

		LOG_T("Get trampoline memory");
		uintptr_t trampolineAddr = HookManager::GetFreeTrampoline();
		if (trampolineAddr == NULL)
			return NULL;

		LOG_T("Copying data from original function");
		SafeMemCpy((void*)trampolineAddr, (void*)originalFunc, offset);
		auto patch = new MemoryPatch(5, (uintptr_t)originalFunc);
		auto patch2 = new MemoryPatch(5, (uintptr_t)trampolineAddr + offset);
		if (useAbsJump) 
		{
			LOG_D("Need to use absolute jumps.");
			patch->JmpAbs(handlerFunc);
			patch2->JmpAbs((uintptr_t)originalFunc + offset);
		}
		else
		{
			LOG_D("Using relative jumps.");
			patch->Jmp(handlerFunc);
			patch2->Jmp((uintptr_t)originalFunc + offset);
		}
		patch->Write();
		patch2->Write();

		HookManager::patches.insert(std::make_pair((uintptr_t)patch->address, *patch));
		HookManager::patches.insert(std::make_pair((uintptr_t)patch->address, *patch2));

		LOG_T("Flushing instruction cache");
		FlushInstructionCache(GetCurrentProcess(), (LPCVOID)trampolineAddr, MaxTotaltTrampolineSize);
		FlushInstructionCache(GetCurrentProcess(), (LPCVOID)originalFunc, useAbsJump ? AbsJumpPatchSize : NearJumpPatchSize);

		LOG_T("Removing used trampoline from free trampoline pool");
		freeTrampolines.erase(trampolineAddr);

		auto hook = new Hook("", (uintptr_t)originalFunc, (uintptr_t)handlerFunc, trampolineAddr, offset);
		HookManager::hooks.insert(std::make_pair((uintptr_t)handlerFunc, *hook));

		HookManager::LogStatus();
		return hook;
	}

	Hook* HookManager::MonitorCalls(unsigned long originalFunc, const char* name)
	{
		char* text = new char[255];
		snprintf(text, 255, "%s (0x%X2) was called.\n\0", name, (DWORD)originalFunc);

		LOG_I("Adding monitoring call for 0x%x", (DWORD)originalFunc);

		auto patch = new MemoryPatch(17);
		if (patch->address == 0)
		{
			LOG_E("Unable to allocate code.");
			return NULL;
		}
		DWORD logFunc = (DWORD)&HookManager::LogFunc;
		DWORD trampoline = HookManager::GetFreeTrampoline();
		LOG_T("Trampoline is 0x%x.", DWORD(trampoline));

		patch->Add(0x60); // PUSHAD
		patch->Push(*(PDWORD)&text);
		patch->Call(logFunc);
		patch->Add(0x61); // POPAD
		patch->Jmp(trampoline);

		std::vector<std::string>* asmCode = patch->GetAsm();
		LOG_D("Generated code:");
		for (auto s : *asmCode)
		{
			Log::Debug(LogModule::Hooking, "%s", s);
		}

		if (!patch->Write())
		{
			Log::Error(LogModule::Hooking, "%s", patch->error.c_str());
			return NULL;
		}
		if(patch->warn.length() != 0)
			Log::Debug(LogModule::Hooking, "Warn: %s", patch->error.c_str());

		return HookManager::CreateHook(originalFunc, patch->address);
	}

	Hook* HookManager::ReplaceFunction(unsigned long originalFunc, unsigned long handlerFunc)
	{
		LOG_I("Replacing 0x%x with 0x%x", originalFunc, handlerFunc);
		return HookManager::CreateHook(originalFunc, handlerFunc);
	}

	DWORD HookManager::GetDLLFunction(const char* dllName, const  char* funcName)
	{
		DWORD funcPtr = NULL;
		HMODULE hDll = LoadLibraryA(dllName);
		if (hDll == NULL)
			return 0;
		funcPtr = (DWORD)GetProcAddress(hDll, funcName);
		FreeLibrary(hDll);
		return funcPtr;
	}

	// Call handler before original func while preserving registers for the original function 
	// (and with right calling convention stack will be fine after).
	Hook* HookManager::CreateBeforeHook(unsigned long originalFunc, unsigned long handlerFunc)
	{
		LOG_I("Creating a before hook for 0x%x, handler function 0x%x", (DWORD)originalFunc, (DWORD)handlerFunc);

		if (IsAlreadyHooked(originalFunc))
			return NULL;

		DWORD trampoline = HookManager::GetFreeTrampoline();
		if (trampoline == NULL)
			return NULL;

		// 4 * 8 for all registers
		DWORD regspace = Allocate(32);

		auto patch = new MemoryPatch(150);

		auto convention = CallingConvention::stdcall;
		
		patch->SaveRegisters(regspace);
		patch->PushStackArgs(convention, 6);
		
		patch->Call(handlerFunc);
		
		patch->RestoreRegisters(regspace);

		patch->Jmp(trampoline);

		std::vector<std::string>* asmCode = patch->GetAsm();
		LOG_D("Generated code:");
		for (auto s : *asmCode)
		{
			LOG_D("%s", s);
		}

		if (!patch->Write())
		{
			LOG_E("%s", patch->error.c_str());
			return 0;
		}
		if (patch->warn.length() != 0)
			LOG_D("Warn: %s", patch->error.c_str());

		auto address = patch->address;
		HookManager::patches.insert(std::make_pair(address, *patch));

		return HookManager::CreateHook(originalFunc, patch->address);
	}

	void HookManager::RemoveHooks()
	{
		// Remove hooks
		for (auto &hook : HookManager::hooks)
		{
			LOG_I("Removing 0x%x hook (hooked to 0x%x)", hook.second.originalFunc, hook.second.hookFunc);
			HookManager::RemoveHook(&hook.second);
		}
		HookManager::LogStatus();
	}

	void HookManager::RemoveHook(Hook* hook)
	{
		LOG_I("Removing hook of 0x%x");
		SafeMemCpy((void*)hook->originalFunc, (void*)hook->trampoline, hook->patchSize);
		HookManager::hooks.erase(hook->hookFunc);
		HookManager::freeTrampolines.insert(hook->trampoline);
		freeTrampolines.insert(hook->trampoline);
		FlushInstructionCache(GetCurrentProcess(), (LPCVOID)hook->originalFunc, hook->patchSize);
		HookManager::LogStatus();
	}

	void HookManager::LogStatus()
	{
		LOG_D("Current active hooks: %d, free trampolines: %d, patches written: %d", 
			HookManager::hooks.size(), HookManager::freeTrampolines.size(), HookManager::patches.size());
	}

	void HookManager::RemoveEndHook(DWORD entryPoint)
	{
		for (auto hook : HookManager::hooks)
		{
			if (hook.second.originalFunc == entryPoint)
			{
				for (auto& kv : hook.second.overwrittenCode)
				{
					LOG_D("Restoring code for 0x%x.", hook.second.originalFunc);
					DWORD offset = (DWORD)kv.first;
					CodeBytes* cb = kv.second;
					SafeWrite((DWORD)offset, cb->bytes, cb->size);
				}
			}
		}
	}

	EndHookError HookManager::CreateEndHook(std::string name, DWORD entryPoint, DWORD hookFunction)
	{
		if (entryPoint == 0)
		{
			LOG_E("Null entrypoint.");
			return EndHookError::NullEntryPoint;
		}

		unsigned int instructionCount;
		std::vector<_DecodedInst>* disassembled = new std::vector<_DecodedInst>();

		auto bytes = SafeRead((DWORD)entryPoint, 4096);

		auto disAsm = new DisAssembler();

		auto result = disAsm->DisAssemble(bytes, 4096, *disassembled);
		instructionCount = disassembled->size();

		bool done = false;
		bool firstReturn = true;
		

		auto funcAnalyzer = new FunctionAnalyzer(entryPoint, *disassembled, instructionCount);

		if (funcAnalyzer->endOfFunction == 0)
		{
			LOG_E("Unable to find where function ends.");
			return EndHookError::NoFunctionEnd;
		}

		auto rets = funcAnalyzer->FindByMnemonic("RET");

		DWORD retOffset = ((DWORD)(funcAnalyzer->entryPoint + rets.front().offset));
		
		LOG_D("First RET offset found at 0x%x", retOffset);
		LOG_D("Found %d RET instructions.", rets.size());

		int saveSize = funcAnalyzer->endOfFunction - retOffset;
		byte* savedCode = SafeRead(retOffset, saveSize);
		CodeBytes* cb = new CodeBytes;
		cb->bytes = savedCode;
		cb->size = saveSize;
		Hook* endhook = new Hook(name, (DWORD)entryPoint, (DWORD)hookFunction, 0, cb->size);
		endhook->addOverwrittenCode(retOffset, cb);
		

		for (auto& ret : rets)
		{
			for (unsigned int i=0;i<funcAnalyzer->disassembled.size();i++)
			{
				if (ret.offset == funcAnalyzer->disassembled[i].offset)
				{
					char* mnemonic = (char*)funcAnalyzer->disassembled[i + 1].mnemonic.p;

					if ((strcmp("INT 3", mnemonic) != 0) &&
						(strcmp("NOP", mnemonic) != 0))
					{
						LOG_D("The return instruction at 0x%x is not the last one so we need to relocate the code",
							(DWORD)funcAnalyzer->entryPoint + ret.offset);
						LOG_D("The relocation code is not available so we need to cancel.");

						auto asmCode = disAsm->GetAsm(funcAnalyzer->disassembled, i, 3);
						for (auto& line : *asmCode)
						{
							Log::Debug(LogModule::Hooking, "%s", line.c_str());
						}

						return EndHookError::UnableToRelocate;
					}
				}
			}
			auto patch = new MemoryPatch(5, (DWORD)(funcAnalyzer->entryPoint + ret.offset));
			patch->Jmp(hookFunction);
			patch->Write();
		}
		free(disassembled);

		return EndHookError::Success;
	};
}
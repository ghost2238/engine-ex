#include "hookmanager.h"

namespace EngineEx
{
	NCodeHookIA32 nch;
	std::vector<Hook*> HookManager::Hooks;

	#define LOG_D(...) Log::Debug(LogModule::Hooking, __VA_ARGS__);
	#define LOG_E(...) Log::Error(LogModule::Hooking, __VA_ARGS__);
	#define LOG_T(...) Log::Trace(LogModule::Hooking, __VA_ARGS__);
	#define LOG_I(...) Log::Info(LogModule::Hooking, __VA_ARGS__);

	void HookManager::Init()
	{
		nch.forceAbsoluteJumps(false);
	}

	// Called by generated code
	void __stdcall HookManager::LogFunc(char* text)
	{
		printf("%s", text);
	}

	DWORD HookManager::CreateHook(DWORD originalFunc, DWORD handlerFunc)
	{
		LOG_T("Creating a hook, original: 0x%x, handler: 0x%x", originalFunc, handlerFunc);
		DWORD result = nch.createHook(originalFunc, (DWORD)handlerFunc);
		LOG_D("Result: 0x%x", result);
		if (result == 0)
		{
			LOG_E("Hooking failed");
			return 0;
		}
		return 0;
	}

	void HookManager::MonitorCalls(unsigned long originalFunc, const char* name)
	{
		char* text = new char[255];
		snprintf(text, 255, "%s (0x%X2) was called.\n\0", name, (DWORD)originalFunc);

		LOG_I("Adding monitoring call for 0x%x", (DWORD)originalFunc);

		auto patch = new MemoryPatch(17);
		if (patch->address == 0)
		{
			LOG_E("Unable to allocate code.");
			return;
		}
		DWORD logFunc = (DWORD)&HookManager::LogFunc;
		DWORD trampoline = nch.getNextFreeTrampoline();
		LOG_D("Trampoline 0x%x.", DWORD(trampoline));

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
			return;
		}
		if(patch->warn.length() != 0)
			Log::Debug(LogModule::Hooking, "Warn: %s", patch->error.c_str());

		HookManager::CreateHook(originalFunc, patch->address);
	}

	DWORD HookManager::ReplaceFunction(unsigned long originalFunc, unsigned long handlerFunc)
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
	DWORD HookManager::CreateBeforeHook(unsigned long originalFunc, unsigned long handlerFunc)
	{
		LOG_I("Creating a before hook for 0x%x, handler function 0x%x", (DWORD)originalFunc, (DWORD)handlerFunc);
		// All this needs to be seriously refactored, this is just experiment code...

		// 4 * 8 for all registers
		DWORD regspace = AllocateSpace(32);

		auto patch = new MemoryPatch(44);

		/* Save registers (only EAX/EDX for now) */
		patch->Add(0x89);
		patch->Add(0x0D);
		patch->AddAdress(regspace);
		patch->Add(0x89);
		patch->Add(0x15);
		patch->AddAdress(regspace + 4);
		
		// Assumes callee cleanup and 2 args thus the defined handler needs to be a __fastcall, __stdcall or __thiscall
		// Push our arguments to handler function
		patch->Add(0x8B); // MOV EAX, DWORD PTR SS : [ESP + 8]
		patch->Add(0x44);
		patch->Add(0xE4);
		patch->Add(0x08);

		patch->Add(0x8B); // MOV EDX, DWORD PTR SS : [ESP + 4]
		patch->Add(0x54);
		patch->Add(0xE4);
		patch->Add(0x04);

		patch->Add(0x50); // PUSH EAX
		patch->Add(0x52); // PUSH EDX
		
		patch->Call(handlerFunc);
		
		/* Restore registers */
		patch->Add(0x8B);
		patch->Add(0x0D);
		patch->AddAdress(regspace);

		patch->Add(0x8B);
		patch->Add(0x15);
		patch->AddAdress(regspace);

		DWORD trampoline = nch.getNextFreeTrampoline();

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

		return HookManager::CreateHook(originalFunc, patch->address);
	}

	void HookManager::RemoveHooks()
	{
		// Remove CallHooks
		for (auto &kv : nch.getHookedFunctions())
		{
			LOG_I("Removing hook 0x%x (Hooked to 0x%x)", kv.second.OriginalFunc, kv.second.HookFunc);
			nch.removeHook(kv.second.HookFunc);
		}
		// Remove EndHooks
		for (auto &hook : HookManager::Hooks)
		{
			LOG_I("Removing EndHook 0x%x (Hooked to 0x%x)", hook->originalFunctionOffset, hook->handlerFunctionOffset);
			HookManager::RemoveEndHook(hook->originalFunctionOffset);
		}
	}

	void HookManager::RemoveHook(DWORD func)
	{
		nch.removeHook(func);
	}

	void HookManager::RemoveEndHook(DWORD entryPoint)
	{
		for (auto hook : HookManager::Hooks)
		{
			if (hook->originalFunctionOffset == entryPoint)
			{
				for (auto& kv : hook->overwrittenCode)
				{
					LOG_D("Restoring code for 0x%x.", hook->originalFunctionOffset);
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

		auto result = disAsm->DisAssemble(bytes, 4096, instructionCount, *disassembled);

		bool done = false;
		bool firstReturn = true;
		Hook* endhook = new Hook(name, (DWORD)entryPoint, (DWORD)hookFunction);

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
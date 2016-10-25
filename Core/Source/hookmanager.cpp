#include "hookmanager.h"

namespace EngineEx
{
	std::map<uintptr_t, Hook> HookManager::hooks;
	std::set<uintptr_t> HookManager::freeTrampolines;
	std::map<uintptr_t, MemoryPatch> HookManager::patches;
	std::map<std::string, FunctionSymbol> HookManager::functions;
	std::map<std::string, VariableSymbol> HookManager::variables;

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

		LOG_D("Reading symbols from JSON file.");
		std::ifstream stream("./symbols.json");
		if (!stream.is_open())
		{
			Log::Error(LogModule::Global, "Error, unable to open ./symbols.json");
		}
		else
		{
			Json::Value root;
			stream >> root;
			const Json::Value funcs = root["Functions"];
			for (unsigned int i = 0; i < funcs.size(); ++i)
			{
				LOG_T("Reading function symbol %d", i + 1);
				auto symbol = new FunctionSymbol;
				symbol->name = funcs[i].get("Name", "").asString();
				LOG_T("Read name");
				const char* off = funcs[i].get("Offset", 0).asCString();
				int number = (int)strtol(off, NULL, 16);
				if (number == 0) number = (int)strtol(off, NULL, 0);
				symbol->offset = number;
				LOG_T("Read offset");
				symbol->returnValue = funcs[i].get("RetVal", "void").asString();
				LOG_T("Read return value");
				auto argStr = funcs[i].get("Args", "").asString();
				symbol->arguments = split(argStr, ',');
				HookManager::functions[symbol->name] = *symbol;
				LOG_T("[0x%x] %s %s(%s)", symbol->offset, symbol->returnValue.c_str(), symbol->name.c_str(), argStr.c_str());
			}
			if (funcs.size() != HookManager::functions.size())
				LOG_E("  %d functions was found and only %d was added.", funcs.size(), HookManager::functions.size());
			LOG_D("  Loaded %d functions", HookManager::functions.size());

			const Json::Value vars = root["Variables"];
			for (unsigned int i = 0; i < vars.size(); ++i)
			{
				LOG_T("Reading variable symbol %d", i + 1);
				auto symbol = new VariableSymbol;
				symbol->name = vars[i].get("Name", "").asString();
				LOG_T("Read name");
				const char* off = vars[i].get("Offset", 0).asCString();
				int number = (int)strtol(off, NULL, 16);
				if (number == 0) number = (int)strtol(off, NULL, 0);
				symbol->offset = number;
				LOG_T("Read offset");
				symbol->type = vars[i].get("Type", "").asString();
				LOG_T("Read type");
				HookManager::variables[symbol->name] = *symbol;
				LOG_T("[0x%x] %s - %s", symbol->offset, symbol->name.c_str(), symbol->type.c_str());
			}

			if (vars.size() != HookManager::variables.size())
				LOG_E("  %d variables was found and only %d was added.", vars.size(), HookManager::variables.size());
			LOG_D("  Loaded %d variables", HookManager::variables.size());

		}
		
		LOG_I("Initialization done.");
	}

	// Called by generated code
	int count;
	DWORD lastTick;
	void __stdcall HookManager::LogFunc(char* text)
	{
		printf("%s", text);
		/*count++;
		DWORD now = GetTickCount();
		if ((now - lastTick) > 500)
		{
			printf("%s: has been called %d times since monitoring started.\n", text, count);
			lastTick = now;
		}*/
	}

	void __stdcall HookManager::Disable() { }

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

	Hook* HookManager::MonitorCalls(const std::string & functionName)
	{
		auto symbol = HookManager::functions[functionName];
		if (symbol.name.empty())
			return NULL;
		return HookManager::HookFunction(symbol.name, symbol.offset, NULL, HookType::Monitor, HookMethod::Detours);
	}

	Hook* HookManager::HookFunction(DWORD originalFunc, DWORD handlerFunc, HookType type, HookMethod method)
	{
		return HookManager::HookFunction("", originalFunc, handlerFunc, type, method);
	}

	Hook* HookManager::HookFunction(DWORD originalFunc, DWORD handlerFunc, HookType type)
	{
		return HookManager::HookFunction("", originalFunc, handlerFunc, type, HookMethod::Detours);
	}

	Hook* HookManager::HookFunction(const std::string& functionName, DWORD handlerFunc, HookType type)
	{
		return HookManager::HookFunction(functionName, handlerFunc, type, HookMethod::Detours);
	}

	Hook* HookManager::HookFunction(const std::string& functionName, DWORD handlerFunc, HookType type, HookMethod method)
	{
		auto symbol = HookManager::functions[functionName];
		if (symbol.name.empty())
			return NULL;
		return HookManager::HookFunction(symbol.name, symbol.offset, handlerFunc, type, method);
	}

	Hook* HookManager::HookFunction(const std::string& name, DWORD originalFunc, DWORD handlerFunc, HookType type, HookMethod method)
	{
		for (auto& kv : HookManager::hooks)
		{
			if (kv.second.originalFunc == originalFunc)
			{
				LOG_D("0x%x is already hooked by 0x%x, returning that hook.", originalFunc, kv.second.hookFunc);
				return &kv.second;
			}
		}
		std::string typeStr = "";
		if (type == HookType::Before) typeStr = "Before hooking ";
		if (type == HookType::Return) typeStr = "Return hooking ";
		if (type == HookType::Replace) typeStr = "Replacing ";

		if (type != HookType::Monitor)
			LOG_I("%s 0x%x %s with handler function 0x%x", typeStr.c_str(), (DWORD)originalFunc, name.c_str(), (DWORD)handlerFunc)
		else
			LOG_I("Adding monitoring call for %s (%x0x)", name, originalFunc);

		if (type == HookType::Return)
		{
			return HookManager::ReturnHook(name, originalFunc, handlerFunc);
		}
		else if (type == HookType::Monitor)
		{
			char* text = new char[255];
			snprintf(text, 255, "%s (0x%X2) was called.\n\0", name.c_str(), (DWORD)originalFunc);

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
			patch->Before(logFunc);
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
			if (patch->warn.length() != 0)
				Log::Debug(LogModule::Hooking, "Warn: %s", patch->error.c_str());

			handlerFunc = trampoline;
		}
		else if (type == HookType::Before)
		{
			// Call handler before original func while preserving registers for the original function 
			// (and with right calling convention stack will be fine after).
			DWORD trampoline = HookManager::GetFreeTrampoline();
			if (trampoline == NULL)
				return NULL;

			// 4 * 8 for all registers
			DWORD regspace = Allocate(32);

			auto patch = new MemoryPatch(150);
			auto convention = CallingConvention::stdcall;

			patch->SaveRegisters(regspace);
			patch->Before(handlerFunc);
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

			handlerFunc = patch->address;
		}

		if(type == HookMethod::Detours)
			return HookManager::DetourHook(originalFunc, handlerFunc, type);

		return NULL;
	}

	Hook* HookManager::DetourHook(DWORD originalFunc, DWORD handlerFunc, HookType type)
	{
		if (originalFunc == 0)
			LOG_E("Original function provided is null");
		if (handlerFunc == 0)
			LOG_E("Handler function provided is null");

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
		if (offset == -1)
		{
			LOG_D("Unable to determine offset");
			return NULL;
		}

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

		auto hook = new Hook("", (uintptr_t)originalFunc, (uintptr_t)handlerFunc, trampolineAddr, offset, type);
		HookManager::hooks.insert(std::make_pair((uintptr_t)handlerFunc, *hook));

		HookManager::LogStatus();
		return hook;
	}

	DWORD HookManager::GetDLLFunction(const std::string& dllName, const std::string& funcName)
	{
		DWORD funcPtr = NULL;
		HMODULE hDll = LoadLibraryA(dllName.c_str());
		if (hDll == NULL)
			return 0;
		funcPtr = (DWORD)GetProcAddress(hDll, funcName.c_str());
		FreeLibrary(hDll);
		return funcPtr;
	}

	void HookManager::RemoveHooks()
	{
		LOG_D("Removing all active hooks (%d)", HookManager::hooks.size());
		for (auto &hook : HookManager::hooks)
			HookManager::RemoveHook(&hook.second);

		HookManager::LogStatus();
	}

	void HookManager::RemoveHook(Hook* hook)
	{
		
		LOG_D("Removing 0x%x hook (hooked to 0x%x)", hook->originalFunc, hook->hookFunc);

		if(!hook->type == HookType::Return)
			SafeMemCpy((void*)hook->originalFunc, (void*)hook->trampoline, hook->patchSize);
		
		HookManager::freeTrampolines.insert(hook->trampoline);
		freeTrampolines.insert(hook->trampoline);
		FlushInstructionCache(GetCurrentProcess(), (LPCVOID)hook->originalFunc, hook->patchSize);

		// Restore other overwritten code, like in end hooks.
		if (hook->type == HookType::Return)
		{
			for (auto& kv : hook->overwrittenCode)
			{
				LOG_D("Restoring overwritten instructions for 0x%x.",hook->originalFunc);
				DWORD offset = (DWORD)kv.first;
				CodeBytes* cb = kv.second;
				SafeWrite((DWORD)offset, cb->bytes, cb->size);
			}
		}
		HookManager::hooks.erase(hook->hookFunc);
	}

	void HookManager::LogStatus()
	{
		LOG_D("Current active hooks: %d, free trampolines: %d, patches written: %d", 
			HookManager::hooks.size(), HookManager::freeTrampolines.size(), HookManager::patches.size());
	}

	Hook* HookManager::ReturnHook(const std::string& functionName, DWORD handlerFunc)
	{
		return HookManager::HookFunction(functionName, handlerFunc, HookType::Return);
	}

	Hook* HookManager::ReturnHook(const std::string& functionName, DWORD originalFunc, DWORD handlerFunc)
	{
		if (originalFunc == 0)
		{
			LOG_E("Null entrypoint.");
			return NULL;
		}

		unsigned int instructionCount;
		std::vector<_DecodedInst> disassembled;

		auto bytes = SafeRead((DWORD)originalFunc, 4096);
		DisAssembler disAsm;
		auto result = disAsm.DisAssemble(bytes, 4096, disassembled);

		instructionCount = disassembled.size();

		bool done = false;
		bool firstReturn = true;
		
		auto funcAnalyzer = new FunctionAnalyzer(originalFunc, disassembled);

		if (funcAnalyzer->endOfFunction == 0)
		{
			LOG_E("Unable to find where function ends.");
			return NULL;
		}

		bool relocationNeeded = false;
		auto rets = funcAnalyzer->FindByMnemonic("RET");

		if (rets.size() == 0)
		{
			LOG_D("No RET instructions found");
			return NULL;
		}

		if (rets.size() > 1)
		{
			LOG_D("More than one RET instruction found, we need to relocate.");
			relocationNeeded = true;
		}

		DWORD retOffset = ((DWORD)(funcAnalyzer->entryPoint + rets.front().offset));
		
		LOG_D("First RET offset found at 0x%x", retOffset);
		LOG_D("Found %d RET instructions.", rets.size());

		int saveSize = funcAnalyzer->endOfFunction - retOffset;
		byte* savedCode = SafeRead(retOffset, saveSize);
		CodeBytes* cb = new CodeBytes;
		cb->bytes = savedCode;
		cb->size = saveSize;
		Hook* hook = new Hook(functionName, (DWORD)originalFunc, (DWORD)handlerFunc, 0, cb->size, HookType::Return);
		hook->addOverwrittenCode(retOffset, cb);

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

						relocationNeeded = true;

						auto asmCode = disAsm.GetAsm(funcAnalyzer->disassembled, i, 3);
						for (auto& line : *asmCode)
						{
							Log::Debug(LogModule::Hooking, "%s", line.c_str());
						}

						return NULL;
					}
				}
			}
			auto patch = new MemoryPatch(5, (DWORD)(funcAnalyzer->entryPoint + ret.offset));
			patch->Jmp(handlerFunc);
			patch->Write();
		}

		HookManager::hooks.insert(std::make_pair((uintptr_t)hook->hookFunc, *hook));

		return hook;
	};
}
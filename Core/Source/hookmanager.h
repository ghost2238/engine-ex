#pragma once

#include "debug.h"
#include "utils.h"
#include "hook.h"
#include "memory.h"

#include "../Lib/fmt/format.h"

#include <iostream>
#include <set>
#include <map>
#include <iterator>
#include <windows.h>

namespace EngineEx
{
	enum EndHookError
	{
		Success = 0,
		NoFunctionEnd = 1,
		NullEntryPoint = 2,
		DisassembleFailed = 3,
		UnableToRelocate = 4
	};

	class HookManager
	{
	public:
		static void Init();
		static bool IsAlreadyHooked(DWORD originalFunc);
		static DWORD GetDLLFunction(const std::string& dllName, const std::string& funcName);
		static Hook* CreateBeforeHook(unsigned long originalFunc, unsigned long handlerFunc);
		static Hook* CreateEndHook(const std::string& functionName, DWORD hookFunction);
		static Hook* CreateEndHook(std::string name, DWORD entryPoint, DWORD hookFunction);
		static Hook* ReplaceFunction(unsigned long originalFunc, unsigned long handlerFunc);
		static Hook* MonitorCalls(unsigned long originalFunc, const char* name);
		static Hook* MonitorCalls(const std::string & functionName);
		static void RemoveHook(Hook* hook);
		static void RemoveEndHook(DWORD entryPoint);
		static void RemoveHooks();
		static void LogStatus();

		static std::map<std::string, FunctionSymbol> functionSymbols;
	private:
		static Hook* CreateHook(DWORD originalFunc, DWORD handlerFunc);
		static uintptr_t GetFreeTrampoline();
		static std::map<uintptr_t, Hook> HookManager::hooks;
		static std::map<uintptr_t, MemoryPatch> HookManager::patches;
		static std::set<uintptr_t> freeTrampolines;
		static void __stdcall LogFunc(char* text);
	};
}
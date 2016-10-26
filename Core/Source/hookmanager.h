#pragma once

#include "debug.h"
#include "utils.h"
#include "hook.h"
#include "memory.h"

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

		static Hook* HookFunction(const std::string& name, DWORD originalFunc, DWORD handlerFunc, HookType type, HookMethod method);
		static Hook* HookFunction(const std::string& functionName, DWORD handlerFunc, HookType type, HookMethod method);
		static Hook* HookFunction(const std::string& functionName, DWORD handlerFunc, HookType type);
		static Hook* HookFunction(DWORD originalFunc, DWORD handlerFunc, HookType type);
		static Hook* HookFunction(DWORD originalFunc, DWORD handlerFunc, HookType type, HookMethod method);
		static Hook* ReturnHook(const std::string& functionName, DWORD handlerFunc);
		static Hook* ReturnHook(const std::string& functionName, DWORD entryPoint, DWORD handlerFunc);

		static Hook* MonitorCalls(unsigned long originalFunc);
		static Hook* MonitorCalls(const std::string& functionName);
		static void RemoveHook(Hook* hook);
		static void RemoveEndHook(DWORD entryPoint);
		static void RemoveHooks();
		static void LogStatus();

		static std::map<std::string, VariableSymbol> variables;
		static std::map<std::string, FunctionSymbol> functions;
	private:
		static Hook* DetourHook(DWORD originalFunc, DWORD handlerFunc, HookType type);
		static uintptr_t GetFreeTrampoline();
		static std::map<uintptr_t, Hook> HookManager::hooks;
		static std::map<uintptr_t, MemoryPatch> HookManager::patches;
		static std::set<uintptr_t> freeTrampolines;

		static void __stdcall LogFunc(char* text);
		static void __stdcall Disable();
	};
}
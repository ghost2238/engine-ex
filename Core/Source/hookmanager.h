#pragma once

#include "debug.h"
#include "utils.h"
#include "hook.h"
#include "memory.h"
#include "threading.h"
#include "../Lib/fmt/format.h"

#include <iostream>
#include <set>
#include <map>
#include <iterator>
#include <Windows.h>

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
		static void Log(const char* Text, ...);
		static bool IsAlreadyHooked(DWORD originalFunc);
		static DWORD GetDLLFunction(const char* dllName, const char* funcName);
		static Hook* CreateBeforeHook(unsigned long originalFunc, unsigned long handlerFunc);
		static EndHookError CreateEndHook(std::string name, DWORD entryPoint, DWORD hookFunction);
		static Hook* HookManager::ReplaceFunction(unsigned long originalFunc, unsigned long handlerFunc);
		static Hook* MonitorCalls(unsigned long originalFunc, const char* name);
		static void RemoveHook(Hook* hook);
		static void RemoveEndHook(DWORD entryPoint);
		static void RemoveHooks();
		static void LogStatus();
	private:
		static Hook* HookManager::CreateHook(DWORD originalFunc, DWORD handlerFunc);
		static uintptr_t GetFreeTrampoline();
		static std::map<uintptr_t, Hook> HookManager::Hooks;
		static std::set<uintptr_t> freeTrampolines;
		static void __stdcall LogFunc(char* text);
	};
}
#pragma once

#include "debug.h"
#include "utils.h"
#include "hook.h"
#include "memory.h"
#include "threading.h"
#include "../Lib/fmt/format.h"
#include "../Lib/NCodeHook/NCodeHook.h"
#include "../Lib/NCodeHook/NCodeHookInstantiation.h"

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
		static DWORD GetDLLFunction(const char* dllName, const char* funcName);
		static DWORD CreateBeforeHook(unsigned long originalFunc, unsigned long handlerFunc);
		static EndHookError CreateEndHook(std::string name, DWORD entryPoint, DWORD hookFunction);
		static DWORD ReplaceFunction(unsigned long originalFunc, unsigned long handlerFunc);
		static void MonitorCalls(unsigned long originalFunc, const char* name);
		static void RemoveHook(DWORD func);
		static void RemoveEndHook(DWORD entryPoint);
		static void RemoveHooks();
	private:
		static DWORD CreateHook(DWORD originalFunc, DWORD handlerFunc);
		static std::vector<Hook*> Hooks;
		static void __stdcall LogFunc(char* text);
	};
}
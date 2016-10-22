#pragma once

#include "utils.h"
#include "hook.h"
#include "memory.h"
#include "threading.h"
#include "../Lib/NCodeHook/NCodeHook.h"
#include "../Lib/NCodeHook/NCodeHookInstantiation.h"

namespace EngineEx
{
	
	#define DEBUG_HOOK(__text, ...) HookManager::Log(__text, __VA_ARGS__);

	enum EndHookError
	{
		Success = 0,
		NoFunctionEnd = 1,
		NullEntryPoint = 2,
		DisassembleFailed = 3,
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
		static void RemoveCallHook(DWORD func);
		static void RemoveEndHook(DWORD entryPoint);
		static void RemoveHooks();
	private:
		static std::vector<Hook*> Hooks;
		static void __stdcall LogFunc(char* text);
	};
}
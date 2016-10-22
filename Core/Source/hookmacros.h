#pragma once

#include <windows.h>

namespace EngineEx
{
	#define FUNC_BEFOREHOOK(__Name, __Func)  *(PDWORD)&Orig_##__Name = HookManager::CreateBeforeHook((DWORD)Offset_##__Name, ((DWORD)&__Func));
	#define FUNC_REPLACE(__Name, __Func) *(PDWORD)&Orig_##__Name = HookManager::ReplaceFunction((DWORD)Offset_##__Name, ((DWORD)&__Func));
	#define FUNC_ENDHOOK(__Name, __Func) *(PDWORD)&Orig_##__Name = HookManager::CreateEndHook("Endhook##__Name##",(DWORD)Offset_##__Name, ((DWORD)&__Func));
}
#define DECL_OFFSETS

#include "hooks.h"
#include "appmain.h"

bool AppMain()
{
	//HookManager::ReplaceFunction(HookManager::GetDLLFunction("User32", "MessageBoxA"), (DWORD)&MsgBoxA);
	//MessageBoxA(0, "Calling MessageBoxA... this should be only be visible in console.", "Test", 0);

	//HookManager::MonitorCalls((DWORD)Offset_FOClient_AddMess, "FOClient_AddMess");
	//HookManager::MonitorCalls((DWORD)Offset_WriteLog, "WriteLog");

	/*
	HookManager::RemoveHook(hook);
	hook = HookManager::ReplaceFunction((DWORD)Offset_FOClient_AddMess, (DWORD)&AddMessTest);
	hook = HookManager::ReplaceFunction((DWORD)Offset_FOClient_AddMess, (DWORD)&AddMessTest);
	hook = HookManager::ReplaceFunction((DWORD)Offset_FOClient_AddMess, (DWORD)&AddMessTest);
	HookManager::RemoveHook(hook);
	HookManager::RemoveHook(hook);
	*/

	// Uses JSON symbol table to resolve the offset.

	//auto a = HookManager::MonitorCalls("FOClient_AddMess");

	//HookManager::CreateBeforeHook((DWORD)Offset_SpriteManager_DrawStr, (DWORD)&StringFinder);

	//auto before = HookManager::CreateBeforeHook((DWORD)Offset_FOClient_AddMess, (DWORD)&AddMessTest2);

	//auto hook = HookManager::ReplaceFunction(before->hookFunc, (DWORD)&AddMessTest);
	//auto monitor = HookManager::MonitorCalls(hook->hookFunc,  "hehe");

	/*
	hook = HookManager::ReplaceFunction((DWORD)Offset_FOClient_AddMess, (DWORD)&AddMessTest);
	hook = HookManager::ReplaceFunction((DWORD)Offset_FOClient_AddMess, (DWORD)&AddMessTest);
	hook = HookManager::ReplaceFunction((DWORD)Offset_FOClient_AddMess, (DWORD)&AddMessTest);*/

	//HookManager::RemoveHooks();
	
	while (true) { Sleep(1000); }
	return true;
}
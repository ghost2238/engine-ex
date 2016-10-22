#define DECL_OFFSETS

#include "hooks.h"
#include "appmain.h"

FILE* stdfile;

bool AppMain()
{
	HookManager::Init();
	ConfigManager::Init();

	AllocConsole();
	AttachConsole(ATTACH_PARENT_PROCESS);
	freopen_s(&stdfile, "CONOUT$", "w", stdout);

	*(byte*)Offset_FoDebugSprites = 1;
	FoClient = ((FOClient*)((DWORD)Static_FoClient));
	FoSoundManager = (void*)((DWORD)FoClient + 0x8FC);
	

	HookManager::ReplaceFunction(HookManager::GetDLLFunction("User32", "MessageBoxA"), (DWORD)&MsgBoxA);
	MessageBoxA(0, "Testing... this should be only be visible in console.", "Test", 0);

	//HookManager::MonitorCalls((DWORD)Offset_FOClient_AddMess, "FOClient_AddMess");
	//HookManager::MonitorCalls((DWORD)Offset_FOMsg_GetStr, "FOMsg_GetStr");
	//HookManager::MonitorCalls((DWORD)Offset_WriteLog, "WriteLog");

	HookManager::MonitorCalls((DWORD)Offset_FOClient_DrawIfaceLayer, "FOClient_DrawIfaceLayer");

	FUNC_BEFOREHOOK(WriteLog, OnWriteFileLog);
	//FUNC_BEFOREHOOK(FOMsg_GetStr, GetMsgTest);
	FUNC_BEFOREHOOK(FOClient_AddMess, AddMessTest);
	//FUNC_BEFOREHOOK(FOClient_AddMess, AddMessTest);
	FUNC_ENDHOOK(FOMsg_GetStr2, EndHookGetMsg);

	return true;
}

bool IsFinished()
{
	return false;
}

bool AppFinish()
{
	fflush(stdfile);
	return true;
}
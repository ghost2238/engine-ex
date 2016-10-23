#define DECL_OFFSETS

#include "hooks.h"
#include "appmain.h"

FILE* stdfile;

bool AppMain()
{
	AllocConsole();
	AttachConsole(ATTACH_PARENT_PROCESS);
	freopen_s(&stdfile, "CONOUT$", "w", stdout);
	
	HookManager::ReplaceFunction(HookManager::GetDLLFunction("User32", "MessageBoxA"), (DWORD)&MsgBoxA);
	MessageBoxA(0, "Calling MessageBoxA... this should be only be visible in console.", "Test", 0);

	HookManager::MonitorCalls((DWORD)Offset_FOClient_AddMess, "FOClient_AddMess");
	HookManager::MonitorCalls((DWORD)Offset_WriteLog, "WriteLog");

	HookManager::RemoveHook((DWORD)Offset_WriteLog);

	FoClient = ((FOClient*)((DWORD)Static_FoClient));
	FUNC_ENDHOOK(FOClient_ConsoleDraw, OnDrawGame);

	//FUNC_BEFOREHOOK(WriteLog, OnWriteFileLog);
	//FUNC_BEFOREHOOK(FOMsg_GetStr, GetMsgTest);
	FUNC_BEFOREHOOK(FOClient_AddMess, AddMessTest);
	
	while (true) { Sleep(1000); }

	fflush(stdfile);
	return true;
}
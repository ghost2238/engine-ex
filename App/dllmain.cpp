// EngineEx
//#define DECL_OFFSETS

#include "appmain.h"

DWORD WINAPI DLLThread(LPVOID);
HMODULE hModule;
DWORD threadID;

void init(HMODULE hDLL)
{
	hModule = hDLL;
	DisableThreadLibraryCalls(hDLL);
	CreateThread(NULL, NULL, &DLLThread, NULL, NULL, &threadID);
}

void finish()
{
	DEBUG_DLL("Finishing...\n");
	//HookManager::RemoveHooks();
	//Sleep(10000); // Prevent crash from hooked code. 
	FreeLibraryAndExitThread(hModule, 0);
}

DWORD WINAPI DLLThread(LPVOID)
{
	AppMain();
	while (!IsFinished()) { Sleep(250); }
	finish();
	FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

extern "C" BOOL APIENTRY DllMain(HMODULE hDLL, DWORD Reason, LPVOID Reserved)
{
    switch(Reason)
    {
		case DLL_PROCESS_ATTACH:
			init(hDLL);
			break;
		case DLL_THREAD_ATTACH:
		case DLL_PROCESS_DETACH:
			break;
		case DLL_THREAD_DETACH:
			finish();
			break;
    }
    return TRUE;
}
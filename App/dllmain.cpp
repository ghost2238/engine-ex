#include "appmain.h"

DWORD WINAPI DLLThread(LPVOID);
HMODULE hModule;
DWORD threadID;

void init(HMODULE hDLL)
{
	hModule = hDLL;
	DisableThreadLibraryCalls(hDLL);
	CreateThread(NULL, NULL, &DLLThread, NULL, NULL, &threadID);
	Log::Init(LoggingLevel::Info, true, "EngineEx.log");
	ConfigManager::Init();
	HookManager::Init();
}


void finish()
{
	Log::Info(LogModule::Global, "Finishing...");
	FreeLibraryAndExitThread(hModule, 0);
}

DWORD WINAPI DLLThread(LPVOID)
{
	AppMain();
	finish();
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
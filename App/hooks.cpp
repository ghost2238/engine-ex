#include "hooks.h"

bool firstDraw = true;

typedef void(__thiscall *FOClient_AddMess)(int, int, const char*);
void AddLogMessage(int type, char* str)
{
	auto func = HookManager::functionSymbols["FOClient_AddMess"];
	FOClient_AddMess f = (FOClient_AddMess)(func.offset);
	int foclient = *(int*)0x02F914A0;
	f(foclient, type, str);	
}

void OnDrawConsole()
{
	int textY = 10;
	if (firstDraw)
	{
		AddLogMessage(MSGTYPE_DOT_RED, "Loaded EngineEx in FOnline.");
		firstDraw = false;
	}
}

char* __stdcall GetMsgTest(int id, int dummy)
{
	printf("%d\n", id);
	return "";
}

int __stdcall MsgBoxA(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
	printf("Redirected to stdout: %s\n", (char*)lpText);
	return 0;
}

void __fastcall StringFinder(const char* t1, const char* t2, const char* t3, const char* t4, const char* t5, const char* t6)
{
	if (rand() % 30 == 0)
		printf("%s\n", t2);
	//if (rand() % 30 == 0)
	//	printf("no crash");
}

void __stdcall AddMessTest2(int msgType, const char* text)
{
	//printf("%s\n", text);
}

void __stdcall AddMessTest(int msgType, const char* text)
{
	printf("%s\n", text);

	// FOClient->AddMess(int, char*)
	/*typedef void(__thiscall *MsgCall)(int, int, const char*);
	MsgCall f = (MsgCall)((DWORD)Orig_FOClient_AddMess);
	f(r_ecx, msgType, text);
	*/
	
	//f(msgType, text);
	//void (*func)(int, const char*) = (void(*)(int, const char*))((DWORD)Offset_FOClient_AddMess + 5);
	//func(msgType, text);
	//((FuncPtr*))(msgType, text);
}

void __stdcall OnWriteFileLog(char* logLine, int dummy)
{
	AddLogMessage(MSGTYPE_DOT_LIGHTGREEN, logLine);
}
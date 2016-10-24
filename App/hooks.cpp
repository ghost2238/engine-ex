#include "hooks.h"

bool firstDraw = true;

typedef void(__thiscall *FOClient_AddMess)(int, int, const char*);
void FOAddLogMessage(int type, char* str)
{
	auto func = HookManager::functions["FOClient_AddMess"];
	auto var = HookManager::variables["FOClient"];

	FOClient_AddMess f = (FOClient_AddMess)(func.offset);
	int foclient = *(int*)var.offset;
	f(foclient, type, str);	
}

struct Rect { int x1; int y1; int x2; int y2; };
typedef void(__thiscall *SpriteManager_DrawStr)(unsigned int, Rect*, const char*, int, char*, int);
void __stdcall FODrawText(int x, int y, int flags, char* color, int font, char* text, ...)
{
	char buffer[512];
	va_list args;
	va_start(args, text);
	vsprintf_s(buffer, 512, text, args);
	va_end(args);
	Rect pos;

	pos.x1 = x;
	pos.y1 = y;
	pos.x2 = 1200;
	pos.y2 = 1200;

	auto var = HookManager::variables["SpriteManager"];
	auto func = HookManager::functions["SpriteManager_DrawStr"];

	SpriteManager_DrawStr f = (SpriteManager_DrawStr)(func.offset);

	f(var.offset, &pos, buffer, flags, color, font);
}


void OnDrawConsole()
{
	if (firstDraw)
	{
		FOAddLogMessage(MSGTYPE_DOT_RED, "Loaded EngineEx in FOnline.");
		firstDraw = false;
	}
	int y = 10;
	for(int i=4;i<9;i++)
		FODrawText(10, y+=30, FT_BORDERED, "FF00FF00", i, "|0xFFFFFF00 Hello World!");
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
	FOAddLogMessage(MSGTYPE_DOT_LIGHTGREEN, logLine);
}
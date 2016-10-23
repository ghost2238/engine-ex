#include "hooks.h"

char* filestr;
char* netstr;
char* logstr;
int id;
int num;
int lookType;
CritterCl* cr;

bool done = false;
bool firstDraw = true;
char args[10][40];

#define DRAW_DEBUGTEXT(__text, ...) foclient_drawText(10, textY, FT_BORDERED, "FF00FF00", 8,  __text, __VA_ARGS__); textY = textY+30;
void OnDrawGame()
{
	int textY = 10;
	if (firstDraw)
	{
		foclient_writeToLog(MSGTYPE_DOT_RED, "Loaded FOClientEx.");
		firstDraw = false;
	}
	DRAW_DEBUGTEXT("|0xFF00FF00 It's working!");
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

void __stdcall AddMessTest(int msgType, const char* text)
{
	//int r_ecx;
	//{ __asm mov r_ecx, ecx }

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
	//EventHandler::OnWriteFileLog(logLine);
	foclient_writeToLog(MSGTYPE_DOT_RED, logLine);
}

char* retval;
__declspec(naked) void EndHookGetMsg()
{ 
	__asm { __asm mov retval, eax __asm pushad }
 
	cout << retval;
 
	__asm { __asm popad __asm RET 4 } 
}
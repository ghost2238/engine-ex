#ifndef __HOOKS__
#define __HOOKS__

#include "appmain.h"
#include "Game/defines.h"

#define IS_CMD(__str) (strstr(sentStr, __str) != NULL)

void readPointers();

//void __stdcall ReplaceTest();
void __cdecl CDeclTest(int a, int b, int c);
void __stdcall AddMessTest(int msgType, const char* text);
void  __stdcall OnWriteFileLog(char* logLine, int dummy);
char* __stdcall GetMsgTest(int id, int dummy);
void OnDrawGame();
bool OnNetSendText(char* str);
bool isDone();

int __stdcall MsgBoxA(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);

void HookedCustomFileWrite();
void HookedCustomFOClient_AddMess();
void HookedCustomFOClient_FmtCritLook();
void HookedCustomFOClient_Net_SendText();
void HookedCustomFOClient_FmtCritLook();
void HookedCustomFOMsg_GetStr();
void EndHookGetMsg();

#endif
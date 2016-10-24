#ifndef __HOOKS__
#define __HOOKS__

#include "appmain.h"
#include "Game/defines.h"

void __stdcall AddMessTest(int msgType, const char* text);
void __stdcall AddMessTest2(int msgType, const char* text);
void __fastcall StringFinder(const char* t1, const char* t2, const char* t3, const char* t4, const char* t5, const char* t6);
void  __stdcall OnWriteFileLog(char* logLine, int dummy);
char* __stdcall GetMsgTest(int id, int dummy);
void OnDrawConsole();

#endif
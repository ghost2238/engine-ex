#include "../structures.h"
#include "../offsets.h"
#include "defines.h"

#include <stdio.h>
#include <windows.h>

#ifndef __FOCLIENT__
#define __FOCLIENT__

DECL_PTR(FOClient* FoClient); // Ptr to FOClient class, read from FoClientStaticPtr
DECL_PTR(void* FoSoundManager);
void foclient_init();

// Helpers to make ASM calling less verbose.
#define CALL_ASM1(__func, __arg1)														__asm { __asm pushad __asm push __arg1 __asm call __func __asm popad }

#define CALL_FOCLIENT(__func, __arg)													__asm { __asm push __arg __asm call __func __asm popad }
#define CALL_FOCLIENT2(__func, __arg1, __arg2)											__asm { __asm pushad __asm mov ecx, FoClient __asm push __arg2 __asm push __arg1 __asm call __func __asm popad }

#define CALL_FOSPRITE4(__func, __arg1, __arg2, __arg3,  __arg4)							__asm { __asm pushad __asm mov ecx, Offset_FoSpriteManager __asm push __arg4 __asm push __arg3 __asm push __arg2 __asm push __arg1 __asm call __func __asm popad }
#define CALL_FOSPRITE5(__func, __arg1, __arg2, __arg3,  __arg4, __arg5)					__asm { __asm pushad __asm mov ecx, Offset_FoSpriteManager __asm push __arg5 __asm push __arg4 __asm push __arg3 __asm push __arg2 __asm push __arg1 __asm call __func __asm popad }

#define WRITE_GAMELOG(__text, ...) foclient_writeToLog(MSGTYPE_DOT_RED, __text, __VA_ARGS__)

void foclient_useskill(int crId, int skill);
void foclient_writeToLog(int msgType, const char* Text, ...);
void foclient_drawText(int x, int y, int flags, char* color, int font, char* text, ...);
void foclient_drawSprite(int spriteId, int x, int y, int c);

int crittercl_getParam(CritterCl* cl, int param);

#endif
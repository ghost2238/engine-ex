#include "foclient.h"

int i=0;
struct Rect
{
	int x1;
	int y1;
	int x2;
	int y2;
};

Rect* pos = new Rect();

void foclient_init()
{

}

void foclient_writeToFileLog(char* text)
{
	CALL_ASM1(GET_OFFSET(WriteLog), text);
}

void foclient_Net_SendText(char* msg, int how_say) { CALL_FOCLIENT2(GET_OFFSET(FOClient_Net_SendText), msg, how_say); }
void foclient_WMMove(int x, int y) { CALL_FOCLIENT3(GET_OFFSET(FOClient_Net_SendRuleGlobal), 1, x, y); }
void foclient_WMEncEnter()		   { CALL_FOCLIENT3(GET_OFFSET(FOClient_Net_SendRuleGlobal), 3, 0, 0); }
void foclient_WMLocEnter()
{
	// Bruteforce correct id.
	for(i=1;i<100;i++)
	{
		CALL_FOCLIENT3(GET_OFFSET(FOClient_Net_SendRuleGlobal), 3, i, 0);
	}
}

void foclient_moveToHex(int hexX, int hexY, int run)
{
	int tick = 0;
	__asm { call Offset_WinMM_timeGetTime
			mov tick, eax }
	CALL_FOCLIENT7(GET_OFFSET(FOClient_SetAction2), ACTION_TYPE_MOVE, hexX, hexY, run, 0, 1, tick);
}

void foclient_useskill(int crId, int skill)    { CALL_FOCLIENT7(GET_OFFSET(FOClient_SetAction2), ACTION_TYPE_USE_SKILL, skill, crId, 0, 0, 0, 0); }
void foclient_rotate(int right)				   { CALL_FOCLIENT7(GET_OFFSET(FOClient_SetAction2), ACTION_TYPE_ROTATE, right, 0, 0, 0, 0, 0); }
void foclient_attack(int crId, int attackType) { CALL_FOCLIENT7(GET_OFFSET(FOClient_SetAction2), ACTION_TYPE_ATTACK, 0, attackType, 2, crId, 0, 0); }

char buffer[512];
void foclient_drawText(int x, int y, int flags, char* color, int font, char* text, ...)
{
	va_list args;
    va_start(args, text);
    vsprintf_s(buffer, 512, text, args);
    va_end(args);

	char* ptrBuf = buffer;
	pos->x1 = x;
	pos->y1 = y;
	pos->x2 = 1200; // Wrap area
	pos->y2 = 1200; // If smaller will wrap text to nextline
	CALL_FOSPRITE5(GET_OFFSET(SpriteManager_DrawStr), pos, ptrBuf, flags, color, font);
}

// msgType 1-4 valid.
void foclient_writeToLog(int msgType, const char* text, ...)
{
	va_list args;
    va_start(args, text);
    vsprintf_s(buffer, 512, text, args);
    va_end(args);

	char* ptrBuf = buffer;
	CALL_FOCLIENT2(GET_OFFSET(FOClient_AddMess), msgType, ptrBuf);
}

int crittercl_getParam(CritterCl* cl, int param)
{
	int adr = (DWORD)cl;
	int result;
	__asm { pushad
			mov ecx, adr
			push param
			call Offset_CritterCl_GetParam
			mov result, eax
			popad
	}
	return result;
}

void foclient_drawSprite(int spriteIndex, int x, int y, int c)
{
	CALL_FOSPRITE4(GET_OFFSET(SpriteManager_DrawSprite2), spriteIndex, x, y, c);
}
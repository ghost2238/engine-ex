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

void foclient_drawSprite(int spriteIndex, int x, int y, int c)
{
	CALL_FOSPRITE4(GET_OFFSET(SpriteManager_DrawSprite2), spriteIndex, x, y, c);
}
#pragma once

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "../Lib/diStorm/distorm.h"

namespace EngineEx
{
	void SafeWrite(DWORD address, byte* bytes, int size);
	void SafeWrite4(DWORD address, byte* bytes);
	void SafeWrite8(DWORD address, byte* bytes);
	void SafeWrite16(DWORD address, byte* bytes);
	void SafeWrite32(DWORD address, byte* bytes);

	void MemLog(const char* Text, ...);
	DWORD FindEmptySpace(int neededBytes, DWORD beginOffset);
	DWORD FindEmptySpace(int neededBytes);
	DWORD AllocateSpace(int neededBytes);
}
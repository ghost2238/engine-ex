#pragma once

#include <fstream>
#include <stdarg.h>

namespace EngineEx
{
	#ifdef DEBUG
		#define DEBUG_DLL(__text, ...) debug_file(".\\dlldebug.txt", __text, __VA_ARGS__);
	#else
		#define DEBUG_DLL(__text, ...)
	#endif

	void debug_file(char* file, const char* Text, ...);
}
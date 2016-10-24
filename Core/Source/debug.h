#pragma once

#include "../Lib/fmt/ostream.h"
#include "../Lib/fmt/format.h"

#include <fstream>
#include <stdio.h>
#include <stdarg.h>
#include <ctime>
#include <iomanip>
#include <windows.h>

namespace EngineEx
{
	enum LogModule
	{
		Global,
		Hooking,
		Memory,
		Utils
	};

	enum LoggingLevel
	{
		Trace,
		Debug,
		Info,
		Error,
		None
	};

	class Log
	{
		public:
			static LoggingLevel level;
			static char* filename;
			static bool logToFile;
			static FILE* stdfile;
			static FILE* logfile;

			static void OpenConsole();
			static void Init(LoggingLevel level, bool logToFile, char* filename);
			static void Log_(char* text, LogModule module, int Level);
			static void Info(LogModule module, char* text, ...);
			static void Trace(LogModule module, char* text, ...);
			static void Error(LogModule module, char* text, ...);
			static void Debug(LogModule module, char* text, ...);

	};

	
}
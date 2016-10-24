#include "debug.h"

namespace EngineEx
{
	#define __VA_ARGS(__buffer) char buffer[__buffer]; va_list args; va_start(args, text); \
					  vsprintf_s(buffer, __buffer, text, args); va_end(args);
	
	LoggingLevel Log::level;
	char* Log::filename;
	bool Log::logToFile;
	FILE* Log::stdfile;

	std::ofstream fs;

	void Log::Init(LoggingLevel level, bool logToFile, char* filename)
	{
		Log::logToFile = logToFile;
		Log::filename = filename;
		Log::level = level;

		if (logToFile)
			fs = std::ofstream(filename, std::ios_base::app);

		Log::OpenConsole();
		Log::Info(LogModule::Global, "Initialized loggging");
	}

	void Log::OpenConsole()
	{
		AllocConsole();
		AttachConsole(ATTACH_PARENT_PROCESS);
		freopen_s(&stdfile, "CONOUT$", "w", stdout);
	}

	void Log::Log_(char* text, LogModule module, int level)
	{
		if (level < Log::level) return;
		std::time_t t = std::time(nullptr);

		struct tm timeinfo;
		auto time = localtime_s(&timeinfo, &t);

		char* mod = "Global";
		if (module == LogModule::Hooking) mod = "Hooking";
		else if (module == LogModule::Memory) mod = "Memory";
		else if (module == LogModule::Utils)  mod = "Utils";

		auto str = fmt::format("{0} [{1}] {2} \n", std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S"), mod, text);
		printf(str.c_str());
		if (Log::logToFile) { fs << str.c_str(); fs.flush(); }
	}

	void Log::Info(LogModule module, char* text, ...)
	{
		__VA_ARGS(512);
		if (strcmp(buffer, "") == 0) Log_(text, module, LoggingLevel::Info);
		else Log_(buffer, module, LoggingLevel::Info);
	}

	void Log::Trace(LogModule module, char* text, ...)
	{
		__VA_ARGS(512);
		if (strcmp(buffer, "") == 0) Log_(text, module, LoggingLevel::Trace);
		else Log_(buffer, module, LoggingLevel::Trace);
	}

	void Log::Error(LogModule module, char* text, ...)
	{
		__VA_ARGS(512);
		if (strcmp(buffer, "") == 0) Log_(text, module, LoggingLevel::Error);
		else Log_(buffer, module, LoggingLevel::Error);
	}

	void Log::Debug(LogModule module, char* text, ...)
	{
		__VA_ARGS(512);
		if (strcmp(buffer, "") == 0) Log_(text, module, LoggingLevel::Debug);
		else Log_(buffer, module, LoggingLevel::Debug);
	}
}
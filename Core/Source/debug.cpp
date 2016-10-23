#include "debug.h"

namespace EngineEx
{
	#define __VA_ARGS(__buffer) char buffer[__buffer]; va_list args; va_start(args, text); \
					  vsprintf_s(buffer, __buffer, text, args); va_end(args);
	
	LoggingLevel Log::level;
	char* Log::filename;
	bool Log::logToFile;

	void Log::Init(LoggingLevel level, bool logToFile, char* filename)
	{
		Log::logToFile = logToFile;
		Log::filename = filename;
		Log::level = level;
	}

	void Log::Log_(char* text, LogModule module, int level)
	{
		if (level < Log::level) return;
		std::time_t t = std::time(nullptr);

		struct tm timeinfo;
		auto time = localtime_s(&timeinfo, &t);
		

		/*fmt::Color c;
		     if (level == LoggingLevel::Debug) c = fmt::Color::YELLOW;
		else if (level == LoggingLevel::Error) c = fmt::Color::RED;
		else if (level == LoggingLevel::Info)  c = fmt::Color::WHITE;
		else if (level == LoggingLevel::Trace) c = fmt::Color::WHITE;*/

		char* mod = "";
		if (module == LogModule::Hooking) mod = "Hooking";
		else if (module == LogModule::Memory)  mod = "Memory";
		else if (module == LogModule::Utils)  mod = "Utils";

		fmt::print("{0} [{1}] {2} \n", std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S"), mod, text);
	}

	void Log::Info(LogModule module, char* text, ...)
	{
		__VA_ARGS(512);
		Log_(buffer, module, LoggingLevel::Info);
	}

	void Log::Trace(LogModule module, char* text, ...)
	{
		__VA_ARGS(512);
		Log_(buffer, module, LoggingLevel::Trace);
	}

	void Log::Error(LogModule module, char* text, ...)
	{
		__VA_ARGS(512);
		Log_(buffer, module, LoggingLevel::Error);
	}

	void Log::Debug(LogModule module, char* text, ...)
	{
		__VA_ARGS(512);
		Log_(buffer, module, LoggingLevel::Debug);
	}
}
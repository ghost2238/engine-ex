#include "debug.h"

namespace EngineEx
{
	void debug_file(char* file, const char* Text, ...)
	{
		char buffer[4096];

		std::ofstream output(file, std::ios_base::app);
		va_list args;
		va_start(args, Text);
		vsprintf_s(buffer, 4096, Text, args);
		va_end(args);
		output << buffer;
		output.flush();
		output.close();
	}
}
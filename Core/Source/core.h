#include "debug.h"

#include "hook.h"
#include "hookmanager.h"

#include "config.h"

#include "utils.h"
#include "memory.h"

namespace EngineEx
{
	namespace Core
	{
		void Init(LoggingLevel level, bool logToFile, char* filename);
	}
}
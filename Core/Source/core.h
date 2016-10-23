#include "debug.h"

#include "hook.h"
#include "hookmanager.h"

#include "Config/configmanager.h"

#include "utils.h"
#include "memory.h"
#include "threading.h"

namespace EngineEx
{
	namespace Core
	{
		void Init(LoggingLevel level, bool logToFile, char* filename);
	}
}
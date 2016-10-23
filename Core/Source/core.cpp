#include "core.h"

namespace EngineEx
{
	namespace Core
	{
		void Init(LoggingLevel level, bool logToFile, char* filename)
		{
			Log::Init(level, logToFile, filename);
			ConfigManager::Init();
			HookManager::Init();
		}
	}
}
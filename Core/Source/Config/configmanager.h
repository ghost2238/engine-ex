#ifndef __CONFIGMANAGER__
#define __CONFIGMANAGER__

#include "../../Lib/JsonCPP/json.h"

class ConfigManager
{
	public:
		static void Init();
		static Json::Value ConfigManager::GetModuleConfig(char* module, char* var);
};

#endif
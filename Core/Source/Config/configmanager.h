#pragma once

#ifndef __CONFIGMANAGER__
#define __CONFIGMANAGER__

#include "../../Lib/JsonCPP/json.h"
#include "../debug.h"

#include <fstream>
#include <windows.h>
#include <stdio.h>

namespace EngineEx
{
	class ConfigManager
	{
	public:
		static void Init();
		static Json::Value ConfigManager::GetModuleConfig(char* module, char* var);
	};
}

#endif
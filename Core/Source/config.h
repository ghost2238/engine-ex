#pragma once

#ifndef __CONFIGMANAGER__
#define __CONFIGMANAGER__

#include "../Lib/JsonCPP/json.h"
#include "debug.h"

#include <fstream>
#include <windows.h>
#include <stdio.h>
#include <string>

namespace EngineEx
{
	class Config
	{
	public:
		static void Init();
		static Json::Value Config::Value(const std::string value);
	};
}

#endif
#pragma once

#ifndef __CONFIG__
#define __CONFIG__

#include "../Lib/JsonCPP/json.h"
#include "debug.h"

#include <fstream>
#include <stdio.h>
#include <string>
#include <windows.h>

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
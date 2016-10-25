#include "config.h"

namespace EngineEx
{
	Json::Value root;
	void Config::Init()
	{
		std::ifstream stream("./Core.json");
		if(stream.is_open())
			stream >> root;
		else
			Log::Error(LogModule::Global, "Error, unable to open Core.json");
	}

	Json::Value Config::Value(const std::string value)
	{
		if (root == NULL) return NULL;
		return root[value];
	}

}
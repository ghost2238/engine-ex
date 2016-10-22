#include <fstream>
#include <windows.h>
#include <stdio.h>
#include "configmanager.h"

Json::Value root;
void ConfigManager::Init()
{
	std::ifstream stream("./Core.json");
	if(stream.is_open())
		stream >> root;
	else
		printf("[ConfigManager] Error, unable to open Core.json");
}

Json::Value ConfigManager::GetModuleConfig(char* module, char* var) 
{
	if (root == NULL) return NULL;

	return root["modules"][module][var];
}
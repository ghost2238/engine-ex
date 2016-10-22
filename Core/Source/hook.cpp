#include "hook.h"


namespace EngineEx
{
	Hook::Hook(const std::string name, DWORD originalFunction, DWORD hookHandler)
	{
		this->name = name;
		this->originalFunction = originalFunction;
		this->hookHandler = hookHandler;
	}
}
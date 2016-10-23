#include "hook.h"


namespace EngineEx
{
	Hook::Hook(const std::string name, DWORD originalFunction, DWORD hookHandler)
	{
		this->name = name;
		this->originalFunctionOffset = originalFunction;
		this->handlerFunctionOffset = hookHandler;
	}

	Hook::Hook(DWORD originalFunction, DWORD hookHandler)
	{
		this->name = "";
		this->originalFunctionOffset = originalFunction;
		this->handlerFunctionOffset = hookHandler;
	}
}
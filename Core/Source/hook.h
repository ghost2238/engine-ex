#pragma once

#include <windows.h>
#include <vector>
#include <map>
#include "utils.h"

namespace EngineEx
{
	enum HookType
	{
		Before,
		End,
		Replace,
		Monitor
	};

	class Hook
	{
	public:
		Hook(const std::string name, DWORD originalFunction, DWORD hookHandler);
		Hook(DWORD originalFunction, DWORD hookHandler);

		void addOverwrittenCode(DWORD offset, CodeBytes* bytes) { this->overwrittenCode.insert(std::pair<DWORD, CodeBytes*>(offset, bytes)); }
		std::map<DWORD, CodeBytes*> overwrittenCode;

		DWORD originalFunctionOffset;
		DWORD trampolineOffset;
		DWORD handlerFunctionOffset;
		HookType type;
		std::string name;
	};
}
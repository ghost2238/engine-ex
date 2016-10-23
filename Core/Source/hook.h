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
		Hook(const std::string name, uintptr_t original, uintptr_t hookFunc, uintptr_t trampoline, uintptr_t patchSize);

		void addOverwrittenCode(DWORD offset, CodeBytes* bytes) { this->overwrittenCode.insert(std::pair<DWORD, CodeBytes*>(offset, bytes)); }
		std::map<DWORD, CodeBytes*> overwrittenCode;
		uintptr_t originalFunc;
		uintptr_t hookFunc;
		uintptr_t patchSize;
		uintptr_t trampoline;
		HookType type;
		std::string name;
	};
}
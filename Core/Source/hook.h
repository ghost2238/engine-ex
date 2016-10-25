#pragma once

#include "utils.h"

#include <windows.h>

#include <vector>
#include <map>

namespace EngineEx
{
	enum Arch
	{
		x86_32,
		x86_64
	};

	enum HookMethod
	{
		Detours
	};

	enum HookType
	{
		Before,
		Return,
		Replace,
		Disable,
		Monitor
	};

	class Hook
	{
	public:
		Hook(const std::string name, uintptr_t original, uintptr_t hookFunc, uintptr_t trampoline, uintptr_t patchSize, HookType type);

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
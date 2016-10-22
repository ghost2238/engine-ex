#pragma once

#include <windows.h>
#include <vector>
#include <map>

namespace EngineEx
{
	struct CodeBytes
	{
		byte* bytes;
		short size;
	};

	class Hook
	{
	public:
		Hook(const std::string name, DWORD originalFunction, DWORD hookHandler);

		void addOverwrittenCode(DWORD offset, CodeBytes* bytes) { this->overwrittenCode.insert(std::pair<DWORD, CodeBytes*>(offset, bytes)); }
		DWORD getOriginalFunction() { return originalFunction; }
		DWORD getHookHandler() { return hookHandler; }
		std::string getName() { return name; }
		std::map<DWORD, CodeBytes*> overwrittenCode;

	private:
		DWORD originalFunction;
		DWORD hookHandler;
		std::string name;
	};
}
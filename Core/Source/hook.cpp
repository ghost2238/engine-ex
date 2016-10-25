#include "hook.h"

namespace EngineEx
{
	Hook::Hook(const std::string name, uintptr_t original, uintptr_t hookFunc, uintptr_t trampoline, uintptr_t patchSize, HookType t)
		: name(name), originalFunc(original), hookFunc(hookFunc), trampoline(trampoline), patchSize(patchSize), type(t)
	{

	}
}
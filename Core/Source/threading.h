#pragma once

#include <windows.h>
#include <tlhelp32.h>

namespace EngineEx
{
	void ResumeMainThread();
	void SuspendMainThread();
	HANDLE GetMainThread();
}
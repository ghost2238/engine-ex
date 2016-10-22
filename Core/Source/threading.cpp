
#include "threading.h"

namespace EngineEx
{
	void ResumeMainThread()
	{
		ResumeThread(GetMainThread());
	}

	void SuspendMainThread()
	{
		SuspendThread(GetMainThread());
	}

	HANDLE GetMainThread()
	{
		HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
		if (hThreadSnapshot == INVALID_HANDLE_VALUE) {
			//throw std::runtime_error("GetMainThreadId failed");
		}
		THREADENTRY32 tEntry;
		tEntry.dwSize = sizeof(THREADENTRY32);
		DWORD result = 0;
		DWORD currentPID = GetCurrentProcessId();
		for (BOOL success = Thread32First(hThreadSnapshot, &tEntry);
			!result && success && GetLastError() != ERROR_NO_MORE_FILES;
			success = Thread32Next(hThreadSnapshot, &tEntry))
		{
			if (tEntry.th32OwnerProcessID == currentPID) {
				result = tEntry.th32ThreadID;
			}
		}

		return OpenThread(THREAD_SUSPEND_RESUME, false, result);

	}


}
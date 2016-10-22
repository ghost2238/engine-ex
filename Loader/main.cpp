#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>

#define CREATE_THREAD_ACCESS (PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ)

FILE* f;

BOOL FileExists(LPCTSTR szPath)
{
	DWORD dwAttrib = GetFileAttributes(szPath);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
		 !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

void Log(const char* text, ...)
{
	if(f == NULL) return;
    char buffer[4096];
    va_list args;
    va_start(args, text);
    vsprintf_s(buffer, 4096, text, args);
    va_end(args);
	fputs(buffer, f);
}


void MsgError(char* error, ...) {
	va_list args;
	char buffer[4096];
	va_start(args, error);
	vsprintf_s(buffer, 4096, error, args);
	va_end(args);

	MessageBox(NULL, buffer, "Loader", MB_ICONERROR); 
	Log("Error: %s\n", buffer);
}

char* GetSetting(char* setting, char* def)
{
	char* buf = new char[255];
	GetPrivateProfileString("Loader", setting, def, buf, 255, ".\\loader.ini"); 
	return buf;
}

DWORD FindProcessId(char* processName)
{
   DWORD processId = 0;
   PROCESSENTRY32 pe;
   HANDLE thSnapshot;
   BOOL retval;

   thSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
   if(thSnapshot == INVALID_HANDLE_VALUE)
   {
	  MsgError("Error: unable to create toolhelp snapshot");
	  return false;
   }
   pe.dwSize = sizeof(PROCESSENTRY32);
   retval = Process32First(thSnapshot, &pe);
   while(retval)
   {
	  if(strstr(pe.szExeFile, processName) != NULL)
	  {
		 processId = pe.th32ProcessID;
	  }
	  retval    = Process32Next(thSnapshot,&pe);
	  pe.dwSize = sizeof(PROCESSENTRY32);
   }
   return processId;
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	char* process_name;
	char* dll_name;

	bool debug; // Debug to log
	bool createIfNotFound; // Create process only if not found
	bool createProcess;    // Create new process

	DWORD processId = 0;
	PROCESS_INFORMATION procInfo;
	STARTUPINFO startupInfo = {0};

	if(!FileExists(".\\loader.ini"))
	{
		MessageBox(NULL, "loader.ini not found.", "Loader", MB_ICONERROR);
		exit(1);	
	}

	createProcess			 = (strcmp(GetSetting("process_create", ""), "1") == 0);
	createIfNotFound		 = (strcmp(GetSetting("process_create_not_found", ""), "1") == 0);
	debug					 = (strcmp(GetSetting("debug", ""), "1") == 0);
	
	process_name			 = GetSetting("process_name", "");
	dll_name				 = GetSetting("dll_name", "");

	if(debug)
	{
		fopen_s(&f, ".\\loader.log", "w");
		if (f==NULL) 
			MsgError("Error opening debugfile loader.log");
	}

	if(strcmp(process_name, "") == 0) MsgError("process_name parameter not specified.");
	if(strcmp(dll_name, "") == 0)	  MsgError("dll_name parameter not specified.");

	if(createProcess)
	{
		bool create = true;

		if(createIfNotFound)
		{
			processId = FindProcessId(process_name);
			if (processId != 0)
			{
				create = false;
				Log("Process found with PID %d, won't create new.\n", processId);
			}
		}
		
		if(create)
		{
			Log("Creating process %s", process_name);
			if (!CreateProcess(process_name, process_name, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &procInfo))
			{
				MsgError("CreateProcess %s failed. \nIs the application in the same folder?\n", process_name);
				exit(1);
			}
			else
			{
				processId = procInfo.dwProcessId;
				Log("CreateProcess: Process created with PID %d\n", processId);
				Sleep(100);
			}
		}
	}
	else
	{
		processId = FindProcessId(process_name);
	}
   
   if(processId == 0)
   {
		MsgError("Error: No process id found.");
		exit(0);
   }

   Log("Opening process %s (%d)\n", process_name, processId);
   HANDLE Proc;
   LPVOID RemoteString, LoadLibraryAddress;
   Proc = OpenProcess(CREATE_THREAD_ACCESS, FALSE, processId);
   if(!Proc)
   {
      MsgError("Error: Failed to OpenProcess handle.");
      exit(0);
   }

   LoadLibraryAddress = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
   Log("LoadLibraryAddress = 0x%x\n", LoadLibraryAddress);
   RemoteString = (LPVOID)VirtualAllocEx(Proc, NULL, strlen(dll_name), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
   Log("Writing DLL name (%s) to memory...\n", dll_name);
   if (!WriteProcessMemory(Proc, (LPVOID)RemoteString, dll_name, strlen(dll_name), NULL))
	   Log("WriteProcessMemory failed!");
   Log("CreateRemoteThread...\n");
   if (!CreateRemoteThread(Proc, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryAddress, (LPVOID)RemoteString, NULL, NULL))
	   Log("CreateRemoteThread failed!");
   else
	   Log("CreateRemoteThread was successful.\n");
   Log("Closing handle..\n");
   CloseHandle(Proc);
   fclose(f);
   return 0;
}

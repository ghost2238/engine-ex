#include "hookmanager.h"

namespace EngineEx
{
	NCodeHookIA32 nch;
	std::vector<Hook*> HookManager::Hooks;

	void HookManager::Log(const char* Text, ...)
	{
		char buffer[4096];
		va_list args;
		va_start(args, Text);
		vsprintf_s(buffer, 4096, Text, args);
		va_end(args);

		printf("[Hooking] %s", buffer);
	}

	void HookManager::Init()
	{
		nch.forceAbsoluteJumps(false);
	}

	// Called by generated code
	void __stdcall HookManager::LogFunc(char* text)
	{
		printf("%s", text);
	}

	void HookManager::MonitorCalls(unsigned long originalFunc, const char* name)
	{
		char* text = new char[255];
		snprintf(text, 255, "%s (0x%X2) was called.\n\0", name, (DWORD)originalFunc);

		DEBUG_HOOK("Adding monitoring call for 0x%x\n", (DWORD)originalFunc);

		DWORD offset = AllocateSpace(18);

		if (offset == 0x0)
		{
			DEBUG_HOOK("Unable to find codespace.\n");
			return;
		}

		DWORD logFunc = (DWORD)&HookManager::LogFunc;

		DWORD trampoline = nch.getNextFreeTrampoline();
		DEBUG_HOOK("Trampoline 0x%x.\n", DWORD(trampoline));

		DEBUG_HOOK("String: 0x%X\n", *(PDWORD)&text);

		byte data[17];
		int i = 0;
		data[i++] = 0x60; // PUSHAD
		data[i++] = 0x68; // PUSH
		*(PDWORD)&data[i] = ((*(PDWORD)&text));
		i += 4;
		data[i++] = 0xE8; // CALL
		*(PDWORD)&data[i] = (DWORD)(logFunc - offset - i - 4);
		i += 4;
		data[i++] = 0x61; // PUSHAD
		data[i++] = 0xE9; // JMP
		*(PDWORD)&data[i] = (DWORD)(trampoline - offset - i - 4);
		i += 4;

		SafeWrite(offset, data, i);

		DWORD result = nch.createHook(originalFunc, (DWORD)offset);
		DEBUG_HOOK("Result 0x%x.\n", DWORD(result));
		if (result == 0x0)
		{
			DEBUG_HOOK("Hooking failed.\n");
			return;
		}
	}

	DWORD HookManager::ReplaceFunction(unsigned long originalFunc, unsigned long handlerFunc)
	{
		DEBUG_HOOK("Handler 0x%x.\n", DWORD(handlerFunc));
		DWORD result = nch.createHook(originalFunc, (DWORD)handlerFunc);
		if (result == 0x0)
			DEBUG_HOOK("Hooking failed.\n");
		DEBUG_HOOK("Code written\n");
		return result;
	}

	DWORD HookManager::GetDLLFunction(const char* dllName, const  char* funcName)
	{
		DWORD funcPtr = NULL;
		HMODULE hDll = LoadLibraryA(dllName);
		if (hDll == NULL)
			return 0;
		funcPtr = (DWORD)GetProcAddress(hDll, funcName);
		FreeLibrary(hDll);
		return funcPtr;
	}

	// Call handler before original func while preserving registers for the original function 
	// (and with right calling convention stack will be fine after).
	DWORD HookManager::CreateBeforeHook(unsigned long originalFunc, unsigned long handlerFunc)
	{
		DEBUG_HOOK("Creating wrapper handler...");
		DWORD offset = AllocateSpace(60);

		DWORD trampoline = nch.getNextFreeTrampoline();

		// All this needs to be seriously refactored, this is just experiment code...

		// 4 * 8 for all registers
		DWORD regspace = AllocateSpace(32);

		byte data[43];
				
		/* Save registers */
		int i = 0;
		data[i++] = 0x89;
		data[i++] = 0x0D;
		*(PDWORD)&data[i] = (DWORD)regspace;
		i += 4;
		data[i++] = 0x89;
		data[i++] = 0x15;
		*(PDWORD)&data[i] = (DWORD)regspace + 4;
		i += 4;
		
		// Assumes callee cleanup and 2 args thus the defined handler needs to be a __fastcall, __stdcall or __thiscall
		// Push our arguments to handler function
		data[i++] = 0x8B; // MOV EAX, DWORD PTR SS : [ESP + 8]
		data[i++] = 0x44;
		data[i++] = 0xE4;
		data[i++] = 0x08;

		data[i++] = 0x8B; // MOV EDX, DWORD PTR SS : [ESP + 4]
		data[i++] = 0x54;
		data[i++] = 0xE4;
		data[i++] = 0x04;

		data[i++] = 0x50; // PUSH EAX
		data[i++] = 0x52; // PUSH EDX
		

		data[i++] = 0xE8; // CALL
		*(PDWORD)&data[i] = (DWORD)(handlerFunc - offset - i - 4);
		i += 4;
		
		/* Restore registers */
		data[i++] = 0x8B;
		data[i++] = 0x0D;
		*(PDWORD)&data[i] = (DWORD)regspace;
		i += 4;
		data[i++] = 0x8B;
		data[i++] = 0x15;
		*(PDWORD)&data[i] = (DWORD)regspace + 4;
		i += 4;

		data[i++] = 0xE9; // JMP
		*(PDWORD)&data[i] = (DWORD)(trampoline - offset - i - 4);
		i += 4;

		SafeWrite(offset, data, i);

		
		DWORD result = nch.createHook(originalFunc, offset);
		if (result == 0x0)
			DEBUG_HOOK("Hooking failed.\n");
		return result;
	}

	void HookManager::RemoveHooks()
	{
		std::map<uintptr_t, NCodeHookItem> funcs = nch.getHookedFunctions();
		// Remove CallHooks
		for (std::map<uintptr_t, NCodeHookItem>::iterator it = funcs.begin(); it != funcs.end(); ++it)
		{
			DEBUG_HOOK("Removing hook 0x%x (Hooked to 0x%x)\n", it->second.OriginalFunc, it->second.HookFunc);
			nch.removeHook(it->second.HookFunc);
		}
		// Remove EndHooks
		for (std::vector<Hook*>::iterator it = HookManager::Hooks.begin(); it != HookManager::Hooks.end(); ++it)
		{
			DEBUG_HOOK("Removing EndHook 0x%x (Hooked to 0x%x)\n", (*it)->getOriginalFunction(), (*it)->getHookHandler());
			HookManager::RemoveEndHook((*it)->getOriginalFunction());
		}
	}

	void HookManager::RemoveCallHook(DWORD func)
	{
		nch.removeHook(func);
	}

	void HookManager::RemoveEndHook(DWORD entryPoint)
	{
		DWORD OldProtect;
		for (std::vector<Hook*>::iterator it = HookManager::Hooks.begin(); it != HookManager::Hooks.end(); ++it)
		{
			if ((*it)->getOriginalFunction() == entryPoint)
			{
				for (std::map<DWORD, CodeBytes*>::iterator it2 = (*it)->overwrittenCode.begin(); it2 != (*it)->overwrittenCode.end(); ++it2)
				{
					DEBUG_HOOK("Restoring code for 0x%x.\n", (*it)->getOriginalFunction());
					DWORD offset = (DWORD)it2->first;

					CodeBytes* cb = it2->second;

					VirtualProtectEx(GetCurrentProcess(), (void*)((DWORD)offset), cb->size + 4, PAGE_EXECUTE_WRITECOPY, &OldProtect);
					for (int i = 0;i < cb->size;i++)
					{
						DEBUG_HOOK("	Restoring 0x%x at 0x%x\n", cb->bytes[i], (DWORD)offset + i);
						(*(byte*)((DWORD)offset + i)) = cb->bytes[i];
					}
					VirtualProtectEx(GetCurrentProcess(), (void*)((DWORD)offset), cb->size + 4, OldProtect, NULL);
				}
			}
		}
	}

	// Needs to be heavily refactored...
	EndHookError HookManager::CreateEndHook(std::string name, DWORD entryPoint, DWORD hookFunction)
	{
		DEBUG_HOOK("Searching func 0x%x\n", (DWORD)entryPoint);

		if (entryPoint == 0x0)
		{
			DEBUG_HOOK("Null entrypoint.\n");
			return EndHookError::NullEntryPoint;
		}
		byte newData[5];
		DWORD OldProtect;

		int maxLength = 512;

		_DecodeResult result;
		_DecodedInst disassembled[256];
		unsigned int instructionCount = 0;
		result = distorm_decode(0, (const unsigned char*)entryPoint, 256, Decode32Bits, disassembled, 256, &instructionCount);
		if (result != DECRES_SUCCESS)
		{
			DEBUG_HOOK("diStorm was unable to disassemble code.\n");
			return EndHookError::DisassembleFailed;
		}

		bool done = false;
		bool firstReturn = true;
		Hook* endhook = new Hook(name, (DWORD)entryPoint, (DWORD)hookFunction);

		for (unsigned int i = 0;i < instructionCount;i++)
		{
			DEBUG_HOOK("%08I64x (%02d) %-24s %s%s%s\n", (DWORD)entryPoint + disassembled[i].offset, disassembled[i].size,
				(char*)disassembled[i].instructionHex.p, (char*)disassembled[i].mnemonic.p, disassembled[i].operands.length != 0 ? " " : "",
				(char*)disassembled[i].operands.p);

			if (strcmp((char*)disassembled[i].mnemonic.p, "RET") == 0)
			{
				DWORD retOffset = ((DWORD)entryPoint + (DWORD)disassembled[i].offset);
				DEBUG_HOOK("Found RET!\n");
				if (firstReturn)
				{
					DEBUG_HOOK("Saving all code from here.\n");
					short saveSize;
					for (unsigned int x = i;x < instructionCount;x++)
					{
						if (strcmp((char*)disassembled[x].mnemonic.p, "INT 3") == 0)
						{
							DEBUG_HOOK("	Found end @ 0x%x.\n", (DWORD)entryPoint + disassembled[x].offset);
							saveSize = (5 + 1 + x) - i; // 5 because of JMP size and potential relocation.
														// 1 because of INT 3.
							break;
						}
					}
					if (saveSize == 0)
					{
						DEBUG_HOOK("ERROR: Unable to find where function ends.\n");
						return EndHookError::NoFunctionEnd;
					}
					byte* savedCode = new byte[saveSize];
					for (int x = 0;x < saveSize; x++)
					{
						savedCode[x] = *(byte*)(retOffset + x);
						DEBUG_HOOK("Saving byte %0x\n", savedCode[x]);
					}
					CodeBytes* cb = new CodeBytes;
					cb->bytes = savedCode;
					cb->size = saveSize;
					endhook->addOverwrittenCode(retOffset, cb);
					firstReturn = false;
				}

				DEBUG_HOOK("Hook Func adr = 0x%x\n", (DWORD)hookFunction);
				DEBUG_HOOK("Calculated jmp: 0x%x\n", (((DWORD)(hookFunction - 5)) - ((DWORD)retOffset)));

				*(PDWORD)&newData[1] = (((DWORD)(hookFunction - 5)) - ((DWORD)retOffset));
				newData[0] = 0xE9; //JMP (near)

				int sizeRemain = 5 - disassembled[i].size;
				int relocSize = 5 - disassembled[i].size;
				DWORD relocOffset = ((DWORD)retOffset + (5 - disassembled[i].size));
				bool needReloc = false;
				if (sizeRemain > 0)
				{
					DEBUG_HOOK("Reloc instructions: \n");
					int x = 1;
					while (sizeRemain > 0)
					{
						DEBUG_HOOK("%08I64x (%02d) %-24s %s%s%s\r\n", (DWORD)entryPoint + disassembled[i + x].offset, disassembled[i + x].size,
							(char*)disassembled[i + x].instructionHex.p, (char*)disassembled[i + x].mnemonic.p, disassembled[i + x].operands.length != 0 ? " " : "",
							(char*)disassembled[i + x].operands.p);
						sizeRemain -= disassembled[i + x].size;
						x++;
						DEBUG_HOOK("sizeRemain: %d\n", sizeRemain);
						if ((strcmp("INT3", (char*)disassembled[i + x].mnemonic.p) != 0) &&
							(strcmp("NOP", (char*)disassembled[i + x].mnemonic.p) != 0))
						{
							DEBUG_HOOK("Need to reloc!!\n");
							needReloc = true;
						}
					}
				}

				if (needReloc)
				{
					VirtualProtectEx(GetCurrentProcess(), (void*)(DWORD)relocOffset, 128, PAGE_EXECUTE_WRITECOPY, &OldProtect);
					byte lastByte;
					byte writeByte;
					// Relocating code.
					for (int x = 0; x < relocSize; x++)
					{
						lastByte = 0x90; // nop
						for (int y = 0;y < 128;y++)
						{
							DEBUG_HOOK("Reloc: Write %0x to %0x\n", lastByte, ((DWORD)relocOffset + y));
							memcpy(&writeByte, &lastByte, 1);
							lastByte = *(byte*)((DWORD)relocOffset + y);
							(*(byte*)((DWORD)relocOffset + y)) = writeByte;
							if (lastByte == 0xCC)
							{
								DEBUG_HOOK("Reloc: Done\n");
								break;
							}
						}
					}
					// Relocating jumps to code.
					DEBUG_HOOK("Relocating jumps.\n");
					for (unsigned int x = 0;x < instructionCount;x++)
					{
						if (disassembled[x].mnemonic.p[0] == 'J')
						{
							DWORD jumpOffset = (DWORD)entryPoint + (DWORD)disassembled[x].offset;
							byte jmpCode = *(byte*)((DWORD)jumpOffset + 1);

							if (disassembled[x].size == 5)
								DEBUG_HOOK("0x%x == Long jump.\n", (DWORD)disassembled[x].offset);

							DWORD jumpTo = ((DWORD)jumpOffset + 2 + (int)jmpCode);
							// The offset we disassembled.
							if (jumpTo >= retOffset && jumpTo <= ((DWORD)retOffset + disassembled[i].size + relocSize))
							{
								VirtualProtectEx(GetCurrentProcess(), (void*)((DWORD)jumpOffset), 4, PAGE_EXECUTE_WRITECOPY, &OldProtect);
								DEBUG_HOOK("Found jump that needs to be relocated by %d.\n", relocSize);
								DEBUG_HOOK("	Jump @ 0x%x\n", (DWORD)jumpOffset);
								DEBUG_HOOK("	JumpTo: 0x%x\n", (DWORD)jumpTo);
								byte* savedCode = new byte[1];
								savedCode[0] = *(byte*)(jumpOffset + 1);
								DEBUG_HOOK("	Saving byte 0x%x\n", savedCode[0]);
								CodeBytes* cb = new CodeBytes;
								cb->bytes = savedCode;
								cb->size = 1;
								endhook->addOverwrittenCode((DWORD)jumpOffset + 1, cb);
								(*(byte*)((DWORD)jumpOffset + 1)) = jmpCode + ((byte)relocSize);
								DEBUG_HOOK("	New value = 0x%x\n", *(byte*)(DWORD(jumpOffset + 1)));
								VirtualProtectEx(GetCurrentProcess(), (void*)((DWORD)jumpOffset), 4, OldProtect, NULL);
							}
						}
					}
				}


				VirtualProtectEx(GetCurrentProcess(), (void*)((DWORD)entryPoint + disassembled[i].offset), 10, PAGE_EXECUTE_WRITECOPY, &OldProtect);
				(*(byte*)((DWORD)entryPoint + disassembled[i].offset)) = newData[0];
				(*(byte*)((DWORD)entryPoint + disassembled[i].offset + 1)) = newData[1];
				(*(byte*)((DWORD)entryPoint + disassembled[i].offset + 2)) = newData[2];
				(*(byte*)((DWORD)entryPoint + disassembled[i].offset + 3)) = newData[3];
				(*(byte*)((DWORD)entryPoint + disassembled[i].offset + 4)) = newData[4];
				VirtualProtectEx(GetCurrentProcess(), (void*)((DWORD)entryPoint + disassembled[i].offset), 10, OldProtect, NULL);
				DEBUG_HOOK("Hooking done.\n");
			}
			else if (strcmp((char*)disassembled[i].mnemonic.p, "INT 3") == 0)
			{
				DEBUG_HOOK("0x%x: End of func!\n", (DWORD)entryPoint + disassembled[i].offset);
				HookManager::Hooks.push_back(endhook);
				done = true;
			}

			if (done) return EndHookError::Success;
		}

		return EndHookError::Success;
	};
}
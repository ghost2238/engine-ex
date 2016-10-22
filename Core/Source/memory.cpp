#include "memory.h"

namespace EngineEx
{
	void SafeWrite(DWORD address, byte* bytes, int size)
	{
		MemLog("Starting to write %d bytes at 0x%x.\n", size, (DWORD)address);
		DWORD OldProtect;
		VirtualProtectEx(GetCurrentProcess(), (void*)((DWORD)address), size, PAGE_EXECUTE_WRITECOPY, &OldProtect);
		for (int i = 0;i < size;i++)
		{
			(*(byte*)((DWORD)address + i)) = bytes[i];
			MemLog("0x%x: Writing '0x%x'\n", (DWORD)(address + i), bytes[i]);
		}
		VirtualProtectEx(GetCurrentProcess(), (void*)((DWORD)address), size, OldProtect, NULL);
	}

	void SafeWrite4(DWORD address, byte* bytes) { SafeWrite(address, bytes, 4); }
	void SafeWrite8(DWORD address, byte* bytes) { SafeWrite(address, bytes, 8); }
	void SafeWrite16(DWORD address, byte* bytes) { SafeWrite(address, bytes, 16); }
	void SafeWrite32(DWORD address, byte* bytes) { SafeWrite(address, bytes, 32); }

	void MemLog(const char* Text, ...)
	{
		char buffer[256];
		va_list args;
		va_start(args, Text);
		vsprintf_s(buffer, 4096, Text, args);
		va_end(args);

		printf("[Memory] %s", buffer);
	}

	DWORD AllocateSpace(int neededBytes)
	{
		void* mem = VirtualAllocEx(GetCurrentProcess(), NULL, neededBytes, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

		MemLog("Allocated %d bytes @ 0x%x\n", neededBytes, (DWORD)mem);
		return (DWORD)mem;
	}


	// Use AllocateSpace instead, this is if we want to inject
	DWORD FindEmptySpace(int neededBytes)
	{
		return FindEmptySpace(neededBytes, 0x400000);
	}

	DWORD FindEmptySpace(int neededBytes, DWORD beginOffset)
	{
		DWORD startOffset = NULL;
		if (beginOffset != NULL)
			startOffset = beginOffset;

		if (startOffset == NULL)
		{
			MemLog("Invalid starting offset\n");
			return 0;
		}

		MemLog("Trying to find space for code, %d bytes starting at 0x%x.\n", neededBytes, startOffset);

		int foundBytes;

		int fetches = 2000;
		DWORD foundOffset = 0;
		DWORD currentOffset = 0;

#define bufferSize 256

		for (int x = 0;x < fetches; x++)
		{
			// Continue where we are
			if (currentOffset != 0) startOffset = currentOffset;

			_DecodeResult result;
			_DecodedInst disassembled[bufferSize];
			unsigned int instructionCount = 0;
			result = distorm_decode(0, (const unsigned char*)startOffset, bufferSize, Decode32Bits, disassembled, bufferSize, &instructionCount);
			if (result != DECRES_SUCCESS)
			{
				MemLog("diStorm was unable to disassemble code.\n");
				return 0;
			}

			for (unsigned int i = 0;i < instructionCount;i++)
			{
				currentOffset = (DWORD)(startOffset + disassembled[i].offset);
				//MemLog("current is 0x%x\n", currentOffset);
				bool canUse = (strcmp((char*)disassembled[i].mnemonic.p, "INT 3") == 0 ||
					strcmp((char*)disassembled[i].mnemonic.p, "NOP") == 0);

				if (canUse)
				{
					if (foundBytes == 0) foundOffset = (DWORD)currentOffset;
					foundBytes++;
					//DEBUG_HOOK("Found INT 3 @ 0x%x\n", (DWORD)currentOffset);
				}
				else
				{
					foundBytes = 0;
				}

				if (foundBytes == neededBytes)
				{
					MemLog("Suitable offset found @ 0x%x\n", (DWORD)foundOffset);
					return foundOffset;
				}
			}

		}
		return NULL;
	}
}
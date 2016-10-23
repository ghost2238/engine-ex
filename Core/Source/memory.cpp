#include "memory.h"

namespace EngineEx
{
	#define LOG_D(...) Log::Debug(LogModule::Memory, __VA_ARGS__);
	#define LOG_E(...) Log::Error(LogModule::Memory, __VA_ARGS__);
	#define LOG_T(...) Log::Trace(LogModule::Memory, __VA_ARGS__);
	#define LOG_I(...) Log::Info(LogModule::Memory, __VA_ARGS__);

	void SafeWrite(DWORD address, byte* bytes, int size)
	{
		LOG_D("Starting to write %d bytes at 0x%x.", size, (DWORD)address);
		DWORD OldProtect;
		VirtualProtectEx(GetCurrentProcess(), (void*)((DWORD)address), size, PAGE_EXECUTE_WRITECOPY, &OldProtect);
		for (int i = 0;i < size;i++)
		{
			(*(byte*)((DWORD)address + i)) = bytes[i];
			LOG_T("0x%x: Writing '0x%x'", (DWORD)(address + i), bytes[i]);
		}
		VirtualProtectEx(GetCurrentProcess(), (void*)((DWORD)address), size, OldProtect, NULL);
	}

	byte* SafeRead(DWORD address, int bytes)
	{
		byte* data = new byte[bytes];
		for (int i = 0;i < bytes;i++)
		{
			*(byte*)&data[i] = *(byte*)((DWORD)(address + i));
			LOG_T("0x%x: Reading '0x%x'", ((DWORD)(address + i)), (*(byte*)((DWORD)(address + i))));
		}
		return data;
	}

	void SafeWrite4(DWORD address, byte* bytes) { SafeWrite(address, bytes, 4); }
	void SafeWrite8(DWORD address, byte* bytes) { SafeWrite(address, bytes, 8); }
	void SafeWrite16(DWORD address, byte* bytes) { SafeWrite(address, bytes, 16); }
	void SafeWrite32(DWORD address, byte* bytes) { SafeWrite(address, bytes, 32); }

	void SafeMemCpy(void* dst, void* src, int size)
	{
		LOG_T("Copying src 0x%x to dst 0x%x, size %d", (DWORD)dst, (DWORD)src, size);
		memcpy_s(dst, size, src, size);
	}

	DWORD Allocate(int neededBytes)
	{
		void* mem = VirtualAllocEx(GetCurrentProcess(), NULL, neededBytes, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

		LOG_D("Allocated %d bytes @ 0x%x", neededBytes, (DWORD)mem);
		return (DWORD)mem;
	}

	// Use AllocateSpace generally, this is if we want to inject near a function.
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
			LOG_D("Invalid starting offset");
			return 0;
		}

		LOG_D("Trying to find space for code, %d bytes starting at 0x%x.", neededBytes, startOffset);

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
				LOG_E("diStorm was unable to disassemble code.");
				return 0;
			}

			for (unsigned int i = 0;i < instructionCount;i++)
			{
				currentOffset = (DWORD)(startOffset + disassembled[i].offset);
				LOG_T("Current offset is 0x%x", currentOffset);
				bool canUse = (strcmp((char*)disassembled[i].mnemonic.p, "INT 3") == 0 ||
					strcmp((char*)disassembled[i].mnemonic.p, "NOP") == 0);

				if (canUse)
				{
					if (foundBytes == 0) foundOffset = (DWORD)currentOffset;
					foundBytes++;
					LOG_T("Found INT 3 @ 0x%x", (DWORD)currentOffset);
				}
				else
				{
					foundBytes = 0;
				}

				if (foundBytes == neededBytes)
				{
					LOG_I("Suitable offset found @ 0x%x", (DWORD)foundOffset);
					return foundOffset;
				}
			}

		}
		return NULL;
	}
}
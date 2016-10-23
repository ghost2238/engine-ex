#pragma once

#include <string>
#include <cstdarg>
#include <sstream>
#include <vector>
#include "windows.h"
#include "memory.h"
#include "debug.h"

namespace EngineEx
{
	std::string string_format(const char* fmt, ...);

	struct CodeBytes
	{
		byte* bytes;
		short size;
	};

	class Asm
	{
		public:
			//char* FromString(char* string);

	};

	class FunctionAnalyzer
	{
		public:
			FunctionAnalyzer(DWORD entryPoint, std::vector<_DecodedInst> disassembled, unsigned int instructionCount);
			std::vector<_DecodedInst> FindByMnemonic(char* mnemonic);
			_DecodedInst FindFirstMnemonic(char* mnemonic);

			DWORD entryPoint;
			DWORD endOfFunction;
			std::vector<_DecodedInst> disassembled;
			unsigned int instructionCount;
	};

	class DisAssembler
	{
		public:
			DisAssembler();
			_DecodeResult DisAssemble(byte* bytes, int length, unsigned int& instructions, std::vector<_DecodedInst>& disassembledCode);
			std::vector<std::string>* GetAsm(std::vector<_DecodedInst> instructions, int start, int count);
			std::vector<std::string>* GetAsm(byte* bytes, int size);
			std::string error;
	};

	class MemoryPatch
	{
		public:
			MemoryPatch(int size);
			MemoryPatch(int size, DWORD address);
			void Add(byte b);
			void AddAdress(DWORD adress);
			std::vector<std::string>* MemoryPatch::GetAsm();

			void Call(DWORD to);
			void Jmp(DWORD to);
			void Push(DWORD adress);

			bool Write();

			DWORD address;
			byte* data;
			std::string error;
			std::string warn;
			int size;
			int bytesUsed;
			int currentPos;
	};
}
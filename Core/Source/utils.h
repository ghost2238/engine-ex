#pragma once

#include "memory.h"
#include "debug.h"
#include "../Lib/JsonCPP/json.h"
#include "../Lib/diStorm/distorm.h"

#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <cstdarg>
#include <sstream>
#include <vector>


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

	enum CallingConvention
	{
		c_decl, // cdecl
		stdcall,
		fastcall,
		thiscall
	};

	struct CodeBytes
	{
		byte* bytes;
		short size;
	};

	struct VariableSymbol
	{
		std::string name;
		DWORD offset;
		std::string type;
	};

	struct FunctionSymbol
	{
		std::string name;
		DWORD offset;
		std::string returnValue;
		std::vector<std::string> arguments;
		CallingConvention callingConvention;
	};

	enum AsmX86
	{
		call    = 0xE8,
		jmp     = 0xE9,
		jmp_abs = 0xFF
	};

	_DecodeResult DisAssemble(byte* bytes, int length, std::vector<_DecodedInst>& disassembledCode);
	std::vector<std::string>* GetAsm(std::vector<_DecodedInst> instructions, int start, int count);
	std::vector<std::string>* GetAsm(byte* bytes, int size);
	bool IsBranch(_DecodedInst instruction);


	// String functions
	void split(const std::string &s, char delim, std::vector<std::string> &elems);
	std::vector<std::string> split(const std::string &s, char delim);
	bool equals(const std::string &a, const std::string &b);
	//bool equals(const std::string &a, char* b);
	inline std::string format(const char* fmt, ...);

	// Misc, maybe move these somewhere
	bool requiresAbsJump(uintptr_t from, uintptr_t to);
	int getMinOffset(const unsigned char* codePtr, unsigned int jumpPatchSize);
	
	// Threading
	void ResumeMainThread();
	void SuspendMainThread();
	HANDLE GetMainThread();

	class FunctionAnalyzer
	{
		public:
			FunctionAnalyzer(DWORD entryPoint, std::vector<_DecodedInst> disassembled);
			std::vector<_DecodedInst> FindByMnemonic(char* mnemonic);

			DWORD entryPoint;
			DWORD endOfFunction;
			std::vector<_DecodedInst> disassembled;
			unsigned int instructionCount;
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
			void JmpAbs(DWORD adress);
			void Push(DWORD adress);

			void PushStackArgs(CallingConvention call, int count);

			void SaveRegisters(DWORD to);
			void RestoreRegisters(DWORD from);

			bool Write();

			DWORD address;
			byte* data;
			byte* overWritten;
			std::string error;
			std::string warn;
			int size;
			int bytesUsed;
			int currentPos;
	};
}
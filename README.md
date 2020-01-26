# EngineEx

## What is EngineEx?
EngineEx is basically a framework for modifying (generally called binary instrumentation) and inspecting the internals of game engines and other applications that don't have any modding API.
This is facilitated by function hooking, dynamic code generation and reading/writing the target process memory.

The end result after compilation is a single DLL and a loader binary which injects the DLL into the game or application.

You will still need to do the hard work of reverse engineering the data structures and functions, but hopefully EngineEx can help you even here with some code and call analysis.

## Why?
Mostly for learning, but the if the project comes long enough, it may be actually useful for something.

Yes, I know about [other hooking libraries](https://github.com/frida/frida/wiki/Comparison-of-function-hooking-libraries) which can do a lot more and work on many platforms.

## History
It started as some experiments with modifying/scripting the [FOnline](http://www.moddb.com/games/fonline-2238) (a game I was involved heavily in for a few years) client with only the binary available and was called FOnlineEx.

## Libraries
EngineEx currently uses the following excellent libraries:

- [JsonCPP](https://github.com/open-source-parsers/jsoncpp)
- [diStorm](https://github.com/gdabah/distorm)
- [fmt](https://github.com/fmtlib/fmt)

## Platform Support
Only Windows and x86 binaries are really supported so far. I'm still thinking of the best approach to multi-arch.

| OS  | Status |
| ------------- | ------------- |
| Windows 10 64-bit | Works
| Windows 7 64-bit  | Works

## How to build? ##
I've only tested with Visual C++ 2015 so far. It uses some VC++ specific directives so other compilers probably won't work. It's on the TODO to make it compile on GCC/MinGW.

 1. Download and install [Visual Studio Community](https://www.visualstudio.com/downloads/)
 2. Open the .sln file.
 3. "Build" -> "Build Solution"

Please report any issues with building.

## Current status
Very alpha, the codebase still contains testcode for a specific game (FOnline) that I'm testing the framework with.
Most likely it will crash if you try to use it without knowing exactly what you are doing since there's a lot of assumptions in the hooking code right now.

Compared to other hooking libraries, it's still very limited.

### Current hooking functions
| Function | Purpose |
| ------------- | ------------- |
| Replace function | Target function is replaced by a handler function with the same signature and calling convention. Or you can use __declspec(naked) to handle prolog/epilog.|
| Before hook | Handler is called before the original function, then the original function is called as normal. Calling the original function is handled by EngineEx.
| End hook | Handler is called when the original function returns (RET instructions).
| Monitor calls | Prints to stdout when target function gets called.

All of these are currently done using detour-style hooks where a JMP is written in the original function and the overwritten bytes are relocated.

## Goals
- Try to use libraries in source form, it should be easy to build the project.
- The end result should be easy to redistribute (Just a DLL + loader/patcher), so that it can be used as a modding base even by non-technical people.
- This framework is not made to facilitate cheating in multiplayer games, so there will not be any anti-cheat engine evasion code or similiar. The techniques used are very obvious for standard anti-cheating engines.

## TODO
- [X] Better interface with core library, remove macros and other ugly stuff.
- [ ] Guess calling convention and number of arguments of a target function. Static/dynamic analysis.
- [ ] Embedded HTTP server to facilitate UI. [Kore](https://github.com/jorisvink/kore) looks good.
- [ ] Scripting language, both for exposing your API and EngineEx internals. ChaiScript? https://github.com/ChaiScript/ChaiScript/
- [ ] Patcher to patch exe with LoadLibrary on disk so that DLL is autoloaded by the target application.
- [ ] Dump registers/stack at target instruction offset.
- [ ] Read symbols from JSON file.
  - [X] Functions
  - [ ] Classes, ability to call hook functions with ClassName::FunctionName
  - [X] Variables
  - [ ] Tool for generating header file from symbols
- [ ] Read symbols from PDB.
- [ ] Relocating functions to another place in memory.
- [ ] Better thread safety.
- [ ] SEH hooking
- [ ] IAT hooking
- [ ] Proper jump relocation
- [ ] Helpers for calling functions easily
  - [ ] Ability to call functions with non-standard calling conventions.
- [ ] Example project should be with some binary that's included in the repo.
- [ ] Small JSON editor to add/remove data.
- [ ] x64 support
- [ ] Linux support
  - [ ] GCC Compilation
  - [ ] Makefile
  - [ ] .so injection

## Considering
- Use of Frida core as it has a lot of really advanced hooking functionality already.
- Keystone framework for machine code generation.
- Capstone for the disassembly parts.

## Directories
| Directory  | Purpose |
| ------------- | ------------- |
| __build  | Common binary output.  |
| App      | Example project which configure the hooks and has some hook handlers.  |
| Core      | This is the main code, contains the hooking framework and the low level code for doing various things. Compiles to static library. |
| Loader      | Injects the compiled Core DLL.  |
| Utils     | Utilities, e.g. for generating the function/variable defines. |

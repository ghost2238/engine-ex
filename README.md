# EngineEx

## What is EngineEx?
EngineEx is basically a framework for modifying (generally called binary instrumentation) and inspecting the internals of game engines and other applications that don't have any modding API.
This is facilitated by function hooking, dynamic code generation and reading/writing the target process memory.

The end result after compilation is a single DLL and a loader binary which injects the DLL into the game or application.

You will still need to do the hard work of reverse engineering the data structures and functions, but hopefully EngineEx can help you even here with some code and call analysis.

## Why?
Mostly for learning, but the if the project comes long enough, it may be actually useful for something.

Yes, I know about [other hooking frameworks](https://github.com/frida/frida/wiki/Comparison-of-function-hooking-libraries) which can do a lot more and works on many platforms.

## History
It started as some experiments on modifying/scripting the [FOnline](http://www.moddb.com/games/fonline-2238) (a game I was involved heavily in for a few years) client with only the binary available and was called FOnlineEx.

## Libraries
EngineEx currently uses the following excellent libraries:

- [JsonCPP](https://github.com/open-source-parsers/jsoncpp)
- [diStorm](https://github.com/gdabah/distorm)
- NCodeHook

## Platform Support
Only Windows and x86 binaries are really supported so far.

## How to build? ##
I've only tested with Visual C++ 2015 so far. It uses some VC++ specific directives so other probably compilers won't work.

 1. Download and install [Visual Studio Community](https://www.visualstudio.com/downloads/)
 2. Open the .sln file.
 3. "Build" -> "Build Solution"

Please report any issues with building.

## Current status
Very alpha, the codebase still contains testcode for a specific game (FOnline) that I'm testing the framework with.
Most likely it will crash if you try to use it without knowing exactly what you are doing since there's a lot of assumptions in the hooking code now, leading to different calling conventions not being handled properly.

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
- This framework is not made to facilitate cheating in multiplayer games, so there will not be any anti-cheat engine evasion code or similiar. The techniques used are very obvious for standard cheating engines.

## TODO
- [ ] Better interface with core library, remove macros and other ugly stuff.
- [ ] Better separation between game/app part, too tangled right now due to testing.
- [ ] Switch to the Keystone framework for machine code generation.
- [ ] Switch to Capstone for the disassembly parts.
- [ ] Guess calling convention and number of arguments of a target function. Static/dynamic analysis.
- [ ] Embedded HTTP server to facilitate UI. [Kore](https://github.com/jorisvink/kore) looks good.
- [ ] Scripting language, both for exposing your API and EngineEx internals.
- [ ] Patcher to patch exe with LoadLibrary on disk so that DLL is autoloaded by the target application.
- [ ] Dump specific registers at target instruction offset.
- [ ] Read symbols from JSON file.
- [ ] Read symbols from PDB.

## Considering
- Use of Frida core as it has a lot of really advanced hooking functionality already.
- Probably remove NCodeHook.

## Directories
| Directory  | Purpose |
| ------------- | ------------- |
| __build  | Common binary output.  |
| App      | Example project which configure the hooks and has some hook handlers.  |
| Core      | This is the main code, contains the hooking framework and the low level code for doing various things. Compiles to static library. |
| Loader      | Injects the compiled Core DLL.  |
| Utils     | Utilities, e.g. for generating the function/variable defines. |

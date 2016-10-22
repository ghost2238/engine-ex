#include <windows.h>
#include <stdio.h>
#include <string>

// Include core
#pragma comment(lib, "../__build/Core.lib")
#include "../Core/Source/core.h"
using namespace EngineEx;

// App specific stuff here
#include "offsets.h"
#include "hooks.h"
#include "Game/foclient.h"

bool AppMain();
bool AppFinish();
bool IsFinished();
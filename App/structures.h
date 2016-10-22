#ifndef __STRUCTURES__
#define __STRUCTURES__

#include <Windows.h>

// CritterCl
//0x0 = id
//0x06 = HexX
//0x08 = HexY
//0x0A = Dir
//0x28 = PID?
//0x120 = IsPlayer?
//0x12C = HP
//0x13C = EXP
//0x140 = Level?
//0x1AC = DialogId
//0xFEC = Flags
//0x32C = Small Guns
//0x330 = Big Guns
//0x334 = Energy Weapons
//0x338 = Close Combat
//0x33C = Reserved
//0x340 = Throwing
//0x344 = First Aid
//0x348 = Doctor
//0x34C = Sneak
//0x350 = Lockpick
//0x354 = Steal
//0x358 = Traps
//0x1000 = Name
struct CritterCl {
	int id; // 0x0 
	short pid; // 0x4 
	short hexX; // 0x6 
	short hexY; // 0x8 
	byte dir; // 0xa 
	char pad_5 [0x115];
	byte isPlayer; // 0x120 
	char pad_6 [0xb];
	int hp; // 0x12c 
	char pad_7 [0xc];
	int xp; // 0x13c 
	int level; // 0x140 
	char pad_9 [0x68];
	int dialogID; // 0x1ac 
	char pad_10 [0xe38];
	DWORD flags; // 0xfe8 
	char pad_11 [0x14];
	char** name; // 0x1000 
};


// 0x21C = GameState
// 0x1CE4 = wmX
// 0x1CE8 = wmY
// 0x6BA0 = CritterCl Self / Chosen
struct FOClient {
	char pad_0 [0x21c];
	byte GameState; // 0x21c 
	char pad_1 [0x1ac7];
	int wmX; // 0x1ce4 
	int wmY; // 0x1ce8 
	char pad_3 [0x4eb4];
	CritterCl* self; // 0x6ba0 
};

// Custom
struct GameState {
	
};

#endif
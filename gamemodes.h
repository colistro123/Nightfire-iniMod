#ifndef GAMEMODES_H
#define GAMEMODES_H
//This is where we will write all of our "hooks" to push to AMX later *somehow*
//#include "stdafx.h"
#include <stdio.h>
#include <cstdio>
#include <cmath>
#include <math.h>
#include <conio.h>
#include <iostream>
#include <winsock2.h>
#include <time.h>
#include <fstream>
#include <vector>
#include < extdll.h >
#include <string>
#include "string.h"
#include "amx/amx.h"

using namespace std;

//Disconnect Reasons (Moved to player.h)
/*
#define REASON_DISCONNECT		1
#define REASON_TIMEOUT			2
#define REASON_MAPCHANGE		3
*/

class CNetwork {
public:
	CNetwork() { };
    //Destructor
    virtual ~CNetwork() { };
	//Player and server related - Scripting
	void OnServerChangeMap();
	void OnMapLoaded();
	void OnClientConnect(cell playerid);
	void OnClientReconnect(cell playerid);
	void OnClientDisconnect(int playerid, int reason); //Reason means: Timeout or normal disconnect
	int OnClientUpdate(cell playerid); //This function gets called continuously..
	void OnGameModeInit();
	int OnServerThink( void );
	void UpdatePlayers( void );
	void OnClientCommandText(int playerid, const char* recvcmd);
	void OnClientDeath(cell attacker, cell receiver);
	void OnClientSpawn(cell playerid);
	void OnClientEquip(cell playerid);
	//Script / Loading related stuff
	void parseConfigFile( void );
	void decideParse(char* param1, char* param2);
	void initAMXGM( void );
	bool LoadScript(char * filename);
	void UnloadScript();
	void Error(int error);
	bool LoadProgram(char* file,void * _program);
	void RestartMap();
	int ExecMain();
	bool registerNatives(const AMX_NATIVE_INFO *list);
	AMX* GetGMAMXPointer() { return &inimod_amx; };
private:
	static bool GMLoaded;
	int loadTime;
	//void *program;
    static AMX inimod_amx;
    int err;
	//End of script related stuff
};

int amx_initNatives(AMX *amx);
#endif

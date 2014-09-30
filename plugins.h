#ifndef PLUGINS_H
#define PLUGINS_H
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

#define PLUGINS_DIR			"plugins"
#define MAX_PLUGINS 20

class CPlugins {
public:
	CPlugins() { };
    //Destructor
    virtual ~CPlugins() { };
	//Player and server related - Scripting
	void OnServerChangeMap();
	void OnMapLoaded();
	void OnClientConnect(cell playerid);
	void OnClientDisconnect(int playerid, int reason); //Reason means: Timeout or normal disconnect
	int OnClientUpdate(cell playerid); //This function gets called continuously..
	int OnServerThink( void );
	void OnClientCommandText(int playerid, const char* recvcmd);
	void OnClientDeath(cell attacker, cell receiver);
	void OnClientSpawn(cell playerid);
	void OnClientEquip(cell playerid);
	//Script / Loading related stuff
	void parseConfigFile( void );
	void decideParse(char* param1, char* param2);
	bool LoadScript(char * filename);
	void RemovePlugin(int index);
	bool RemovePluginByName(char* name);
	void UnloadAllPlugins( void );
	void Error(int error);
	void UpdatePlayers( void );
	void ParsePlugins(char* plugins);
private:
	static bool PluginLoaded[MAX_PLUGINS];
	static AMX* inimod_plugins[MAX_PLUGINS];
	static char inimod_plugins_name[MAX_PLUGINS][256];
	static int inimod_plugincount;
	int loadTime;
    int err;
	//End of script related stuff
};
#endif
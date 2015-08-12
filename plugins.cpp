#include "plugins.h"
#include "addresses.h"
#include "player.h"
#include "gameinterface.h"
#include "utils.h"
#include "engine.h"
#include "amx/amxaux.h"
#include "amx/amxcons.h"
#include "format.h"
#include "hooks.h"
#include "con_commands.h"
#include "precache.h"
#include "baseentity.h"
#include "gettime.h"
#include "gamemodes.h"

//Set default values to the static values we initialized in gamemodes.h
bool CPlugins::PluginLoaded[MAX_PLUGINS] = {0};
AMX* CPlugins:: inimod_plugins[MAX_PLUGINS] = {0};
char CPlugins::inimod_plugins_name[MAX_PLUGINS][256] = {0};
int CPlugins::inimod_plugincount = 0;

#pragma comment(lib, "winmm.lib")

extern "C" int amx_CoreInit(AMX* amx);
extern "C" int amx_CoreCleanup(AMX* amx);
extern "C" int amx_FloatInit(AMX* amx);
extern "C" int amx_FloatCleanup(AMX* amx);
extern "C" int amx_StringInit(AMX* amx);
extern "C" int amx_StringCleanup(AMX* amx);
extern "C" int amx_FileInit(AMX* amx);
extern "C" int amx_FileCleanup(AMX* amx);
extern "C" int amx_TimeInit(AMX* amx);
extern "C" int amx_TimeCleanup(AMX* amx);
extern "C" int amx_DGramInit(AMX* amx);
extern "C" int amx_DGramCleanup(AMX* amx);

void CPlugins::ParsePlugins(char *plugins) {
	CCommands *cmdPtr = new CCommands;
	Utils *utils = new Utils;
	int entityptr = 0;
	char output[128], scriptpath[128];
	int offset = cmdPtr->returnCommandParamCount(plugins);
	if(offset != 0) {
		for(int i=0; i < offset; i++) {
			strcpy(output, (char *)utils->ExplodeAndReturn(plugins, i, " ").c_str());
			if(strlen(output) > 0) {
				sprintf(scriptpath, "%s/%s", PLUGINS_DIR, output);
				LoadScript(scriptpath);
			}
		}
	}
	delete cmdPtr;
	delete utils;
}
bool CPlugins::LoadScript(char *filename) {
    size_t memsize = aux_ProgramSize(filename);
	Utils *utils = new Utils();
    if(memsize == 0) {
        printf("Script file not found or corrupted\n");
        return false;
    }
	if (inimod_plugincount >= MAX_PLUGINS)
		return false;

	FILE* f = fopen(filename, "rb");
	if (!f) return false;
	fclose(f);
	
	int i;
	for (i = 0; i < MAX_PLUGINS; i++) {
		if (inimod_plugins[i] == NULL) break; //We found a spot
		if (strcmp(filename, inimod_plugins_name[i]) == 0) return false;
	}
	if (i == MAX_PLUGINS) return false; //All the spots were taken
	//loadplugin plugin1.amx
	inimod_plugins[i] = new AMX;
	AMX* amx_plugin = inimod_plugins[i];

	memset((void*)amx_plugin, 0, sizeof(AMX));
    void * program = malloc(memsize);

    int err = aux_LoadProgram(amx_plugin, filename, program);
	if (err != AMX_ERR_NONE) {
		printf("Couldn't load plugin %s.", filename);
		Error(this->err);
		return false;
	}
	amx_CoreInit(amx_plugin);
	amx_FloatInit(amx_plugin);
	amx_StringInit(amx_plugin);
	amx_FileInit(amx_plugin);
	amx_TimeInit(amx_plugin);
	amx_initNatives(amx_plugin);

	int tmpvar;
	if(!amx_FindPublic(amx_plugin, "OnPluginInit", &tmpvar)) {
		amx_Exec(amx_plugin, (cell*)&tmpvar, tmpvar);
	}
	inimod_plugincount = i;
	strcpy(inimod_plugins_name[i], (char *)utils->ExplodeAndReturn(filename, 1, "/").c_str());
	delete utils;
    return true;
}
bool CPlugins::RemovePluginByName(char* name) {
	int i;
	for (i = 0; i < MAX_PLUGINS; i++) {
		if (strcmp(name, inimod_plugins_name[i]) == 0) break; //If we could find it, break out of the loop
	}
	if (i == MAX_PLUGINS) return false; //Couldn't find it
	if (inimod_plugins[i]) {
		RemovePlugin(i);
		return true;
	}
	return false;
}
void CPlugins::RemovePlugin(int index) {
	int val;
	if (!amx_FindPublic(inimod_plugins[index], "OnPluginExit", &val))
		amx_Exec(inimod_plugins[index], (cell*)&val, val);

	aux_FreeProgram(inimod_plugins[index]);
	amx_TimeCleanup(inimod_plugins[index]);
	amx_FileCleanup(inimod_plugins[index]);
	amx_StringCleanup(inimod_plugins[index]);
	amx_FloatCleanup(inimod_plugins[index]);
	amx_CoreCleanup(inimod_plugins[index]);

	inimod_plugins[index] = 0;
}
void CPlugins::UnloadAllPlugins( void ) {
	for (int i=0; i<MAX_PLUGINS; i++) {
		if (inimod_plugins[i]) {
			RemovePlugin(i);
		}
	}
	inimod_plugincount = 0;
}
void CPlugins::Error(int error) {
    printf("Run time error %d: \"%s\"\n",
           error, aux_StrError(error));
}
//General "Network" / Gamemode AMX Code
void CPlugins::OnServerChangeMap() {
	int idx;
	cell ret = 0;
	//amx push would go below here.. (So we can push the OnServerChangeMap data to the loaded AMX script later)
	for (int i=0; i<MAX_PLUGINS; i++) {
		if (inimod_plugins[i]) {
			if (!amx_FindPublic(inimod_plugins[i], "OnServerChangeMap", &idx)) {
				amx_Exec(inimod_plugins[i], &ret, idx);
				printf("OnServerChangeMap() has been called.\n");
			}
		}
	}
	if((long)ret == 1) {
		printf("ret returned %d\n", ret);
	}
}
void CPlugins::OnMapLoaded() {
	int idx;
	cell ret = 0;
	for (int i=0; i<MAX_PLUGINS; i++) {
		if (inimod_plugins[i]) {
			//amx push would go below here.. (So we can push the OnClientConnect data to the loaded AMX script later)
			if (!amx_FindPublic(inimod_plugins[i], "OnMapLoaded", &idx)) {
				amx_Exec(inimod_plugins[i], &ret, idx);
				printf("OnMapLoaded() has been called.\n");
			}
		}
	}
	if((long)ret == 1) {
		printf("ret returned %d\n", ret);
	}
}
void CPlugins::OnClientConnect(cell playerid) {
	int idx;
	cell ret = 0;
	for (int i=0; i<MAX_PLUGINS; i++) {
		if (inimod_plugins[i]) {
			//amx push would go below here.. (So we can push the OnClientConnect data to the loaded AMX script later)
			if (!amx_FindPublic(inimod_plugins[i], "OnClientConnect", &idx)) {
				amx_Push(inimod_plugins[i], playerid);
				amx_Exec(inimod_plugins[i], &ret, idx);
				printf("OnClientConnect(%d) has been called.\n", playerid);
			}
		}
	}
	if((long)ret == 1) {
		printf("ret returned %d\n", ret);
	}
}
void CPlugins::OnClientDisconnect(int playerid, int reason) {
	int idx;
	cell ret = 0;
	for (int i=0; i<MAX_PLUGINS; i++) {
		if (inimod_plugins[i]) {
			if (!amx_FindPublic(inimod_plugins[i], "OnClientDisconnect", &idx)) {
				amx_Push(inimod_plugins[i], reason);
				amx_Push(inimod_plugins[i], playerid);
				amx_Exec(inimod_plugins[i], &ret, idx);
			}
		}
	}
	return;
}
int CPlugins::OnClientUpdate(cell playerid) { //This gets called from CBasePlayer (UpdateHeartBeat())
	//AMX Code would go here..
	int idx;
	cell ret = 0;
	for (int i=0; i<MAX_PLUGINS; i++) {
		if (inimod_plugins[i]) {
			if(!amx_FindPublic(inimod_plugins[i], "OnClientUpdate", &idx)) {
				amx_Push(inimod_plugins[i], playerid);
				amx_Exec(inimod_plugins[i], &ret, idx);
			}
		}
	}
	return (int)ret;
}
int CPlugins::OnServerThink( void ) {
	int idx;
	cell ret = 0;
	for (int i=0; i<MAX_PLUGINS; i++) {
		if (inimod_plugins[i]) {
			if(!amx_FindPublic(inimod_plugins[i], "ServerThink", &idx)) {
				amx_Exec(inimod_plugins[i], &ret, idx);
			}
		}
	}
	return (int)ret;
}
void CPlugins::OnClientCommandText(int playerid, const char* recvcmd) {
	int idx;
	cell ret = 0;
	for (int i=0; i<MAX_PLUGINS; i++) {
		if (inimod_plugins[i]) {
			if (!amx_FindPublic(inimod_plugins[i], "OnClientCommandText", &idx)) {
				cell amx_addr, *phys_addr;
				amx_PushString(inimod_plugins[i], &amx_addr, &phys_addr, recvcmd, 0, 0);
				amx_Push(inimod_plugins[i], playerid);
				amx_Exec(inimod_plugins[i], &ret, idx);
				amx_Release(inimod_plugins[i], amx_addr);
				printf("Found command param in recvcmd \"%s\", send it to amx\n", recvcmd);
			}
		}
	}
}
void CPlugins::OnClientDeath(cell attacker, cell receiver) {
	CBasePlayer *pPlayer = new CBasePlayer;
	attacker = pPlayer->GetInternalIDFromNFID(attacker);
	receiver = pPlayer->GetInternalIDFromNFID(receiver);
	int idx;
	cell ret = 0;
	for (int i=0; i<MAX_PLUGINS; i++) {
		if (inimod_plugins[i]) {
			if (!amx_FindPublic(inimod_plugins[i], "OnClientDeath", &idx)) {
				amx_Push(inimod_plugins[i], receiver);
				amx_Push(inimod_plugins[i], attacker);
				amx_Exec(inimod_plugins[i], &ret, idx);
				printf("OnClientDeath(%d, %d) has been called.\n", pPlayer->GetNFIDFromInternalID(attacker), pPlayer->GetNFIDFromInternalID(receiver));
			}
		}
	}
	delete pPlayer;
	return;
}
void CPlugins::OnClientSpawn(cell playerid) {
	CBasePlayer *pPlayer = new CBasePlayer();
	playerid = pPlayer->GetInternalIDFromNFID(playerid);
	int idx;
	cell ret = 0;
	//amx push would go below here.. (So we can push the OnClientConnect data to the loaded AMX script later)
	for (int i=0; i<MAX_PLUGINS; i++) {
		if (inimod_plugins[i]) {
			if (!amx_FindPublic(inimod_plugins[i], "OnClientSpawn", &idx)) {
				amx_Push(inimod_plugins[i], playerid);
				amx_Exec(inimod_plugins[i], &ret, idx);
				printf("OnClientSpawn(%d) has been called.\n", playerid);
			}
		}
	}
	if((long)ret == 1) {
		printf("ret returned %d\n", ret);
	}
	delete pPlayer;
}
void CPlugins::OnClientEquip(cell playerid) {
	CBasePlayer *pPlayer = new CBasePlayer();
	playerid = pPlayer->GetInternalIDFromNFID(playerid);
	int idx;
	cell ret = 0;
	//amx push would go below here.. (So we can push the OnClientConnect data to the loaded AMX script later)
	for (int i=0; i<MAX_PLUGINS; i++) {
		if (inimod_plugins[i]) {
			if (!amx_FindPublic(inimod_plugins[i], "OnClientEquip", &idx)) {
				amx_Push(inimod_plugins[i], playerid);
				amx_Exec(inimod_plugins[i], &ret, idx);
				printf("OnClientEquip(%d) has been called.\n", playerid);
			}
		}
	}
	if((long)ret == 1) {
		printf("ret returned %d\n", ret);
	}
	delete pPlayer;
}
/* Unrelated to the game mode calls - This does a client loop */
void CPlugins::UpdatePlayers( void ) {
	CBasePlayer *pPlayer = new CBasePlayer;
	for(size_t i=0; i<ReadInt32(SVS_MAXCLIENTS); i++) {
		if(pPlayer->IsClientConnected(i)) {
			if(pPlayer->IsClientOnTable(i)) {
				OnClientUpdate(i);
			}
		}
	}
	delete pPlayer;
}
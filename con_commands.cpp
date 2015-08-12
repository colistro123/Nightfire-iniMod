#include "con_commands.h"
#include "precache.h"
#include "engine.h"
#include "baseentity.h"
#include "player.h"
#include "utils.h"
#include <iostream>     // std::cout
#include <algorithm>    // std::count
#include "plugins.h"

typedef struct {
	char *name;
	RCONCmdProc proc;
} RCONCmdInfo;

RCONCmdInfo rcon_cmds[] = {
	{"help", rconcmd_availablecommands},
	{"emitambientsound", rconcmd_emitambsound},
	{"playsound", rconcmd_emitambsoundshort},
	{"preload", rconcmd_precache},
	{"players", rconcmd_players},
	{"loadplugin", rconcmd_loadplugin},
	{"unloadplugin", rconcmd_unloadplugin},
	{"unloadallplugins", rconcmd_unloadallplugins},
	{"host_timescale", rconcmd_host_timescale},
};

RCONCmdProc find_rcon_command(const char *name) {
	for(int i=0;i<sizeof(rcon_cmds)/sizeof(RCONCmdInfo);i++) {
		if(strcmpi(name,rcon_cmds[i].name) == 0) {
			return rcon_cmds[i].proc;
		}
	}
	//functionpointers = &servercommandfuncpointersinfo[nextservercommandfuncpointersinfo];
	//nextservercommandfuncpointersinfo ++;
	return NULL;
}
void CCommands::tryFindCommand(const char* recvcmd) {
	Utils *utils = new Utils;
	engineModule *engine = new engineModule;
	char paramcmd[32];
	strcpy(paramcmd, (char *)utils->ExplodeAndReturn(recvcmd, 0, " ").c_str());
	RCONCmdProc cmd = find_rcon_command(paramcmd);
	int addedCMDAddr = engine->returnAddedCmdPtr(recvcmd);
	if(cmd == NULL && addedCMDAddr == NULL) {
		printf("iniMod: Unknown Command \"%s\"\n", recvcmd);
		return;
	}
	if (!addedCMDAddr) { //Calls the regular iniMod functions
		cmd(recvcmd);
	} else { //Calls custom functions
		int (*func)(void) = (int (*)(void))addedCMDAddr;
		__asm call func
		//int x = func();
		//if(!x) {
			//printf("iniMod: Found an exception while trying to call address 0x%.8X!\n", addedCMDAddr);
			//return;
		//}
	}
	delete engine;
	delete utils;
}
int CCommands::returnCommandParamCount(const char* params) {
	bool noparams = true;
	int offset = 1;
	for (int i = 0; params[i] != '\0'; i++){
		noparams = false;
		if(params[i] == ' ') {
			offset++;
		}
	}
	if ( noparams ) 
		return 0;
	return offset;
}
void rconcmd_players(const char *params) {
	CBasePlayer *pPlayer = new CBasePlayer;
	pPlayer->ShowConnectedClients();
	delete pPlayer;
}
void rconcmd_availablecommands(const char *params) {
	printf("\n\n------------------------------\n");
	printf("Available iniMod commands:\n");
	for(int i=0;i<sizeof(rcon_cmds)/sizeof(RCONCmdInfo);i++) {
		printf("%2s %s\n", "-", rcon_cmds[i].name);
	}
	printf("------------------------------\n\n");
}
void rconcmd_emitambsound(const char *params) {
	printf("Called rconcmd_emitamb..\n");
	Utils *utils = new Utils;
	engineModule *engine = new engineModule;
	CBaseEntity *entity = new CBaseEntity;
	int entityptr = 0;
	char param[4][128];
	//Syntax: emitambientsound <sample> <volume> <channel> <pointer>
	for(int i = 0; i<sizeof(param)/sizeof(*param); i++) {
		strcpy(param[i], (char *)utils->ExplodeAndReturn(params, i+1, " ").c_str());
	}
	engine->Precache(TYPE_SND, param[0]);
	entityptr = entity->FindEntityByClassname(0, param[3]);
	entityptr = engine->EDICT_NUM(entityptr); //get edictptr
	if(entityptr != 0)
		engine->EmitAmbientSound(0, (float)atof(param[1]), param[0], atoi(param[2]), entityptr);
	delete utils;
	delete engine;
	delete entity;
	return;
}
void rconcmd_emitambsoundshort(const char *params) {
	printf("Called rconcmd_emitambshort..\n");
	Utils *utils = new Utils;
	engineModule *engine = new engineModule;
	CBaseEntity *entity = new CBaseEntity;
	int entityptr = 0;
	char param1[128];
	strcpy(param1, (char *)utils->ExplodeAndReturn(params, 1, " ").c_str()); //Sample
	engine->Precache(TYPE_SND, param1);
	entityptr = entity->FindRandomEntityPointer();
	entityptr = engine->EDICT_NUM(entityptr); //get edictptr
	if(entityptr != 0) 	
		engine->EmitAmbientSound(0, 1.0f, param1, 6, entityptr);
	delete utils;
	delete engine;
	delete entity;
	return;
}
void rconcmd_precache(const char *params) { //Syntax: File - Type
	printf("Called rconcmd_precache..\n");
	Utils *utils = new Utils;
	engineModule *engine = new engineModule;
	CCommands *cmdPtr = new CCommands;
	Precache *precache = new Precache;
	int entityptr = 0;
	char param1[128], param2[128];
	int offset = cmdPtr->returnCommandParamCount(params);
	if(offset <= 1) {
		printf("preload <type> <filename>\n");
		printf("Types: sound, model, event\n");
		return;
	}
	strcpy(param1, (char *)utils->ExplodeAndReturn(params, 1, " ").c_str()); //Type - Model, Sound, etc..
	strcpy(param2, (char *)utils->ExplodeAndReturn(params, 2, " ").c_str()); //File
	
	if(!precache->IsItemOnPrecacheTable(param2)) {
		if(find_precachetype(param1)) {
			precache->AddPrecacheToTable(find_precachetype(param1), param2);
		}
	}
	delete cmdPtr;
	delete utils;
	delete engine;
	delete precache;
	return;
}
void rconcmd_loadplugin(const char *params) {
	printf("Called rconcmd_loadplugin..\n");
	Utils *utils = new Utils;
	CCommands *cmdPtr = new CCommands;
	char param1[128], param2[128];
	int offset = cmdPtr->returnCommandParamCount(params);
	if(offset < 1) {
		printf("loadplugin <plugin_name.amx>\n");
		return;
	}
	strcpy(param1, (char *)utils->ExplodeAndReturn(params, 1, " ").c_str()); //plugin name
	CPlugins *plugin = new CPlugins;
	sprintf(param2, "%s/%s", PLUGINS_DIR, param1);
	plugin->LoadScript(param2);
	delete plugin;
	delete utils;
	delete cmdPtr;
	return;
}
void rconcmd_unloadplugin(const char *params) {
	printf("Called rconcmd_unloadplugin..\n");
	Utils *utils = new Utils;
	CCommands *cmdPtr = new CCommands;
	char param1[128], param2[128];
	int offset = cmdPtr->returnCommandParamCount(params);
	if(offset < 1) {
		printf("unloadplugin <plugin_name.amx>\n");
		return;
	}
	strcpy(param1, (char *)utils->ExplodeAndReturn(params, 1, " ").c_str()); //plugin name
	CPlugins *plugin = new CPlugins;
	plugin->RemovePluginByName(param1);
	delete plugin;
	delete utils;
	delete cmdPtr;
	return;
}
void rconcmd_unloadallplugins(const char *params) {
	printf("Called rconcmd_unloadplugin..\n");
	CPlugins *plugin = new CPlugins;
	plugin->UnloadAllPlugins();
	delete plugin;
	return;
}
void rconcmd_host_timescale(const char *params) {
	printf("Called rconcmd_host_timescale..\n");
	Utils *utils = new Utils;
	engineModule *engine = new engineModule;
	CCommands *cmdPtr = new CCommands;
	char param1[128], param2[128];
	int offset = cmdPtr->returnCommandParamCount(params);
	if(offset < 1) {
		char buffer[64];
		sprintf(buffer, "host_timescale is %f\n", engine->host_timescale);
		printf(buffer);
		return;
	}
	strcpy(param1, (char *)utils->ExplodeAndReturn(params, 1, " ").c_str()); //value
	engine->host_timescale = stof((const char*)param1);
	delete utils;
	delete cmdPtr;
	delete engine;
	return;
}
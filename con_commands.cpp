#include "con_commands.h"
#include "precache.h"
#include "engine.h"
#include "utils.h"
#include <iostream>     // std::cout
#include <algorithm>    // std::count

typedef struct {
	char *name;
	RCONCmdProc proc;
} RCONCmdInfo;

RCONCmdInfo rcon_cmds[] = {
	{"emitambientsound", rconcmd_emitambsound},
	{"playsound", rconcmd_emitambsoundshort},
	{"preload", rconcmd_precache}
};

RCONCmdProc find_rcon_command(const char *name) {
	for(int i=0;i<sizeof(rcon_cmds)/sizeof(RCONCmdInfo);i++) {
		if(strcmpi(name,rcon_cmds[i].name) == 0) {
			return rcon_cmds[i].proc;
		}
	}
	return NULL;
}

void CCommands::tryFindCommand(const char* recvcmd) {
	printf("value %s\n", recvcmd);
	Utils *utils = new Utils;
	engineModule *engine = new engineModule;
	char paramcmd[32];
	strcpy(paramcmd, (char *)utils->ExplodeAndReturn(recvcmd, 0, " ").c_str());
	RCONCmdProc cmd = find_rcon_command(paramcmd);
	if(cmd == NULL) {
		printf("iniMod: Unknown Command \"%s\"\n", recvcmd);
		return;
	}
	cmd(recvcmd);
	delete engine;
	delete utils;
}
int CCommands::returnCommandParamCount(const char* params) {
	int offset = 0;
	for (int i = 0; params[i] != '\0'; i++){
		if(params[i] == ' ') {
			offset++;
		}
	}
	return offset;
}
void rconcmd_emitambsound(const char *params) {
	printf("Called rconcmd_emitamb..\n");
	Utils *utils = new Utils;
	engineModule *engine = new engineModule;
	int entityptr = 0;
	char param1[128], param2[24], param3[24], param4[24];
	strcpy(param1, (char *)utils->ExplodeAndReturn(params, 1, " ").c_str()); //Sample
	strcpy(param2, (char *)utils->ExplodeAndReturn(params, 2, " ").c_str()); //Volume
	strcpy(param3, (char *)utils->ExplodeAndReturn(params, 3, " ").c_str()); //Channel
	strcpy(param4, (char *)utils->ExplodeAndReturn(params, 4, " ").c_str()); //Pointer
	engine->Precache(TYPE_SND, param1);
	entityptr = engine->FindEntityByClassname(0, param4);
	entityptr = engine->EDICT_NUM(entityptr); //get edictptr
	if(entityptr != 0)
		engine->EmitAmbientSound(0, (float)atof(param2), param1, atoi(param3), entityptr);
	delete utils;
	delete engine;
	return;
}
void rconcmd_emitambsoundshort(const char *params) {
	printf("Called rconcmd_emitambshort..\n");
	Utils *utils = new Utils;
	engineModule *engine = new engineModule;
	int entityptr = 0;
	char param1[128];
	strcpy(param1, (char *)utils->ExplodeAndReturn(params, 1, " ").c_str()); //Sample
	engine->Precache(TYPE_SND, param1);
	entityptr = engine->FindRandomEntityPointer();
	entityptr = engine->EDICT_NUM(entityptr); //get edictptr
	if(entityptr != 0) 	
		engine->EmitAmbientSound(0, 1.0f, param1, 6, entityptr);
	delete utils;
	delete engine;
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
		printf("Less than 1 param or 1 param for %s\n", params);
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

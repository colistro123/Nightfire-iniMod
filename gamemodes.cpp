#include "gamemodes.h"
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
#include "plugins.h"

#define CONFIG_FILE				"iniMod.cfg"
#define GAMEMODES_DIR			"gamemodes"
//#undef UNICODE

#define VERIFY_PAWN_NATIVE(amount) { if (params[0] != (amount * sizeof(cell))) { printf("WARNING: Wrong parameter count (Count is %d, Should be %d)\n", amount, params[0] / sizeof(cell)); return 0; }}

//Set default values to the static values we initialized in gamemodes.h
bool CNetwork::GMLoaded = false;
AMX CNetwork::inimod_amx;
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

enum {
	TYPE_GAMEMODE,
	TYPE_UPDATERATE,
	TYPE_PLUGIN,
};

string GMVars[] = {
	"gamemode", //TYPE_GAMEMODE
	"updaterate", //TYPE_UPDATERATE
	"plugins" //TYPE_PLUGIN
};

//Wrapper functions (These are defined so we can use them on the script later)
static cell AMX_NATIVE_CALL n_print(AMX* amx, const cell *params)
{
        char *ret = NULL;
        amx_StrParam(amx, params[1], ret);
        printf("%s\n", ret);
        return 0;
}
static cell AMX_NATIVE_CALL n_EDICT_NUM(AMX* amx, const cell *params)
{
		engineModule *engine = new engineModule;
		int edictptr = engine->EDICT_NUM(params[1]);
		delete engine;
        return edictptr;
}
static cell AMX_NATIVE_CALL n_EmitSound(AMX* amx, const cell *params)
{
        char *ret = NULL;
		char *charsample = NULL;
        amx_StrParam(amx, params[3], charsample);
		// EmitSound(edict, channel, charsample[], Float:volume, Float:attenuation, fFlags, pitch);
		engineModule *engine = new engineModule;
		//push 64, push 0, PUSH 3F4CCCCD [0.800000011920929], PUSH 3F800000 [1], PUSH 4210E174 [sample], PUSH 1, PUSH ESI
		engine->EmitSound(params[1], params[2], charsample, amx_ctof(params[4]), amx_ctof(params[5]), params[6], params[7]);
        //printf("EmitSound(%i, %i, %s, %f, %f, %i, %i)\n", params[1], params[2], charsample, amx_ctof(params[4]), amx_ctof(params[5]), params[6], params[7]);
		delete engine;
        return 0;
}
static cell AMX_NATIVE_CALL n_EmitAmbientSound(AMX* amx, const cell *params)
{
        char *ret = NULL;
		char *charsample = NULL;
        amx_StrParam(amx, params[3], charsample);
		engineModule *engine = new engineModule;
		//flags, float volume, sound, channel, edict
		engine->EmitAmbientSound(params[1], amx_ctof(params[2]), charsample, params[4], params[5]);
		delete engine;
        return 0;
}
static cell AMX_NATIVE_CALL n_ConsoleCommand(AMX* amx, const cell *params)
{
        char *ret = NULL;
        amx_StrParam(amx, params[1], ret);
		engineModule *engine = new engineModule;
		engine->ConsoleCommand(ret);
		delete engine;
        return 0;
}
static cell AMX_NATIVE_CALL n_PrecacheSound(AMX* amx, const cell *params)
{
        char *ret = NULL;
        amx_StrParam(amx, params[1], ret);
		engineModule *engine = new engineModule;
		engine->Precache(TYPE_SND, ret);
		delete engine;
        return 0;
}
static cell AMX_NATIVE_CALL n_PrecacheModel(AMX* amx, const cell *params)
{
        char *ret = NULL;
        amx_StrParam(amx, params[1], ret);
		engineModule *engine = new engineModule;
		int m_Index = engine->Precache(TYPE_MDL, ret);
		delete engine;
        return m_Index;
}
static cell AMX_NATIVE_CALL n_PrecacheGeneric(AMX* amx, const cell *params)
{
        char *ret = NULL;
        amx_StrParam(amx, params[1], ret);
		engineModule *engine = new engineModule;
		engine->Precache(TYPE_GENERIC, ret);
		delete engine;
        return 0;
}
static cell AMX_NATIVE_CALL n_PrecacheEvent(AMX* amx, const cell *params)
{
        char *ret = NULL;
        amx_StrParam(amx, params[1], ret);
		engineModule *engine = new engineModule;
		engine->Precache(TYPE_EVENT, ret);
		delete engine;
        return 0;
}
static cell AMX_NATIVE_CALL n_PrecacheUnmodified(AMX* amx, const cell *params)
{
        char *ret = NULL;
        amx_StrParam(amx, params[1], ret);
		engineModule *engine = new engineModule;
		engine->Precache(TYPE_UNMODIFIED, ret);
		delete engine;
        return 0;
}
static cell AMX_NATIVE_CALL n_GetGameTimeLeft(AMX *amx, const cell *params) {
	engineModule *engine = new engineModule;
    int timeleft = engine->GetGameTimeLeft();
	delete engine;
	return timeleft;
}
static cell AMX_NATIVE_CALL n_FindEntityByClassname(AMX *amx, const cell *params) {
	CBaseEntity *entity = new CBaseEntity;
	char *ret = NULL;
    amx_StrParam(amx, params[2], ret);
    int id = entity->FindEntityByClassname(params[1], ret);
	delete entity;
	return id;
}
static cell AMX_NATIVE_CALL n_SUB_Remove(AMX* amx, const cell *params)
{
		CBaseEntity *entity = new CBaseEntity;
		entity->SUB_Remove(params[1]);
		delete entity;
        return 1;
}
static cell AMX_NATIVE_CALL n_SetQueryVar(AMX *amx, const cell *params) {
	engineModule *engine = new engineModule;
	char *var = NULL;
	char *vardesc = NULL;
    amx_StrParam(amx, params[1], var);
	amx_StrParam(amx, params[2], vardesc);
    engine->SetQueryVar(var, vardesc);
	delete engine;
	return 1;
}
static cell AMX_NATIVE_CALL n_Sys_FloatTime(AMX *amx, const cell *params) {
	return Sys_FloatTime() * 1000.0f; //return milliseconds
}
//For some reason, all the gamemode natives have to go here.
AMX_NATIVE_INFO gamemode_Natives[] =
{
	//GameMode Related

	{"GetGameTimeLeft", n_GetGameTimeLeft},
	//Player Related
	{"HasClientSpawned", n_HasClientSpawned},
	{"GetPlayerDeaths", n_GetPlayerDeaths},
	{"GetPlayerKills", n_GetPlayerKills},
	{"IsClientConnected", n_IsClientConnected},
	{"SetPlayerHealth", n_SetPlayerHealth},
	{"GetPlayerHealth", n_GetPlayerHealth},
	{"SpectatePlayer", n_SpectatePlayer},
	{"RemoveAllItems", n_RemoveAllItems},
	{"GiveNamedItem", n_GiveNamedItem},
	{"GetClientName", n_GetClientName},
	{"GetPlayerPos", n_GetPlayerPos},
	{"SetPlayerPos", n_SetPlayerPos},
	{"KickClient", n_KickClient},
	//GUI related
	{"TriggerHudMsg", n_TriggerHudMsg},
	//Utils
	{"print", n_print},
	{"ConsoleCommand", n_ConsoleCommand},
	{"PrecacheSound", n_PrecacheSound},
	{"PrecacheModel", n_PrecacheModel},
	{"PrecacheGeneric", n_PrecacheGeneric},
	{"PrecacheEvent", n_PrecacheEvent},
	{"PrecacheUnmodified", n_PrecacheUnmodified},
	{"EmitSound", n_EmitSound},
	{"EmitAmbientSound", n_EmitAmbientSound},
	{"FindEntityByClassname", n_FindEntityByClassname},
	{"EDICT_NUM", n_EDICT_NUM},
	{"SUB_Remove", n_SUB_Remove},
	{"SetQueryVar", n_SetQueryVar},
	{"GetPointDistance", n_GetPointDistance},
	{"CenterPrint", n_CenterPrint},
	{"SetView", n_SetView},
	{"ClientPrint", n_ClientPrint},
	{"UTIL_ScreenFade", n_UTIL_ScreenFade},
	{"UTIL_SayTextFilter", n_UTIL_SayTextFilter},
	{"UTIL_ShowMessage", n_UTIL_ShowMessage},
	{"UTIL_SetVisionColor", n_UTIL_SetVisionColor},
	{"UTIL_CreateBeamEntsPlayers", n_UTIL_CreateBeamEntsPlayers},
	{"UTIL_Create_TE_DLIGHT", n_UTIL_Create_TE_DLIGHT},
	{"GetPlayerEdictPtr", n_GetPlayerEdictPtr},
	{"Sys_FloatTime", n_Sys_FloatTime},
	{"EyeAngles", n_EyeAngles},
	{"WeaponAngles", n_WeaponAngles},
	{"GetGunPos", n_GetGunPos},
    {NULL,NULL}
    /* terminator */
};

int amx_initNatives(AMX *amx)
{
  return amx_Register(amx, gamemode_Natives, -1);
}

//Inlines
void CNetwork::parseConfigFile( void ) {
	if(GMLoaded || (GetTickCount() - loadTime) < 15000)
		return;
	string contents;
	ifstream myfile;
    myfile.open(CONFIG_FILE);
    if (myfile.is_open()) {
		Utils *utils = new Utils();
        while (getline(myfile, contents)) {
			std::string xcontent = contents;
			char *output = (char *)xcontent.c_str();
			decideParse((char *)utils->ExplodeAndReturn(output, 0, ":").c_str(),(char *)utils->ExplodeAndReturn(output, 1, ":").c_str());
        }
		delete utils;
        myfile.close();
    }
    else
        cout << "Unable to get file (reason: " << strerror(errno) << ")." << endl;
}
void CNetwork::decideParse(char* param1, char* param2) {
	CPlugins *plugin = new CPlugins;
	char string[256];
	for(int i = 0; i < sizeof(GMVars)/sizeof(*GMVars); ++i) {
		if(strcmp((char *)GMVars[i].c_str(),param1) == false) {
			switch(i) {
				case TYPE_GAMEMODE: {
					engineModule *engine = new engineModule;
					sprintf_s(string, "%s/%s.amx",GAMEMODES_DIR, param2);
					loadTime = GetTickCount();
					if(LoadScript(string))
						initAMXGM();
					else
						engine->SHOW_TO_CONSOLE_AND_EXIT("The server will now close, please specify an AMX file or run without gamemode support by specifying it in the iniMod.cfg file.\n");
						delete engine;
					break;
				}
				case TYPE_UPDATERATE: {
					break;
				}
				case TYPE_PLUGIN: {
					plugin->ParsePlugins(param2);
					GMLoaded = true;
					break;
				}
			}
		}
	}
	delete plugin;
	return;
}
bool CNetwork::LoadScript(char *filename) {
    size_t memsize = aux_ProgramSize(filename);
    if(memsize == 0) {
        printf("Script file not found or corrupted\n");
        return false;
    }
	memset((void*)&inimod_amx, 0, sizeof(AMX));
    void * program = malloc(memsize);

    if (program == NULL) {
        printf("Memory allocation for script failed\n");
        return false;
    }
    if(!LoadProgram(filename,program)) {
        printf("Loading script into Abstract Machine failed\n");
        return false;
    }
	amx_CoreInit(&inimod_amx);
	amx_FloatInit(&inimod_amx);
	amx_StringInit(&inimod_amx);
	//amx_FileInit(&inimod_amx);
	amx_TimeInit(&inimod_amx);
    return true;
}
void CNetwork::UnloadScript() {
	int val;
	if (!amx_FindPublic(&inimod_amx, "OnGameModeExit", &val))
		amx_Exec(&inimod_amx, (cell*)&val, val);

	if (GMLoaded) {
		aux_FreeProgram(&inimod_amx);
		amx_TimeCleanup(&inimod_amx);
		//amx_FileCleanup(&inimod_amx);
		amx_StringCleanup(&inimod_amx);
		amx_FloatCleanup(&inimod_amx);
		amx_CoreCleanup(&inimod_amx);
	}
	GMLoaded = false;
}
void CNetwork::Error(int error) {
    printf("Run time error %d: \"%s\"\n",
           error, aux_StrError(error));
}
bool CNetwork::LoadProgram(char* file,void * _program) {
    this->err = aux_LoadProgram(&inimod_amx, file, _program);
    if (err != AMX_ERR_NONE) {
        Error(this->err);
        return false;
    }
    return true;
}
void CNetwork::initAMXGM( void ) {
	if(registerNatives(gamemode_Natives)) {
		ExecMain();
		OnGameModeInit(); //Everything is loaded, init our gamemode..
		mainHooks();
		RestartMap();
	}
}
void CNetwork::RestartMap( void ) {
	char cmd[64];
	strcpy(cmd, "changelevel ");
	std::string map = ReadCharArray(SV_CURRENTMAP, 32);
	const char *charmap = map.c_str();
	strcat(cmd, charmap);

	engineModule *engine = new engineModule();
	engine->ConsoleCommand(cmd);
	delete engine;
}
int CNetwork::ExecMain() {
    cell ret;
    this->err = amx_Exec(&inimod_amx, &ret, AMX_EXEC_MAIN);
    if (err != AMX_ERR_NONE)
        Error( this->err);
    if (ret != 0)
        printf("Returns %ld\n", (long)ret);
    return ret;
}
bool CNetwork::registerNatives(const AMX_NATIVE_INFO *list) {
    err = amx_Register(&inimod_amx,list,-1);
    int num = 0;
    amx_NumNatives(&inimod_amx,&num);
    printf("Registered %i native functions.\n",num);
    return true;
}
//General "Network" / Gamemode AMX Code
void CNetwork::OnServerChangeMap() {
	printf("OnServerChangeMap() has been called.\n");
	int idx;
	cell ret = 0;
	CBaseEntity *entity = new CBaseEntity;
	Precache *precache = new Precache;

	CBasePlayer *pPlayer = new CBasePlayer();
	//Dump all the clients from the table because the map was changed..
	for(size_t i=0; i<ReadInt32(SVS_MAXCLIENTS); i++) {
		if(pPlayer->IsClientConnected(i)) {
			if(pPlayer->IsClientOnTable(i)) {
				OnClientDisconnect(i, REASON_MAPCHANGE);
			}
		}
	}
	delete pPlayer;
	//amx push would go below here.. (So we can push the OnServerChangeMap data to the loaded AMX script later)
	if (!amx_FindPublic(&inimod_amx, "OnServerChangeMap", &idx)) {
		amx_Exec(&inimod_amx, &ret, idx);
		precache->PreloadCached(false); //Don't delete anything the users precached..
	}
	if((long)ret == 1) {
		printf("ret returned %d\n", ret);
	}
	entity->OnServerChangeMap();
	delete entity;
	delete precache;
}
void CNetwork::OnMapLoaded() {
	printf("OnMapLoaded() has been called.\n");
	int idx;
	cell ret = 0;
	CBasePlayer *pPlayer = new CBasePlayer();

	pPlayer->ProcessClientsOnMapLoad();

	//amx push would go below here.. (So we can push the OnMapLoaded data to the loaded AMX script later)
	if (!amx_FindPublic(&inimod_amx, "OnMapLoaded", &idx)) {
		amx_Exec(&inimod_amx, &ret, idx);
	}
	if((long)ret == 1) {
		printf("ret returned %d\n", ret);
	}
	delete pPlayer;
}
void CNetwork::OnClientConnect(cell playerid) {
	CBasePlayer *pPlayer = new CBasePlayer();
	pPlayer->UpdateClientsTable(playerid, ADD_TO_TABLE);
	printf("OnClientConnect(%d) has been called.\n", playerid);
	int idx;
	cell ret = 0;
	//amx push would go below here.. (So we can push the OnClientConnect data to the loaded AMX script later)
	if (!amx_FindPublic(&inimod_amx, "OnClientConnect", &idx)) {
		amx_Push(&inimod_amx, playerid);
		amx_Exec(&inimod_amx, &ret, idx);
	}
	if((long)ret == 1) {
		printf("ret returned %d\n", ret);
	}
	delete pPlayer;
}
void CNetwork::OnClientReconnect(cell playerid) {
	CBasePlayer *pPlayer = new CBasePlayer();
	printf("OnClientReconnect(%d) has been called\n", playerid);
	if(pPlayer->IsClientOnTable(playerid)) { 
		OnClientDisconnect(playerid, REASON_DISCONNECT); //The client disconnected remove them from the table to prevent the glitching bug -1
	}
	delete pPlayer;
}
void CNetwork::OnClientDisconnect(int playerid, int reason) {
	CBasePlayer *pPlayer = new CBasePlayer;
	CBaseEntity *entity = new CBaseEntity;
	if(!pPlayer->IsClientOnTable(playerid) || !pPlayer->IsClientConnected(playerid)) {
		printf("Couldn't find client %d in the table.\n", playerid);
		return; //The specified client is not on the table so don't do anything..
	}
	pPlayer->NumPlayersWithoutDynamicLightSupport-= !pPlayer->SupportsDynamicLights(playerid);
	printf("OnClientDisconnect(%d, %d) has been called. (%s)\n", playerid, reason, reason == 1 ? "Disconnect" : "Timeout");
	pPlayer->UpdateClientsTable(playerid, REMOVE_FROM_TABLE); //Update the clients table since a client disconnected
	//amx push would go below here.. (So we can push the OnClientDisconnect data to the loaded AMX script later)
	int idx;
	cell ret = 0;
	if (!amx_FindPublic(&inimod_amx, "OnClientDisconnect", &idx)) {
		amx_Push(&inimod_amx, reason);
		amx_Push(&inimod_amx, playerid);
		amx_Exec(&inimod_amx, &ret, idx);
	}
	entity->EntitiesOnClientDisconnect(playerid);
	delete pPlayer;
	delete entity;
	return;
}
int CNetwork::OnClientUpdate(cell playerid) { //This gets called from CBasePlayer (UpdateHeartBeat())
	//AMX Code would go here..
	int idx;
	cell ret = 0;
	if(!amx_FindPublic(&inimod_amx, "OnClientUpdate", &idx)) {
		amx_Push(&inimod_amx, playerid);
		amx_Exec(&inimod_amx, &ret, idx);
	}
	return (int)ret;
}
void CNetwork::OnGameModeInit() { //Here the host can decide what he/she wants to do with the server.. This gets called once the gamemode is loaded ofcourse..
	int idx;
	cell ret = 0;
	if(!amx_FindPublic(&inimod_amx, "OnGameModeInit", &idx)) {
		amx_Exec(&inimod_amx, &ret, idx);
	}
	if((long)ret == 1) {
		printf("ret returned %d\n", ret);
	}
	return;
}
int CNetwork::OnServerThink( void ) {
	int idx;
	cell ret = 0;
	if(!amx_FindPublic(&inimod_amx, "ServerThink", &idx)) {
		amx_Exec(&inimod_amx, &ret, idx);
	}
	return (int)ret;
}
void CNetwork::OnClientCommandText(int playerid, const char* recvcmd) {
	int idx;
	cell ret = 0;
	printf("Found command param in recvcmd \"%s\", send it to amx\n", recvcmd);
	if (!amx_FindPublic(&inimod_amx, "OnClientCommandText", &idx)) {
		cell amx_addr, *phys_addr;
		amx_PushString(&inimod_amx, &amx_addr, &phys_addr, recvcmd, 0, 0);
		amx_Push(&inimod_amx, playerid);
		amx_Exec(&inimod_amx, &ret, idx);
		amx_Release(&inimod_amx, amx_addr);
	}
}
void CNetwork::OnClientDeath(cell attacker, cell receiver) {
	CBasePlayer *pPlayer = new CBasePlayer;
	attacker = pPlayer->GetInternalIDFromNFID(attacker);
	receiver = pPlayer->GetInternalIDFromNFID(receiver);
	printf("OnClientDeath(%d, %d) has been called.\n", pPlayer->GetNFIDFromInternalID(attacker), pPlayer->GetNFIDFromInternalID(receiver));
	int idx;
	cell ret = 0;
	if (!amx_FindPublic(&inimod_amx, "OnClientDeath", &idx)) {
		amx_Push(&inimod_amx, receiver);
		amx_Push(&inimod_amx, attacker);
		amx_Exec(&inimod_amx, &ret, idx);
	}
	delete pPlayer;
	return;
}
void CNetwork::OnClientSpawn(cell playerid) {
	 CBasePlayer *pPlayer = new CBasePlayer();
	 playerid = pPlayer->GetInternalIDFromNFID(playerid);
	 printf("OnClientSpawn(%d) has been called.\n", playerid);
	 int idx;
	 cell ret = 0;
	 //amx push would go below here.. (So we can push the OnClientConnect data to the loaded AMX script later)
	 if (!amx_FindPublic(&inimod_amx, "OnClientSpawn", &idx)) {
	  amx_Push(&inimod_amx, playerid);
	  amx_Exec(&inimod_amx, &ret, idx);
	 }
	 if((long)ret == 1) {
	  printf("ret returned %d\n", ret);
	 }
	 delete pPlayer;
}
void CNetwork::OnClientEquip(cell playerid) {
	CBasePlayer *pPlayer = new CBasePlayer();
	playerid = pPlayer->GetInternalIDFromNFID(playerid);
	printf("OnClientEquip(%d) has been called.\n", playerid);
	int idx;
	cell ret = 0;
	//amx push would go below here.. (So we can push the OnClientConnect data to the loaded AMX script later)
	if (!amx_FindPublic(&inimod_amx, "OnClientEquip", &idx)) {
		amx_Push(&inimod_amx, playerid);
		amx_Exec(&inimod_amx, &ret, idx);
	}
	if((long)ret == 1) {
		printf("ret returned %d\n", ret);
	}
	delete pPlayer;
}
/* Unrelated to the game mode calls - This does a client loop */
void CNetwork::UpdatePlayers( void ) {
	CBasePlayer *pPlayer = new CBasePlayer;
	for(size_t i=0; i<ReadInt32(SVS_MAXCLIENTS); i++) {
		if(pPlayer->IsClientConnected(i)) {
			if(pPlayer->IsClientOnTable(i)) {
				OnClientUpdate(i);
			}else {
				printf("CNetwork::The client was not on the table, so he or she has been added to it. id: %d\n", i);
				pPlayer->AddPlayerToTable(i);
				OnClientUpdate(i);
			}
		}
	}
	delete pPlayer;
}
#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>
#include <cstdio>
#include <cmath>
#include <math.h>
#include <conio.h>
#include <iostream>
#include <winsock2.h>
#include <time.h>
#include <fstream>
#include <string>
#include <vector>
#include < extdll.h >
#include "processmemory.h"
#include "amx/amx.h"
#include "amxutils.h"
//#include "amx/amxaux.h"

#define PLAYER_TABLE_CELLSIZE	20248
#define MAX_PLAYERS 32

#define NEXT_BEAT 1
#define LAST_BEAT 2
#define REMOVE_FROM_TABLE	1
#define ADD_TO_TABLE		2
using namespace std;

//Player table definition
typedef struct _PlayerInfo {
	char name[MAX_PLAYER_NAME];
	char playermodel[24];
	BYTE score;
	BYTE ping;
	UINT8 connected;
	UINT8 pactive;
	UINT8 pspawned;
	UINT8 pconnected;
	UINT8 pfakeclient;
	UINT8 pedicts;// Table - open with memOpen
	string pname;
	UINT8 puserid;
	string puserinfo;
	int pconnecttime;
	UINT8 plagcompensation;
	UINT8 pnext_messageinterval;
public:
	int pnext_heartbeat;
	int plast_heartbeat;
	int pinternaluserid;
} PlayerInfo;

//int std::vector<PlayerInfo> players;

//enums
enum client_t {             //Client
	pactive = 0,
	pspawned = 1,
	pconnected = 3,
	pfakeclient = 0x2528,
	pUnknown = 0x4C88, //byte
	pUnknown2 = 0x4C8C, //float
	pedicts = 0x4B78,// ' Table - open with memOpen
	pname = 0x4CD0,
	puserid = 0x4B80,
	puserinfo = 0x4B88,
	pconnecttime = 0x28,
	plagcompensation = 0x4F10,
	pnext_messageinterval = 0x3568,// 'cl_updaterate
};
enum edict {                  //'Entity
	pentity = 0x11C, // Table - open with memOpen
	pkills = 644,
	pposX = 296,
	pposY = 300,
	pposZ = 304,
	pvelX = 320,
	pvelY = 324,
	pvelZ = 328,
};
enum player {              //Player
	pdeaths = 0xB30,
	punknown4 = 0x4,
	phealth = 0x160,
	pctfscore1 = 0xB68,
	pctfscore2 = 0xB6C, //both ctf scores are identical, who knows why
};

class CBasePlayer {
public:
	CBasePlayer() { };
    //Destructor
    virtual ~CBasePlayer() { };
	//Functions
	void SetPlayerHealth(int playerid, int percent);
	float GetPlayerHealth( int playerid );
	int GetPlayerDeaths( int playerid );
	float GetPlayerKills( int playerid );
	int HasClientSpawned( int playerid );
	void setPointerForPlayerID( int playerid ); //Just a little function I made to save lines when setting pointers..
	int OnClientSpawn( int playerid );
	int IsClientConnected( int playerid );
	int IsAClient( int playerid );
	void AddPlayerToTable(int playerid );
	int IsClientOnTable( int playerid ); //This one checks if they're on the table while the other one checks if they're connected as well ** existing clients **
	int IsClientOnTableByNFID ( int playerid );
	void RemovePlayerFromTable( int playerid );
	void UpdateClientsTable( int playerid, int option );
	void ShowConnectedClients( void );
	void UpdateClients( void );
	virtual int MaxClients() { return (UINT32)ReadInt32(SV_MAXCLIENTS); } //These are the max clients on the server
	virtual int NumClients() { return (UINT32)ReadInt32(SV_NUMCLIENTS); } //These are the clients on the server
	string FetchInfoForPlayerID( int playerid );
	void ClientCommand(char *text);
	int GetConnectTime(int playerid);
	void UpdateHeartBeat( int playerid );
	int GetBeat( int playerid, int type ); //1 for next, 2 for last
	bool LostBeat(int playerid);
	int GetLastBeat(int playerid);
	int GetNextBeat(int playerid);
	int NumClientsOnTable(); //These are the amount of clients on our internal player table where we store our player data
	int GetNFIDFromInternalID( int internalid ); //This allows us to get their NF ID by knowing their internal ID.
	int IsClientConnectedEx( int playerid );
	int GetFreeIndex(); //Gets a free index to set add a player to the table
	int GetInternalIDFromNFID(int nfuserid);
	void GiveNamedItem(int playerid, char *wepname, int iSubType);
	void RemoveAllItems(int playerid, int dukes); //A value of 0 allows you to keep the dukes and 0 strips them away
	void Weapon_SetLast(int playerid, char *wepname); //Set their last weapon
	void SetPlayerModel(int playerid, char *mdlname);
	void SpectatePlayer(int playerid, int tospectate);
	char* GetClientName(int playerid);
	float GetPlayerPos(int playerid, float &X, float &Y, float &Z);
	void SetPlayerPos(int playerid, float X, float Y, float Z);
	//definitions
	static int cptr;
	static int edictptr;
	static int ptr;
	static double curtime;
	static double lasttick;
	static int currentclient;

	static char* gamever;
	static int lagcompensation;
	static int connected;
	static int active;
	static int spawned;
	static int IsFakeClient;
	static int userid;
	std::string userinfostring;
	static int amountoftimeconnected_secs; //This should be integer
	static int amountoftimeconnected_mins; //This too

	static double cl_updaterate;
	//player's current health
	static float health;
	static char* name;
	static float kills;
	static int deaths;
	static int ctfscore1;
	static int ctfscore2;
	static float x;
	static float y;
	static float z;
	static char* alivestatus;
	static char* bondexe;
};

//Wrapper functions
static cell AMX_NATIVE_CALL n_HasClientSpawned(AMX *amx, const cell *params) {
	CBasePlayer *pPlayer = new CBasePlayer;
    int hasSpawned = pPlayer->HasClientSpawned( (int)params[1] );
	delete pPlayer;
	return hasSpawned;
}
static cell AMX_NATIVE_CALL n_GetPlayerDeaths(AMX *amx, const cell *params) {
	CBasePlayer *pPlayer = new CBasePlayer;
    int deaths = pPlayer->GetPlayerDeaths( (int)params[1] );
	delete pPlayer;
	return deaths;
}
static cell AMX_NATIVE_CALL n_GetPlayerKills(AMX *amx, const cell *params) {
	CBasePlayer *pPlayer = new CBasePlayer;
    int pKills = (int)pPlayer->GetPlayerKills( (float)params[1] );
	delete pPlayer;
	return pKills;
}
static cell AMX_NATIVE_CALL n_IsClientConnected(AMX *amx, const cell *params) {
	CBasePlayer *pPlayer = new CBasePlayer;
    int connected = pPlayer->IsClientConnected( (int)params[1] );
	delete pPlayer;
	return connected;
}
static cell AMX_NATIVE_CALL n_SetPlayerHealth(AMX *amx, const cell *params) {
	CBasePlayer *pPlayer = new CBasePlayer;
    pPlayer->SetPlayerHealth( (int)params[1], (int)params[2] );
	delete pPlayer;
	return true;
}
static cell AMX_NATIVE_CALL n_GetPlayerHealth(AMX *amx, const cell *params) {
	CBasePlayer *pPlayer = new CBasePlayer;
    int health = (int)pPlayer->GetPlayerHealth( (int)params[1] );
	delete pPlayer;
	return health;
}
static cell AMX_NATIVE_CALL n_SpectatePlayer(AMX *amx, const cell *params) {
	CBasePlayer *pPlayer = new CBasePlayer;
    pPlayer->SpectatePlayer( (int)params[1], (int)params[2] );
	delete pPlayer;
	return true;
}
static cell AMX_NATIVE_CALL n_RemoveAllItems(AMX *amx, const cell *params) {
	CBasePlayer *pPlayer = new CBasePlayer;
    pPlayer->RemoveAllItems( (int)params[1], (int)params[2] );
	delete pPlayer;
	return true;
}
static cell AMX_NATIVE_CALL n_GiveNamedItem(AMX *amx, const cell *params) {
	CBasePlayer *pPlayer = new CBasePlayer;
	char *wepname = NULL;
    amx_StrParam_Type(amx, params[2], wepname, char*);
    pPlayer->GiveNamedItem( (int)params[1], wepname, (int)params[3] );
	delete pPlayer;
	return true;
}
static cell AMX_NATIVE_CALL n_GetClientName(AMX *amx, const cell *params) {
	CBasePlayer *pPlayer = new CBasePlayer;
	amx_SetString((cell*)params[2], (const char*)pPlayer->GetClientName( (int)params[1] ), true, false, params[3]);
	delete pPlayer;
	return true;
}
static cell AMX_NATIVE_CALL n_GetPlayerPos(AMX *amx, const cell *params) { //Crashes everything, thanks AMX
	//This function passes XYZ by reference
	CBasePlayer *pPlayer = new CBasePlayer;
	float X, Y, Z;
	pPlayer->GetPlayerPos((int)params[1], X, Y, Z);
	cell* addr[3] = {NULL, NULL, NULL};
	int eCode;
	for(int i=0; i<3; i++) {
		eCode = amx_GetAddr(amx, params[i+2], &addr[i]) == AMX_ERR_MEMACCESS ? printf("Error while checking &addr[%d]: (%x)\n", i, &addr[i]) : 0;
	}
	if (eCode) {
		printf("Last error code at n_GetPlayerPos was %d\n", eCode);
		return false;
	}
	*addr[0] = amx_ftoc(X);
	*addr[1] = amx_ftoc(Y);
	*addr[2] = amx_ftoc(Z);
	delete pPlayer;
	return true;
}
static cell AMX_NATIVE_CALL n_SetPlayerPos(AMX *amx, const cell *params) {
	CBasePlayer *pPlayer = new CBasePlayer;
	pPlayer->SetPlayerPos((int)params[1], amx_ctof(params[2]), amx_ctof(params[3]), amx_ctof(params[4]));
	delete pPlayer;
	return true;
}
#endif

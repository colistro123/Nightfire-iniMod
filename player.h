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

//Disconnect Reasons
#define REASON_DISCONNECT		1
#define REASON_TIMEOUT			2
#define REASON_MAPCHANGE		3

//Other player vars
#define PLAYER_TABLE_CELLSIZE	20248
#define MAX_PLAYERS				32
#define CLIENT_SCAN_TIME		5000.0f //We update the clients every 5 seconds
#define NEXT_BEAT				1
#define LAST_BEAT				2
#define REMOVE_FROM_TABLE		1
#define ADD_TO_TABLE			2
#define ADD_TO_QUEUE			3	//Puts the client on queue if there's a map change (Removed)

//Client states
#define STATE_CONNECTED			1	
#define STATE_QUEUE				2
#define STATE_INVALID			3

#define INVALID_USERID			-1

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
	int puserid;
	string puserinfo;
	int pconnecttime;
	UINT8 plagcompensation;
	UINT8 pnext_messageinterval;
public:
	int pnext_heartbeat;
	int plast_heartbeat;
	int pinternaluserid;
	int pclientstate;
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
	int GetEdictNum(int clientptr);
	int getPointerForPlayerID( int playerid ); //Just a little function I made to save lines when getting pointers..
	int OnClientSpawn( int playerid );
	int OnClientEquip(int playerid);
	void ProcessClientsOnMapLoad( void );
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
	void ClientCommand(char *text, int edictptr);
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
	string GetClientName(int playerid);
	float GetPlayerPos(int playerid, float &X, float &Y, float &Z);
	void SetPlayerPos(int playerid, float X, float Y, float Z);
	void SetPlayerTeam(int playerid, int team);
	int GetPlayerUserID( int edictptr );
	void OnPlayerHudInit( int playerid );
	int GetClientPtr(int edictptr);
	string GetUserInfoStringValue(const char *key, int edictptr);
	void CheckTimeOut(int playerid);
	int DisconnectClient(int playerid, const char* szReason = "");
	int GetFlags(int playerid); //This is supposed to get a player's flags
	bool IsValidPlayerID(int playerid) { return playerid == INVALID_USERID ? false : true; }
	//definitions
	//static int cptr;
	static int edictptr;
	//static int ptr;
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
    amx_StrParam(amx, params[2], wepname);
    pPlayer->GiveNamedItem( (int)params[1], wepname, (int)params[3] );
	delete pPlayer;
	return true;
}
static cell AMX_NATIVE_CALL n_GetClientName(AMX *amx, const cell *params) {
	CBasePlayer *pPlayer = new CBasePlayer;
	cell* addr = NULL;

    //Get the address of our string parameter (str) and store our message 
    amx_GetAddr(amx, params[2], &addr);
    amx_SetString(addr, pPlayer->GetClientName( (int)params[1] ).c_str(), true, false, params[3]);
	delete pPlayer;
	return true;
}
static cell AMX_NATIVE_CALL n_GetPlayerPos(AMX *amx, const cell *params) {
	//This function passes XYZ by reference
	CBasePlayer *pPlayer = new CBasePlayer;
	float X, Y, Z;
	pPlayer->GetPlayerPos((int)params[1], X, Y, Z);
	cell* dest[3] = {NULL, NULL, NULL};
	for(int i=0; i<3; i++) {
		amx_GetAddr(amx, params[i+2], &dest[i]);
	}
	*dest[0] = amx_ftoc(X);
	*dest[1] = amx_ftoc(Y);
	*dest[2] = amx_ftoc(Z);
	delete pPlayer;
	return true;
}
static cell AMX_NATIVE_CALL n_SetPlayerPos(AMX *amx, const cell *params) {
	CBasePlayer *pPlayer = new CBasePlayer;
	pPlayer->SetPlayerPos((int)params[1], amx_ctof(params[2]), amx_ctof(params[3]), amx_ctof(params[4]));
	delete pPlayer;
	return true;
}
static cell AMX_NATIVE_CALL n_KickClient(AMX *amx, const cell *params) {
	CBasePlayer *pPlayer = new CBasePlayer;
	char *ret = NULL;
    amx_StrParam(amx, params[2], ret);
	pPlayer->DisconnectClient((int)params[1], ret);
	delete pPlayer;
	return true;
}
static cell AMX_NATIVE_CALL n_GetPlayerEdictPtr(AMX *amx, const cell *params) {
	CBasePlayer *pPlayer = new CBasePlayer;
	int edictptr = pPlayer->GetEdictNum(params[1]);
	delete pPlayer;
	return edictptr;
}
#endif

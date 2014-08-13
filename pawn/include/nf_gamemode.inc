#pragma tabsize 4
// Ignores warning 217 for properly indented PAWNO code
// It's tab size is 4 and often uses 4 spaces instead, PAWNCC's is 8

#include <core>
#include <string>
#include <float>
#include <file>
#include <time>
#include <datagram>
#include <nf_players>
// Limits
#define MAX_PLAYER_NAME							(24)
#define MAX_PLAYERS								(32)

//GUI TriggerHudMsg
#define MSG_BROADCAST 							(0) // unreliable message, sent to all
#define MSG_ONE 								(1) // reliable message, sent to msg_entity
#define MSG_ALL 								(2) // reliable message, sent to all
#define MSG_INIT 								(3) // write to the init string

// --------------------------------------------------
// Forwards
// --------------------------------------------------
//Gamemode related
forward OnServerChangeMap();
forward OnClientConnect(playerid);
forward OnClientDisconnect(playerid, reason);
forward OnGameModeInit();
forward OnGameModeExit();
forward ServerThink();
forward OnClientUpdate(playerid);
forward OnClientCommandText(playerid, cmdname[]);
forward OnClientSpawn(playerid);
forward OnClientDeath(attacker, receiver);
// --------------------------------------------------
// Native functions
// --------------------------------------------------
//Gamemode related
native GetGameTimeLeft();
//GUI Related
native TriggerHudMsg(playerid, text[], duration, towho);
//Spectate
native SpectatePlayer(playerid, tospectate);
native RemoveAllItems(playerid, dukes);
native GiveNamedItem(playerid, wepname[], iSubType);
native GetClientName(playerid, const dest[], len);
native GetPlayerPos(playerid, &Float:x, &Float:y, &Float:z);
native SetPlayerPos(playerid, Float:x, Float:y, Float:z);
//Utils
//native print(const string[]);
//native printf(const format[], {Float,_}:...);
native format(output[], len, const format[], {Float,_}:...);
//Precache Related
native EmitAmbientSound(flags, Float:volume, charsample[], channel, edict);
native EmitSound(edict, channel, charsample[], Float:volume, Float:attenuation, fFlags, pitch);
native PrecacheSound(name[]);
native PrecacheModel(name[]);
native PrecacheGeneric(name[]);
native PrecacheEvent(name[]);
native PrecacheUnmodified(name[]);
native FindEntityByClassname(pStartEntity, szName[]);
native ConsoleCommand(text[]);
native EDICT_NUM(entity);

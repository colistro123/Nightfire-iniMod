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
forward OnMapLoaded();
forward OnClientConnect(playerid);
forward OnClientDisconnect(playerid, reason);
forward OnGameModeInit();
forward OnGameModeExit();
forward ServerThink();
forward OnClientUpdate(playerid);
forward OnClientCommandText(playerid, cmdname[]);
forward OnClientSpawn(playerid);
forward OnClientDeath(attacker, receiver);
forward OnClientEquip(playerid);
// --------------------------------------------------
// Native functions
// --------------------------------------------------
//Gamemode related
native GetGameTimeLeft();
native GetPointDistance(Float:px, Float:py, Float:pz, Float:X, Float:Y, Float:Z);
//GUI Related
native TriggerHudMsg(playerid, text[], duration, towho);
native CenterPrint(playerid, text[], towho);
native SetView(playerid, toplayerid);
native ClientPrint(playerid, dest, message[]);
native UTIL_ScreenFade(playerid, r, g, b, a, Float:fadeTime, Float:fadeHold, flags);
native UTIL_SayTextFilter(senderid, pText[], receiver, bChat);
native UTIL_ShowMessage(playerid, pString[]);
native UTIL_SetVisionColor(playerid, color); //Set the vision color 0-4
native UTIL_Create_TE_DLIGHT(entity, Float:x, Float:y, Float:z, Float:exponent, Float:radius, red, green, blue, life, Float:decayRate);
//Beams
native UTIL_CreateBeamEntsPlayers(playerid, toplayerid, spriteindex, startframe, frame, life, width, noise, r, g, b, bright, scroll, msgtype);
//Spectate
native SpectatePlayer(playerid, tospectate);
native RemoveAllItems(playerid, dukes);
native GiveNamedItem(playerid, wepname[], iSubType);
native GetClientName(playerid, const dest[], len);
native GetPlayerPos(playerid, &Float:x, &Float:y, &Float:z);
native SetPlayerPos(playerid, Float:x, Float:y, Float:z);
native GetPlayerEdictPtr(playerid);
native Sys_FloatTime();
native KickClient(playerid, reason[]);
//Utils
//native print(const string[]);
//native printf(const format[], {Float,_}:...);
//native format(output[], len, const format[], {Float,_}:...);
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
native SUB_Remove(entity);
native SetQueryVar(var1[], vardesc[]);
//Plugins
forward OnPluginInit();
forward OnPluginExit();

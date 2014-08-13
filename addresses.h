#ifndef ADDRESSES_H
#define ADDRESSES_H
//#include "stdafx.h"
/*
	Just put addresses here
*/
/* Addresses */
//Server related

//#define SERVER_FPS_ADDR			(extern DWORD)0x448BE1C0
#define SERVER_FPS_ADDR				0x448BE1C0
#define CONSOLEMSGADDRESS			0x44B7AF08
#define MSGCALLADDRESS				0x43094920
#define IS_GAMEMODE_CTF				0x42046453
#define HUDMSGADDRESS				0x4201108D
#define CONSOLECOMMAND				0x43045B00
//#define HUDOBJECTIVE				0x42011C46 //this is wrong
#define SV_MAXCLIENTS				0x44A86B50
#define SV_NUMCLIENTS				0x44B82E6C
#define SV_CURRENTMAP				0x44A2A3B8
#define SCR_SHOW_MSG				0x4205A720
//precache
#define ADR_PrecacheSound			0x4306ced0
#define ADR_PrecacheEvent			0x4306cfd0
#define ADR_PrecacheModel			0x4306d680
#define ADR_PrecacheGeneric			0x4306d740
#define ADR_PrecacheUnmodified		0x4306f1c0 //ForceUnmodified?
//cvars
#define SV_SERVERLOG				0x44A86B60
#define MP_LOGECHO					0x448BE6EC
#define CONDEBUG					0x4348AE9C
#define MP_FRAGLIMIT				0x449CA21C
#define MP_TIMELEFT					0x4217A750 //game time left in seconds in multiplayer, unreliable
#define MAP_RUNTIME_SECS			0x4217A168 //time the map has been running in seconds | NOTE: not a cvar, is float
#define MP_TIMELIMIT				0x449C9E2C
//Weapons
#define P90_RELOAD_EMPTY_SPEED		0x420E2B95
#define P90_RELOAD_SPEED			0x420E2BBC
#define ROCKET_PUSHBACK_AMOUNT		0x420E7975
#define GET_PLAYER_PLAYMODE			0x420AAFA2 //Multiplayer or single player? Currently useless I believe since this is client sided
//Others
#define NUM_FOR_EDICT				0x4306fa40
#define CLIENT_COMMAND				0x4306C830 //Stuffcmd
#define ADR_EMITSOUND				0x420C35B0 //0x4306BC30 direct function //emits a sound
#define ADR_UTIL_EMITAMBIENTSOUND	0x4205A020 //0x4306BC30 direct function //emits a sound
#define ADR_EMITAMBIENTSOUND		0x42002620 //emits a sound everywhere
#define ADR_MESSAGE_BEGIN			0x4306E6B0
#define ADR_MESSAGE_END				0x4306E790
#define ADR_WRITE_BYTE				0x4306E9D0
#define ADR_WRITE_CHAR				0x4306EA00
#define ADR_WRITE_SHORT				0x4306EA30
#define ADR_WRITE_LONG				0x4306EA60
#define ADR_WRITE_ANGLE				0x4306EA90
#define ADR_WRITE_COORD				0x4306EAC0
#define ADR_WRITE_STRING			0x4306EAF0
#define ADR_WRITE_ENTITY			0x4306EB20
#define ADR_PF_FadeVolume_I			0x4306EDD0
#define ADR_SetClientMaxspeed		0x4306EE60
#define ADR_GetPhysicsInfoString	0x4306efc0
#define ADR_GetPhysicsKeyValue		0x4306f010
#define ADR_SetPhysicsKeyValue		0x4306f060
#define ADR_PF_CanSkipPlayer		0x4306f100
#define ADR_SetView					0x4306e210
#define ADR_GetPlayerStats			0x4306f2b0
#define ADR_INVALID_CMD				0x43043A74
#define ADR_TRIGGER_EVT				0x420C8053 //0x420C8053 //Death, Spawn, Leave, Connect, Etc
#define ADR_CONNECT_FN				0x4308BA29 //0x4203F9B0 OLD Address for the Connect Function
#define ADR_ADDBOT_FN				0x4202492F
#define ADR_DISCONNECT_FN			0x42047E9D //Disconnect Function
#define ADR_CHANGEMAP_FN			0x420D35DB //OnServerChangeMap
#define ADR_FindEntityByClassname	0x42059D20 //call directly
#define ADR_HOSTSAY					0x4203E2CC
#define ADR_PLAYERKILLED_FN			0x420815CE //0x420815CE <- OLD ADDRESS, WORKS JUST FINE
#define ADR_PLAYERSUICIDED_FN		0x42081306
#define ADR_GIVENAMEDITEM			0x420A3060
#define ADR_WEAPONSTRIP				0x420A6B40
#define ADR_WEAPON_SETLAST			0x420A3980		
#define ADR_SETMODEL				0x4306B7F0
//These are not addresses
#define MSG_BROADCAST 0 // unreliable message, sent to all
#define MSG_ONE 1		// reliable message, sent to msg_entity
#define MSG_ALL 2       // reliable message, sent to all
#define MSG_INIT 3		// write to the init string

//String related
#define MAX_PLAYER_NAME 24
#define MAX_STRING_LEN 512

//Net related
#define MAX_BEAT_INTERVAL 2 //The maximum amount of time a player can lose packets for

//Debug Mode
#define INIMOD_DEBUG 1

//Precache related
#define TYPE_SND 1
#define TYPE_MDL 2
#define TYPE_GENERIC 3
#define TYPE_EVENT 4
#define TYPE_UNMODIFIED 5

//-----------------------------------------------------------------------------
// Sound channels
//-----------------------------------------------------------------------------

#define	CHAN_REPLACE	-1
#define	CHAN_AUTO		0
#define	CHAN_WEAPON		1
#define	CHAN_VOICE		2
#define	CHAN_ITEM		3
#define	CHAN_BODY		4
#define	CHAN_STREAM		5		// allocate stream channel from the static or dynamic area
#define	CHAN_STATIC		6		// allocate channel from the static area 
#define	CHAN_VOICE_BASE	7		// allocate channel for network voice data
#define	CHAN_USER_BASE	= (CHAN_VOICE_BASE+128)		// Anything >= this number is allocated to game code.

#endif

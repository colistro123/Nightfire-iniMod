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
#define ADR_ClientPrint				0x4205A720
#define ADR_CLIENT_PRINTF			0x4306B970
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
#define ADR_GETINFOKEYBUFFER		0x4306D860
#define ADR_INFOKEYVALUE			0x4305FB10
#define ADR_CLIENTPRINTF			0x42058F50 //prints to client's console
#define ADR_LOAD_FILE_FOR_ME		0x43042F00
#define ADR_FREE_FILE				0x43040E00
#define ADR_CVAR_GET_X				0x43045AC0
#define ADR_ALLOCSTRING				0x4306FD40
#define FNULLENT					0x4306FC30
#define GET_BASE_ENTITY				0x4306FC30
#define INDEX_OF_EDICT				0x4306fc40
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
#define ADR_INIT_HUD				0x42081F32 //After player spawns for the first time
#define ADR_CONNECT_FN				0x4308BA29 //0x4203F9B0 OLD Address for the Connect Function
#define ADR_ADDBOT_FN				0x4202492F
#define ADR_DISCONNECT_FN			0x42047E9D //Disconnect Function
#define ADR_CHANGEMAP_FN			0x420D35DB //OnServerChangeMap
#define ADR_CreateEntity			0x42032EA0
#define ADR_FindEntityByX			0x4306F920
#define ADR_FindEntityByClassname	0x42059D20 //call directly
#define ADR_HOSTSAY					0x4203E2CC
#define ADR_PLAYERKILLED_FN			0x420815CE //0x420815CE <- OLD ADDRESS, WORKS JUST FINE
#define ADR_PLAYERSUICIDED_FN		0x42081306
#define ADR_GIVENAMEDITEM			0x420A3060
#define ADR_WEAPONSTRIP				0x420A6B40
#define ADR_WEAPON_SETLAST			0x420A3980		
#define ADR_SETMODEL				0x4306B7F0
#define ADR_PLAYERSPAWN				0x420469C7
#define ADR_PLAYEREQUIP				0x420469FD
#define ADR_SUBREMOVE				0x420C7EE0
#define ADR_MAPLOADED				0x430878C5 //0x4305F00C
#define ADR_SETQUERYVAR				0x430BD020
#define ADR_MASTERLISTDD			0x448BE1C8 //(Needed for sending query vars)
#define ADR_USERMESSAGEBEGIN		0x43088050
#define ADR_DROPCLIENT_FN			0x430574A0		
#define ADR_TIMEOUT_HOOK			0x43085E41
#define ADR_FIRE_BULLETS			0x42042DE0
#define ADR_UTIL_TRACELINE			0x4306F510
#define ADR_GETUSERID				0x4306EEB0
#define ADR_FAKECLIENTMAPCHANGE		0x43088B5A
#define ADR_DROPCLIENT_HOOK			0x430574FA
#define ADR_UTIL_REMOVE				0x42059350
#define ADR_RECONNECT_FN			0x4308B5F9
//#define ADR_FIRE_BULLETS_HOOK		0x42042DEE
//These are not addresses
#define MSG_BROADCAST	0	// unreliable message, sent to all
#define MSG_ONE			1	// reliable message, sent to msg_entity
#define MSG_ALL			2   // reliable message, sent to all
#define MSG_INIT		3	// write to the init string
#define MSG_PVS			4	
#define MSG_PAS			5	//Ents in PAS of origin

//String related
#define MAX_PLAYER_NAME		24
#define MAX_STRING_LEN		512
#define MAX_MOTD_CHUNK		60
#define MAX_MOTD_LENGTH		(MAX_MOTD_CHUNK * 4)

//Net related
#define MAX_BEAT_INTERVAL 15//was 2 //The maximum amount of time a player can lose packets for

//We remove them from the table in 15 seconds to prevent overload
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


//SVC / User Messages

#define	SVC_BAD						0
#define SVC_NOP						1
#define SVC_DISCONNECT				2
#define SVC_EVENT					3
#define SVC_VERSION					4
#define SVC_SETVIEW					5
#define SVC_SOUND					6
#define SVC_TIME					7
#define SVC_PRINT					8
#define SVC_STUFFTEXT				9
#define SVC_SETANGLE				10
#define SVC_SERVERINFO				11
#define SVC_LIGHTSTYLE				12
#define SVC_UPDATEUSERINFO			13
#define SVC_DELTADESCRIPTION		14
#define SVC_CLIENTDATA				15
#define SVC_STOPSOUND				16
#define SVC_PINGS					17
#define SVC_PARTICLE				18
#define SVC_DAMAGE					19
#define SVC_SPAWNSTATIC				20
#define SVC_EVENT_RELIABLE			21
#define SVC_SPAWNBASELINE			22
#define SVC_TEMPENTITY				23
#define SVC_SETPAUSE				24
#define SVC_SIGNONNUM				25
#define SVC_CENTERPRINT				26
#define SVC_KILLEDMONSTER			27
#define SVC_FOUNDSECRET				28
#define SVC_SPAWNSTATICSOUND		29
#define SVC_INTERMISSION			30
#define SVC_FINALE					31
#define SVC_CDTRACK					32
#define SVC_RESTORE					33
#define SVC_CUTSCENE				34
#define SVC_WEAPONANIM				35
#define SVC_DECALNAME				36
#define SVC_ROOMTYPE				37
#define SVC_ADDANGLE				38
#define SVC_NEWUSERMSG				39
#define SVC_PACKETENTITIES			40
#define SVC_DELTAPACKETENTITIES		41
#define SVC_CHOKE					42
#define SVC_RESOURCELIST			43
#define SVC_NEWMOVEVARS				44
#define SVC_RESOURCEREQUEST			45
#define SVC_CUSTOMIZATION			46
#define SVC_CROSSHAIRANGLE			47
#define SVC_SOUNDFADE				48
#define SVC_FILETXFERFAILED			49
#define SVC_HLTV					50
#define SVC_DIRECTOR				51
#define SVC_VOICEINIT				52
#define SVC_VOICEDATA				53
#define SVC_SENDEXTRAINFO			54
#define SVC_TIMESCALE				55
#define SVC_RESOURCELOCATION		56
#define SVC_SENDCVARVALUE			57
#define SVC_SENDCVARVALUE2			58

//Fading
#define FFADE_IN					0x0000 // Just here so we don't pass 0 into the function
#define FFADE_OUT					0x0001 // Fade out (not in)
#define FFADE_MODULATE				0x0002 // Modulate (don't blend)
#define FFADE_STAYOUT				0x0004 // ignores the duration, stays faded out until new ScreenFade message received

//Hud messages
#define HUD_PRINTNOTIFY				1
#define HUD_PRINTCONSOLE			2
#define HUD_PRINTTALK				3		//Doesn't work
#define HUD_PRINTCENTER				4

#define print_console 				2 //for ClientPrint
#define print_center 				1
#define print_chat 					0
#define client_printf_console 		2 //for CLIENT_PRINTF

#define SCREENFADE_FRACBITS			9		// which leaves 16-this for the integer part

//Temp entities
//Tracers
#define TE_TRACER					6			// tracer effect from point to point
#define TE_BEAMPOINTS				0

#define TE_TEXTMESSAGE				29

#define TE_EXPLFLAG_NONE			0	// all flags clear makes default Half-Life explosion
#define TE_EXPLFLAG_NOADDITIVE		1	// sprite will be drawn opaque (ensure that the sprite you send is a non-additive sprite)
#define TE_EXPLFLAG_NODLIGHTS		2	// do not render dynamic lights
#define TE_EXPLFLAG_NOSOUND			4	// do not play client explosion sound
#define TE_EXPLFLAG_NOPARTICLES		8	// do not draw particles

#endif

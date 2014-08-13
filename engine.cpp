/*
To do:
Add GivePlayerWeapon?
Move player related functions to a player.cpp file? - I can do this one

*/

#include <malloc.h>				// malloc, etc
#include <errno.h>				// errno, etc
#include "engine.h"
#include "player.h"
#include <extdll.h>				// always
#include "utils.h"
#include "gamemodes.h"

#pragma comment(lib, "winmm.lib") //Pragma for the time unresolved external symbols

HMODULE g_Module;
DLL_FUNCTIONS g_dllfuncs;
enginefuncs_t g_engfuncs;


double	engineModule::curtime = 0;
double	engineModule::lasttick = 0;


char* engineModule::stringToChar(std::string mystring) {
	char* output = (char *)mystring.c_str();
	return output;
}
void engineModule::mainRoutineCall( void ) {
	ShowConsoleMessage("Message from iniMod: IniMod initialized.\n");
}
DLL_EXP void SUPERTOOLS_MAIN( void ) {
	engineModule *engine = new engineModule;
	if(engine) {// So we don't delete an invalid pointer
		engine->curtime = timeGetTime();
		if ( engine->curtime - engine->lasttick > 100.0f ){ //sanity check
			engine->lasttick = engine->curtime;
			engine->ServerThink();
		}
		delete engine; //This will call its deconstructor to clear the pointer and prevent memory leaks.
	}
}
void engineModule::ServerThink( void ) { //This is where everything gets called, MAIN LOOP/MAINLOOP
	CBasePlayer *pPlayer = new CBasePlayer;
	CNetwork *cNetwork = new CNetwork;
	//pPlayer->UpdateClients(); Not Needed anymore
	cNetwork->parseConfigFile(); //This only gets called once
	cNetwork->OnServerThink(); //OnServerThink
	//pPlayer->ShowConnectedClients(); //Show all the connected clients
	delete pPlayer;
	delete cNetwork;
}
void engineModule::OnCommandReceived() { //We can use this to check if the users entered a command and handle our commands from here..
	//Enter code here..
}
void engineModule::ConsoleCommand(char *text) {
	printf("Executing Console Command: %s\n", text);
	DWORD adr = CONSOLECOMMAND;
	__asm push 1
	__asm push text
	__asm call adr
	__asm add esp, 8
}
/*
string engineModule::getGameVer(int playerid) {
	Utils *utils = new Utils();
	string getVersion = FetchInfoForPlayerID(playerid);
	if(IsAClient(playerid)) {
		string version = utils->ExplodeAndReturn(getVersion, 7);
		return version;
	}
	return 0;
}
*/
void engineModule::ShowConsoleMessage(char *text) {
	DWORD adr = MSGCALLADDRESS;
	__asm {
    push text // text
	call  adr // call the func for show dialogg
	add esp, 4
	}
}
void engineModule::KickOldClient(void ){
	/*
	char *Message = "PLEASE UPDATE YOUR GAME!";
	DWORD func = 0;//0x;
	__asm {
		push Message
		call 1234567
	}
	*/
}
void engineModule::SHOW_TO_CONSOLE_AND_EXIT( char* msg ) {
	ShowConsoleMessage(msg);
	Sleep(5000);
	exit(1);
}
void engineModule::ClearConsoleScreen( void ) {
	HANDLE hndl = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hndl, &csbi);
	DWORD written;
	DWORD n = csbi.dwSize.X * csbi.dwCursorPosition.Y + csbi.dwCursorPosition.X + 1;	
	COORD curhome = {0,0};
	FillConsoleOutputCharacter(hndl, ' ', n, curhome, &written);
	csbi.srWindow.Bottom -= csbi.srWindow.Top;
	csbi.srWindow.Top = 0;
	SetConsoleWindowInfo(hndl, TRUE, &csbi.srWindow);
	SetConsoleCursorPosition(hndl, curhome);
}
void engineModule::Precache(int type, char* name ) {
	DWORD adr;
	printf("Precaching %s\n", name);
	switch(type) {
		case TYPE_MDL: {
			adr = ADR_PrecacheModel;
			__asm push name
			__asm call adr
			__asm add esp, 0x4
			break;
		}
		case TYPE_SND: {
			adr = ADR_PrecacheSound;
			__asm push name
			__asm call adr
			__asm add esp, 0x4
			break;
		}
		case TYPE_GENERIC: {
			adr = ADR_PrecacheGeneric;
			__asm push name	
			__asm call adr
			__asm add esp, 0x4
			break;
		}
		case TYPE_EVENT: {
			adr = ADR_PrecacheEvent;
			__asm push name
			__asm call adr
			__asm add esp, 0x4
			break;
		}
		case TYPE_UNMODIFIED: {
			adr = ADR_PrecacheUnmodified;
			__asm push name
			__asm call adr
			__asm add esp, 0x4
			break;
		}
	}
}
int engineModule::EDICT_NUM(int entitynumber){ //FindEdictPointer
	int edictptr = ReadInt32(entitynumber + 4);
	if ( edictptr != 0 )
		edictptr = ReadInt32(edictptr + 0x204);
	return edictptr;
}
int engineModule::EDICT_NUM_TO_ENTNUM(DWORD edictptr){ //FindEntityNumber
	int edictnum = 0;
	if ( edictptr != 0 )
		edictnum = ReadInt32(edictptr - 0x204);
	return edictnum;
}
void engineModule::EmitAmbientSound(int fFlags, float volume, const char *sample, int channel, int entity) {
	DWORD adr;
	adr = ADR_EMITAMBIENTSOUND;
	__asm push fFlags
	__asm push volume
	__asm push sample
	__asm push channel
	__asm push entity
	__asm call adr
	__asm add esp, 0x14
}
void engineModule::EmitSound(int entity, int channel, const char *sample, /*int*/float volume, float attenuation, int fFlags, int pitch) {
	DWORD adr;
	adr = ADR_EMITSOUND;
	
	//int testedict = ReadInt32(entity + client_t::pedicts);

	//push 64, push 0, PUSH 3F4CCCCD [0.800000011920929], PUSH 3F800000 [1], PUSH 4210E174 [sample], PUSH 1, PUSH ESI
	//DWORD adrofsound = DWORD(sample);
	/*
	__asm push entity
	__asm push channel
	__asm push sample
	__asm push volume
	__asm push attenuation
	__asm push fFlags
	__asm push pitch
	*/
	__asm push pitch
	__asm push 6
	__asm push attenuation
	__asm push volume
	__asm push sample
	__asm push fFlags
	__asm push entity
	__asm call adr
	__asm add esp, 0x1c
}
int engineModule::FindEntityByClassname( int pStartEntity, const char *szName ) {
	int edictptr = 0;
	DWORD adr;
	adr = ADR_FindEntityByClassname;
	__asm push szName
	__asm push pStartEntity
	__asm call adr
	__asm mov edictptr, eax
	__asm add esp, 0x8
	return edictptr;
}
int engineModule::FindRandomEntityPointer() {
	int entityptr = FindEntityByClassname(0, "info_player_start");
	if ( entityptr == 0 ) { entityptr = FindEntityByClassname(0, "game_player_equip"); }
    if ( entityptr == 0 ) { entityptr = FindEntityByClassname(0, "func_wall"); }
    if ( entityptr == 0 ) { entityptr = FindEntityByClassname(0, "func_breakable"); }
	if ( entityptr == 0 ) { entityptr = FindEntityByClassname(0, "func_button"); }
	if ( entityptr == 0 ) { entityptr = FindEntityByClassname(0, "item_breakable"); }
	if ( entityptr == 0 ) { entityptr = FindEntityByClassname(0, "item_grappletarget"); }
	if ( entityptr == 0 ) { entityptr = FindEntityByClassname(0, "env_sprite"); }
	if ( entityptr == 0 ) { entityptr = FindEntityByClassname(0, "trigger_multiple"); }
	if ( entityptr == 0 ) { entityptr = FindEntityByClassname(0, "trigger_once"); }
	if ( entityptr == 0 ) { entityptr = FindEntityByClassname(0, "func_ladder"); }
	if ( entityptr == 0 ) { entityptr = FindEntityByClassname(0, "weapon_dukes"); }
	//new string[128];
	//strformat(string, sizeof(string), false, "Entity Ptr: %d", entityptr);
    //print(string);
	return entityptr;
}
int engineModule::GetGameTimeLeft() {
	int timeleft = (ReadInt(MP_TIMELIMIT) * 60) - ReadFloat(ReadInt32(MAP_RUNTIME_SECS));
	return timeleft; //returns time left in seconds
}

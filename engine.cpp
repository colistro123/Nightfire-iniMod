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
#include "gettime.h"
#include "plugins.h"

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
	engine->curtime = Sys_FloatTime() * 1000.0f;
	if ( engine->curtime - engine->lasttick >= 10.0f ){ //100 fps max
		engine->lasttick = engine->curtime;
		engine->ServerThink();
	}
	delete engine; //This will call its deconstructor to clear the pointer and prevent memory leaks.
}
void engineModule::ServerThink( void ) { //This is where everything gets called, MAIN LOOP/MAINLOOP
	CBasePlayer *pPlayer = new CBasePlayer;
	CNetwork *cNetwork = new CNetwork;
	CPlugins *plugin = new CPlugins;
	//pPlayer->UpdateClients();
	cNetwork->parseConfigFile(); //This only gets called once
	cNetwork->OnServerThink(); //OnServerThink
	cNetwork->UpdatePlayers();
	plugin->OnServerThink();
	plugin->UpdatePlayers();
	//pPlayer->ShowConnectedClients(); //Show all the connected clients
	delete pPlayer;
	delete cNetwork;
	delete plugin;
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
void engineModule::ShowConsoleMessage(char *text) {
	DWORD adr = MSGCALLADDRESS;
	__asm {
    push text // text
	call  adr // call the func for show dialogg
	add esp, 4
	}
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
int engineModule::Precache(int type, char* name ) {
	DWORD adr;
	int index = 0;
	printf("Precaching %s\n", name);
	switch(type) {
		case TYPE_MDL: {
			adr = ADR_PrecacheModel;
			__asm push name
			__asm call adr
			__asm add esp, 0x4
			__asm mov index, eax
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
	return index;
}
int engineModule::ALLOC_STRING(const char *szStr){ //allocates a string in the engine and returns its pointer
	int ptr1 = ADR_ALLOCSTRING;
	__asm {
		push szStr
		call ptr1
		mov ptr1, eax
		add esp,0x4
	}
	return ptr1;
}
int engineModule::IndexOfEdict(int edictptr){
	int index = INDEX_OF_EDICT;
	__asm{
		push edictptr
		call index
		mov index, eax
		add esp, 0x4
	}
	return index;
}
int engineModule::EDICT_NUM(int entitynumber){ //FindEdictPointer
	int edictptr = ReadInt32(entitynumber + 4);
	if ( edictptr != 0 )
		edictptr = ReadInt32(edictptr + 0x204);
	return edictptr;
}
int engineModule::NUM_FOR_EDICT_FN(int edictptr){ //Get the Number of this edict
	DWORD NUMFOREDICT = NUM_FOR_EDICT;
	__asm push edictptr //edictptr
	__asm call NUMFOREDICT //get number for this edict
	__asm mov edictptr, eax
	__asm add esp, 0x4
	return edictptr;
}
int engineModule::EDICT_NUM_TO_ENTNUM(DWORD edictptr){ //FindEntityNumber
	int edictnum = 0;
	if ( edictptr != 0 )
		edictnum = ReadInt32(edictptr - 0x204);
	return edictnum;
}
void engineModule::SetQueryVar(const char *param1, const char *param2) { //Var and description for that var
	DWORD adr;
	adr = ADR_SETQUERYVAR;
	//adr2 = ADR_MASTERLISTDD;
	DWORD masterlistdd = ReadInt32(ADR_MASTERLISTDD);
	__asm push param2
	__asm push param1
	__asm push 1
	__asm mov ecx, masterlistdd
	__asm call adr
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
void engineModule::SetModel(const char *szName, int edictptr){
	DWORD adr = ADR_SETMODEL;
	__asm{
 		push szName
		push edictptr
		call adr
		add esp, 0x8
	}
}
int engineModule::GetGameTimeLeft() {
	int timeleft = (ReadInt(MP_TIMELIMIT) * 60) - ReadFloat(ReadInt32(MAP_RUNTIME_SECS));
	return timeleft; //returns time left in seconds
}
void engineModule::UTIL_TraceLine(DWORD edictptr, const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, int collisionGroup) {
	DWORD adr = ADR_UTIL_TRACELINE;
	Vector tempeax;
	Vector tempecx;
	Vector tempedx;
	__asm {
		/*
		push 0x18eb28
		push edictptr
		push 0
		push mask
		push vecAbsEnd
		push vecAbsStart
		*/
		lea ecx, dword ptr ds:[0x18eab0+0x78]
		push 0x18eb28
		push edictptr
		push 0
		push 0
		mov tempedx, edx;
		mov tempeax, eax
		lea edx, dword ptr ds:[0x18eaa0+0x58]
		push edx
		lea eax, dword ptr ds:[0x18ea9c+0x14c]
		push eax
		call adr
		add esp, 0x18
		mov tempeax, esi
		mov tempecx, ebp
		mov tempedx, edi
	}
	printf("%d, %d, %d\n", tempeax, tempecx, tempedx);
	printf("%f, %f, %f, %f, %f, %f, %f, %f, %f", 
		Vector(tempeax).x, Vector(tempeax).y, Vector(tempeax).z,
		Vector(tempecx).x, Vector(tempecx).y, Vector(tempecx).z,
		Vector(tempedx).x, Vector(tempedx).y, Vector(tempedx).z);

}


/*
void engineModule::UTIL_TraceLine(DWORD edictptr, const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, int collisionGroup) {
	DWORD adr = ADR_UTIL_TRACELINE;
	DWORD tempeax;
	DWORD tempecx;
	DWORD tempedx;
	__asm {
		push collisionGroup
		push edictptr
		push 0
		push mask
		push vecAbsEnd
		push vecAbsStart
		call adr
		add esp, 0x18
		mov tempeax, eax
		mov tempecx, ecx
		mov tempedx, edx
	}
	printf("%d, %d, %d\n", tempeax, tempecx, tempedx);
}
*/
byte * engineModule::LOAD_FILE_FOR_ME(const char *filename, int *pLength){
	int temp = ADR_LOAD_FILE_FOR_ME;
	__asm{
		push pLength
		push filename
		call temp
		mov temp, eax
		add esp,0x8
	}
	return (byte*)temp;
}
void engineModule::FREE_FILE (byte *buffer){
	int temp = ADR_FREE_FILE;
	__asm{
		push buffer
		call temp
		add esp,0x4
	}
}
char* engineModule::CVAR_GET_STRING( const char *x ){
	int temp = ADR_CVAR_GET_X;
	char stringarray[256] = {0};
	__asm{
		push x
		call temp
		add esp, 0x4
		mov temp, eax
		test eax, eax
		jz null
			mov edx, dword ptr ds:[eax]
			mov ecx, eax
			call dword ptr ds:[edx+0x50] //getchar
			mov temp, eax
			add esp,0x5
	}
	for(int i = 0; memGetByte(temp) != 0x0; i++) {
		if(stringarray[i] == '\0') {
			stringarray[i] = memGetByte(temp);
			temp++;
		}
	}
	__asm sub esp, 0x5
	null:
	return (char*)stringarray;
}
void engineModule::ClientPrint(const char *message, int msgtype, int client){
	int adr = ADR_ClientPrint;
	__asm{
		push message
		push msgtype
		push client
		call adr
		add esp,0xC
	}
}
void engineModule::CLIENT_PRINTF(const char *message, int msgtype, int edictptr){
	int adr = ADR_CLIENT_PRINTF;
	__asm{
		push message
		push msgtype
		push edictptr
		call adr
		add esp,0xC
	}
}
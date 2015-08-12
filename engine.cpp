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
#include "common.h"

#pragma comment(lib, "winmm.lib") //Pragma for the time unresolved external symbols

HMODULE g_Module;
DLL_FUNCTIONS g_dllfuncs;
enginefuncs_t g_engfuncs;

CommandVariables cmdvariablesinfo[200];
unsigned int nextcmdvariablesinfo = 0;

ServerCommands servercommandsinfo[200];
unsigned int nextservercommandsinfo = 0;

ServerCommandFuncPointers servercommandfuncpointersinfo[200];
unsigned int nextservercommandfuncpointersinfo = 0;

double	engineModule::realtime = 0;
double	engineModule::curtime = 0;
double	engineModule::lasttime = 0;
float engineModule::host_timescale = 1.0f;
float engineModule::interval_per_tick = 0.01f;
float engineModule::DeltaFrameTime = 0.0f;
float engineModule::host_frametime = 0.0f;
float engineModule::host_remainder = 0.0f;
#ifdef USE_SOURCE2013_TICKRATE_METHOD
	int engineModule::last_fpsmax = 0;
	float engineModule::newtickinterval;
	float engineModule::one = 1.0f;
	float engineModule::fivetwelve = 512.0f;
	float engineModule::zeropointfive = 0.5f;
	float engineModule::zero = 0.0f;
	float engineModule::twopower23 = 8388608.0f;
	float engineModule::oneslash512 = 0.001953125f; //1/512
	float engineModule::min_tick_interval = 0.001953125f; //0.00390625f; //1/256 is the source 2013 max tickrate. our cap is 512
	float engineModule::max_tick_interval = 0.04882812f; //1/20.48
#endif
char* engineModule::stringToChar(std::string mystring) {
	char* output = (char *)mystring.c_str();
	return output;
}
void engineModule::mainRoutineCall( void ) {
	int  (engineModule::*adrofCmd_Argc) (void) = &engineModule::Cmd_Argc;
	char  *(engineModule::*adrofCmd_Argv) (int) = &engineModule::Cmd_Argv;
	char  *(engineModule::*adrofCmd_Args) (void) = &engineModule::Cmd_Args;
	void (engineModule::*adrofCmd_TokenizeString) (char *) = &engineModule::Cmd_TokenizeString;
	void (engineModule::*adrofCmd_ExecuteString) (char *) = &engineModule::Cmd_ExecuteString;
	void (engineModule::*adrofCvar_RegisterVariable) (cvar_reg) = &engineModule::Cvar_RegisterVariable;
	float (engineModule::*adrofCvar_GetFloat) (const char*) = &engineModule::Cvar_GetFloat;
	int (engineModule::*adrofCvar_GetInt) (const char*) = &engineModule::Cvar_GetInt;
	bool (engineModule::*adrofCvar_GetBool) (const char*) = &engineModule::Cvar_GetBool;
	const char* (engineModule::*adrofCvar_GetString) (const char*) = &engineModule::Cvar_GetString;
	void (engineModule::*adrofCvar_SetFloat) (const char*, float) = &engineModule::Cvar_SetFloat;
	void (engineModule::*adrofCvar_SetInt) (const char*, int) = &engineModule::Cvar_SetInt;
	void (engineModule::*adrofCvar_SetBool) (const char*, bool) = &engineModule::Cvar_SetBool;
	void (engineModule::*adrofCvar_SetString) (const char*, const char*) = &engineModule::Cvar_SetString;
	void (engineModule::*adrofAddServerCommand) (const char *, void (*) (void)) = &engineModule::AddServerCommand;
	__asm {
		//added: cvar_directset, cvar_getpointer, checkparm, savespawnparms, then below
		mov ecx, 0x44BAAC6E
		mov eax, adrofCmd_Argc
		mov dword ptr ds:[ecx], eax
		mov eax, adrofCmd_Argv
		mov dword ptr ds:[ecx + 4], eax
		mov eax, adrofCmd_Args
		mov dword ptr ds:[ecx + 8], eax
		mov eax, adrofCmd_TokenizeString
		mov dword ptr ds:[ecx + 12], eax
		mov eax, adrofCmd_ExecuteString
		mov dword ptr ds:[ecx + 16], eax
		mov eax, adrofCvar_RegisterVariable
		mov dword ptr ds:[ecx + 20], eax
		mov eax, adrofCvar_GetFloat
		mov dword ptr ds:[ecx + 24], eax
		mov eax, adrofCvar_GetInt
		mov dword ptr ds:[ecx + 28], eax
		mov eax, adrofCvar_GetBool
		mov dword ptr ds:[ecx + 32], eax
		mov eax, adrofCvar_GetString
		mov dword ptr ds:[ecx + 36], eax
		mov eax, adrofCvar_SetFloat
		mov dword ptr ds:[ecx + 40], eax
		mov eax, adrofCvar_SetInt
		mov dword ptr ds:[ecx + 44], eax
		mov eax, adrofCvar_SetBool
		mov dword ptr ds:[ecx + 48], eax
		mov eax, adrofCvar_SetString
		mov dword ptr ds:[ecx + 52], eax
		mov eax, adrofAddServerCommand
		mov dword ptr ds:[ecx + 56], eax
	}
	cvar_reg newvar;
	newvar.name = "sv_testcommand";
	newvar.string = "this is a test command";
	newvar.type = 1;
	newvar.flags = 0;
	newvar.value = (int*)25;
	Cvar_RegisterVariable(newvar);

	ShowConsoleMessage("Message from iniMod: IniMod initialized.\n");
}
DLL_EXP void SUPERTOOLS_MAIN( void ) {
	engineModule *engine = new engineModule;
	//we don't need time checks anymore since i moved this call to only run when the game needs to run a frame
	//engine->curtime = engine->realtime;
	//if ( engine->curtime <= 0 || (engine->curtime - engine->lasttime) >= (engine->GetTickInterval() - engine->host_remainder) ){
		//engine->lasttime = engine->curtime;
		engine->ServerThink();
	//}
	delete engine; //This will call its deconstructor to clear the pointer and prevent memory leaks.
}
void engineModule::ServerThink( void ) { //This is where everything gets called, MAIN LOOP/MAINLOOP
	//CBasePlayer *pPlayer = new CBasePlayer;
	CNetwork *cNetwork = new CNetwork;
	CPlugins *plugin = new CPlugins;
	//pPlayer->UpdateClients();
	cNetwork->parseConfigFile(); //This only gets called once
	cNetwork->OnServerThink(); //OnServerThink
	cNetwork->UpdatePlayers();
	plugin->OnServerThink();
	plugin->UpdatePlayers();
	//pPlayer->ShowConnectedClients(); //Show all the connected clients

	//delete pPlayer;
	delete cNetwork;
	delete plugin;
}
void engineModule::OnCommandReceived() { //We can use this to check if the users entered a command and handle our commands from here..
	//Enter code here..
}
#define	MAX_ARGS		80

static	int			cmd_argc;
static	char		*cmd_argv[MAX_ARGS];
static	char		*cmd_null_string = "";
static	char		*cmd_args = NULL;


int	engineModule::Cmd_Argc (void) {
	return cmd_argc;
}

char *engineModule::Cmd_Argv (int arg) {
	if ( (unsigned)arg >= (unsigned)cmd_argc )
		return cmd_null_string;
	return cmd_argv[arg];	
}

char *engineModule::Cmd_Args (void) {
	return cmd_args;
}

void engineModule::Cmd_TokenizeString (char *text) {
	//__asm pushad //store registers

	int		i;
	
// clear the args from the last string
	for (i=0 ; i<cmd_argc ; i++)
		delete[] cmd_argv[i];
		
	cmd_argc = 0;
	cmd_args = NULL;
	
	while (1) {
// skip whitespace up to a /n
		while (*text && *text <= ' ' && *text != '\n') 
			text++;
		
		if (*text == '\n') {	// a newline seperates commands in the buffer
			text++;
			break;
		}

		if (!*text)
			return;
	
		if (cmd_argc == 1)
			 cmd_args = text;
			
		text = COM_Parse (text);
		if (!text)
			return;

		if (cmd_argc < MAX_ARGS) {
			cmd_argv[cmd_argc] = (char *)new char[ strlen(com_token)+1 ];
			strcpy (cmd_argv[cmd_argc], com_token);
			cmd_argc++;
		}
	}
	
	//__asm popad //restore registers
}
int engineModule::returnAddedCmdPtr(const char *name) {
	 char cmd[64];
	 Utils *utils = new Utils();
	 strcpy(cmd, (char *)utils->ExplodeAndReturn(name, 0, " ").c_str());
	 for(int j=0; j<nextservercommandfuncpointersinfo; j++) {
		 ServerCommandFuncPointers * functionpointers = &servercommandfuncpointersinfo[j];
		 if(!strcmpi(cmd, functionpointers->cmdname)) {
			return functionpointers->pointer;
		 }
	 }
	 delete utils;
	 return 0;
}
void engineModule::Cmd_ExecuteString(char *text) {
	Cmd_TokenizeString(text);
}
void engineModule::Cvar_RegisterVariable(cvar_reg pCvar) {
	//types: 0-bool,1integer,2float,3string
	int adr = ADR_REGISTERVARIABLE;
	//register int next = 0;
	//for(; cmdvariablesinfo[next] != 0 && next < 100; next++)
	//cmdvariablesinfo[nextcmdvariablesinfo] = CommandVariables();
	CommandVariables * storageaddress = &cmdvariablesinfo[nextcmdvariablesinfo];
	nextcmdvariablesinfo ++;
	//int *storageaddress = new int;
	//int *storageaddress = (int*)malloc(sizeof(int));
	//int *storageaddress = (int*)0x44BAB93D;
	int typeofcmd = pCvar.type;
	const char *name = pCvar.name;
	const char *string = pCvar.string;
	int *value = pCvar.value;
	int flags = pCvar.flags;
	int tempEDI;
	__asm {
		//pushad
		//mov tempEDI, edi
		push flags
		push value
		push string
		push name
		push typeofcmd
		mov eax, adr
		mov ecx, storageaddress
		call eax
		mov eax, 0x430463A0
		push storageaddress
		call eax //43045CC0 second address is for client
		add esp, 0x4
		//popad
		//mov edi, tempEDI
	}
}
void engineModule::AddServerCommand(const char *cmd_name, void (*function) (void) ) {
	for(int j=0; j<nextservercommandfuncpointersinfo; j++) {
		ServerCommandFuncPointers * functionpointers = &servercommandfuncpointersinfo[j];
		if(!strcmpi(cmd_name, functionpointers->cmdname)) {
			printf("Cmd_AddCommand: %s already defined\n", cmd_name);
			return;
		}
	}
	printf("Adding new server command: %s\n", cmd_name);
	ServerCommands * storageaddress = &servercommandsinfo[nextservercommandsinfo];
	nextcmdvariablesinfo ++;

	ServerCommandFuncPointers * functionpointers = &servercommandfuncpointersinfo[nextservercommandfuncpointersinfo];
	nextservercommandfuncpointersinfo ++;

	functionpointers->pointer = (int)function;
	strcpy(functionpointers->cmdname, cmd_name);
}

float engineModule::Cvar_GetFloat(const char* szVarName) {
	int getpointeradr = ADR_CVARGETPOINTER;
	float value = 0;
	__asm {
		push szVarName
		call getpointeradr
		test eax, eax
		je invalidptr
			fld dword ptr ds:[eax+0x14]
			fstp dword ptr ds:[value]
		invalidptr:
		add esp, 0x4
	}
	return value;
}
void engineModule::Cvar_SetFloat(const char* szVarName, float flValue) {
	int getpointeradr = ADR_CVARGETPOINTER;
	__asm {
		push szVarName
		call getpointeradr
		test eax, eax
		je invalidptr
			mov ecx, flValue
			mov dword ptr ds:[eax+0x14], ecx
		invalidptr:
		add esp, 0x4
	}
}
int engineModule::Cvar_GetInt(const char* szVarName) {
	int getpointeradr = ADR_CVARGETPOINTER;
	int value = 0;
	__asm {
		push szVarName
		call getpointeradr
		test eax, eax
		je invalidptr
			mov eax, dword ptr ds:[eax+0x14]
			mov value, eax
		invalidptr:
		add esp, 0x4
	}
	return value;
}
void engineModule::Cvar_SetInt(const char* szVarName, int iValue) {
	int getpointeradr = ADR_CVARGETPOINTER;
	__asm {
		push szVarName
		call getpointeradr
		test eax, eax
		je invalidptr
			mov ecx, iValue
			mov dword ptr ds:[eax+0x14], ecx
		invalidptr:
		add esp, 0x4
	}
}
bool engineModule::Cvar_GetBool(const char* szVarName) {
	int getpointeradr = ADR_CVARGETPOINTER;
	bool value = 0;
	__asm {
		push szVarName
		call getpointeradr
		test eax, eax
		je invalidptr
			mov al, byte ptr ds:[eax+0x14]
			mov value, al
		invalidptr:
		add esp, 0x4
	}
	return value;
}
void engineModule::Cvar_SetBool(const char* szVarName, bool bValue) {
	int getpointeradr = ADR_CVARGETPOINTER;
	__asm {
		push szVarName
		call getpointeradr
		test eax, eax
		je invalidptr
			mov cl, bValue
			mov byte ptr ds:[eax+0x14], cl
		invalidptr:
		add esp, 0x4
	}
}
const char* engineModule::Cvar_GetString(const char* szVarName) {
	int getpointeradr = ADR_CVARGETPOINTER;
	const char* value = 0;
	__asm {
		push szVarName
		call getpointeradr
		test eax, eax
		je invalidptr
			mov eax, dword ptr ds:[eax+0x1c]
			mov value, eax
		invalidptr:
		add esp, 0x4
	}
	return value;
}
void engineModule::Cvar_SetString(const char* szVarName, const char *szValue) {
	int getpointeradr = ADR_CVARGETPOINTER;
	__asm {
		push szVarName
		call getpointeradr
		test eax, eax
		je invalidptr
			mov ecx, szValue
			mov dword ptr ds:[eax+0x1c], ecx
		invalidptr:
		add esp, 0x4
	}
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
void engineModule::UTIL_MakeVectors( Vector angles ) {
	//Vector reversedangles = Vector(angles.y, angles.z, angles.x);
	int adr = ADR_UTIL_MAKEVECTORS;
	__asm {

		push ebx //place to store our new vector
		push ecx //x
		push edx //y
		push esi //z
		mov ecx, adr
		lea eax, angles //C++ angles vector pointer
		lea ebx, [esp+0x10] //place to store our new vector
		fld dword ptr ds:[eax + 0x8] //load C++ z
		fstp dword ptr ss:[ebx + 0x0C] //store z
		fld dword ptr ds:[eax + 0x4] //load C++ y
		fstp dword ptr ss:[ebx + 0x8] //store y
		fld dword ptr ds:[eax] //load C++ x
		fstp dword ptr ss:[ebx] //store x
		push ebx
		call ecx
		pop esi
		pop edx
		pop ecx
		pop ebx
		add esp, 0x4
	}
}
Vector engineModule::v_forward( void ) {
	//the way we get v_forward is a pretty hacky attempt and SHOULDN'T work, but does
#if DEBUG
	Vector vec = Vector(ReadFloat(ADR_V_FORWARD + 0x14), -(ReadFloat(ADR_V_FORWARD)), ReadFloat(0x44B74170));
	printf("v_forward: %f %f %f\n", vec.x, vec.y, vec.z);
	return vec;
#else
	return Vector(ReadFloat(ADR_V_FORWARD + 0x14), -(ReadFloat(ADR_V_FORWARD)), ReadFloat(ADR_V_FORWARD + 2));
#endif

//the below code should have worked but it doesn't
	/*
#if DEBUG
	Vector vec = Vector(ReadFloat(ADR_V_FORWARD), ReadFloat(ADR_V_FORWARD + 4), ReadFloat(ADR_V_FORWARD + 8));
	printf("v_forward: %f %f %f\n", vec.x, vec.y, vec.z);
	return vec;
#else
	return Vector(ReadFloat(ADR_V_FORWARD), ReadFloat(ADR_V_FORWARD + 4), ReadFloat(ADR_V_FORWARD + 8));
#endif
	*/
}
__declspec(naked) int engineModule::SV_EdictToClient(int edictptr) {
	__asm{
	MOV DWORD PTR SS:[ESP-4],EDX
	MOV EAX,DWORD PTR SS:[ESP+4]
	MOV EDX, 0x421085
	SUB EAX,DWORD PTR DS:[sv_edictsadr]
	Js ZeroReturn
		MUL EDX
		DEC EDX
		CMP EDX,DWORD PTR DS:[SVS_MAXCLIENTS]
	Jge ZeroReturn
		MOV EAX, 0x4F18
		MUL EDX
		ADD EAX,DWORD PTR DS:[svs_clientsadr]
		MOV EDX,DWORD PTR SS:[ESP-4]
		RETN 4
	ZeroReturn: 
	MOV EDX,DWORD PTR SS:[ESP-4]
	MOV EAX, 0x0
	RETN 4
	}
	/*
	//The code below works but the compiler is not optimizing it
	int edictstable = *(UINT32*)sv_edictsadr;
	int index = ((edictptr - edictstable) / 0x3E0) - 1;
	if(index < 0 || index >= *(UINT32*)SVS_MAXCLIENTS)
		return NULL;

		int clienttablestart = *(UINT32*)svs_clientsadr	;
		index *= 0x4f18;
		index += clienttablestart;
		return index;
	*/
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
void engineModule::UTIL_TraceLine(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int igmon, const int *ignore, int fromedict, trace_t& ptr){
 DWORD adr = ADR_UTIL_TRACELINE;
 static trace_t temptr;
 __asm {
  lea eax, temptr
  push eax //This is TR
  push fromedict
  push ignore
  push igmon
  push vecAbsEnd
  push vecAbsStart
  call adr
  add esp, 0x18
 }
 ptr = temptr;
 printf("%d\n", temptr);
}
inline bool engineModule::Hibernate(){
	return (UINT32)ReadInt32(SV_NUMCLIENTS) == 0;
}
//Converts the TICKRATE into the interval per tick (1/tickrate)
float engineModule::GetTickInterval(){
	if ( Hibernate() )
		return max_tick_interval;

#ifdef USE_SOURCE2013_TICKRATE_METHOD
	int fpsmax = ReadInt(FPS_MAX_ADR);
	if ( last_fpsmax != fpsmax ){
		last_fpsmax = fpsmax;
		if ( fpsmax == 0 )
			fpsmax = (1 / min_tick_interval);
		interval_per_tick = float(fpsmax);
		__asm{
			MOVSS XMM0,DWORD PTR DS:[one]
			DIVSS XMM0,DWORD PTR SS:[interval_per_tick]
			MULSS XMM0,DWORD PTR DS:[fivetwelve]
			ADDSS XMM0,DWORD PTR DS:[zeropointfive]
			MOVSS XMM1,DWORD PTR DS:[zero]
			MOVSS XMM2,DWORD PTR DS:[twopower23]
			MOVSS XMM4,DWORD PTR DS:[one]
			ANDPS XMM1,XMM0
			MOVAPS XMM3,XMM1
			MOVAPS XMM1,XMM0
			XORPS XMM1,XMM3
			CMPLTSS XMM1,XMM2
			ANDPS XMM2,XMM1
			ORPS XMM2,XMM3
			MOVAPS XMM1,XMM0
			ADDSS XMM1,XMM2
			SUBSS XMM1,XMM2
			MOVAPS XMM2,XMM1
			SUBSS XMM2,XMM0
			CMPNLESS XMM2,XMM3
			ANDPS XMM2,XMM4
			SUBSS XMM1,XMM2
			MULSS XMM1,DWORD PTR DS:[oneslash512]
			MOVSS XMM0,DWORD PTR DS:[min_tick_interval]
			COMISS XMM0,XMM1
			JA JmpOne
			MOVSS XMM0,DWORD PTR DS:[max_tick_interval]
			COMISS XMM1,XMM0
			JBE JmpTwo
		JmpOne:
			MOVAPS XMM1,XMM0
		JmpTwo:
			MOVSS DWORD PTR DS:[interval_per_tick],XMM1
		}
		return interval_per_tick;
	}
#else
	interval_per_tick = (1 / float(ReadInt(FPS_MAX_ADR)));
	return interval_per_tick;
#endif
	return interval_per_tick;
}
//Determines if the engine should run a frame or not
bool engineModule::FilterTime(float dt){
	if ( dt >= DeltaFrameTime ){ //DeltaFrameTime added in Source2013, usually it's 1/fps_max
		return 1; //Tell the engine to run this frame
	}else{
		return 0; //Running too fast, don't run this frame
	}
}
//Increments the clock and adjusts time as necessary
void engineModule::Host_AccumulateTime(float time){
	realtime = time + ReadDouble(0x448BE138);
	WriteInProcessDouble(REALTIME_ADR, realtime);
	host_frametime = time;

	//Fast forward time if the user changes the cvars
	float fps = ReadFloat(host_framerate_adr);
	if ( fps > 0 ){
		if ( fps > 1 )
			fps = 1.0f/fps;
		host_frametime = fps;
	}else if (host_timescale > 0 ){
		host_frametime *= host_timescale;
	}

	//Don't allow a frame to take longer than 100ms
	host_frametime = min( host_frametime, 0.1f ); //MAX_FRAMETIME
	//host_frametime = max( host_frametime, 0.001f ); //MIN_FRAMETIME
	//removed minframetime to allow > 1000 fps
	WriteInProcessDouble(HOST_FRAMETIME_ADR, host_frametime);
}
//Calculates how long this frame took compared to the last frame so we get a consistent tickrate
void engineModule::Host_CalculateTimeDelta(){
	float prevremainder = host_remainder;
	if ( prevremainder < 0 ) 
		prevremainder = 0;

	host_remainder += host_frametime;
	int numticks = 0;	// how many ticks we will simulate this frame
	if ( host_remainder >= interval_per_tick ){
		numticks = (int)( (host_remainder / interval_per_tick ) );
		host_remainder -= numticks * interval_per_tick;
	}
	//This was added in Source2013 engine
	//This allows us to make the tickrate slightly faster this frame if the last frame took longer than expected
	DeltaFrameTime = interval_per_tick - host_remainder; 
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
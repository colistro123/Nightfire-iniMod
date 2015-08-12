#ifndef ENGINE_H
#define ENGINE_H
//#include "stdafx.h"
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
#include "addresses.h"

#define USE_SOURCE2013_TICKRATE_METHOD
#define RETURN_CHAR 1
#define RETURN_INT 2

#define PLAYERS_FILE			"config/players.dat"
#define GAMEDLL_LOCATION		"bond/server_dlls/game.dll"

using namespace std;

#define PBUFLEN 512
#define engine_debug 1

# define DLL_EXP __declspec (dllexport)

#define C_DLLEXPORT		extern "C" DLLEXPORT

//Precache table definition
typedef struct _CommandVariables {
	unsigned char bytes[35];
	/*bool operator!=(int cmpval){
		return *((int*)this) != cmpval;
	}*/
	//int id;
} CommandVariables;
typedef struct _ServerCommands {
	unsigned char bytes[16];
} ServerCommands;


typedef struct _ServerCommandFuncPointers {
	int pointer;
	char cmdname[124];
} ServerCommandFuncPointers;

class engineModule
{
public:
        //Constructor
        engineModule() { };
        //Destructor
        virtual ~engineModule() { };
		char* stringToChar(std::string mystring);
		void iniMod_load_gamedll( void );
		void SHOW_TO_CONSOLE_AND_EXIT( char* msg );
		void mainRoutineCall( void );
		int	Cmd_Argc (void);
		char *Cmd_Argv (int arg);
		char *Cmd_Args (void);
		void Cmd_TokenizeString (char *text);
		int returnAddedCmdPtr(const char *name);
		void Cmd_ExecuteString (char *text);
		void Cvar_RegisterVariable(cvar_reg pCvar);
		float Cvar_GetFloat(const char* szVarName);
		void Cvar_SetFloat(const char* szVarName, float flValue);
		int Cvar_GetInt(const char* szVarName);
		void Cvar_SetInt(const char* szVarName, int iValue);
		bool Cvar_GetBool(const char* szVarName);
		void Cvar_SetBool(const char* szVarName, bool bValue);
		const char* Cvar_GetString(const char* szVarName);
		void Cvar_SetString(const char* szVarName, const char *szValue);
		void AllocateCommand();
		void AddServerCommand(const char *cmd_name, void (*function) (void));
		void ConsoleCommand(char *text);
		void ShowConsoleMessage(char *text);
		virtual float retServFPS() { return 1 / ReadFloat(HOST_FRAMETIME_ADR); }; //virtual UINT8
		void checkForLatestPatch( void );
		void ClearConsoleScreen( void );
		void ServerThink( void );
		void OnCommandReceived( void );
		int Precache( int type, char* name );
		int IndexOfEdict(int edictptr);
		int ENTINDEX(int edictptr){return IndexOfEdict(edictptr);}
		int NUM_FOR_EDICT_FN(int edictptr);
		int EDICT_NUM(int entitynumber);
		void UTIL_MakeVectors( Vector angles );
		Vector v_forward( void );
		int SV_EdictToClient( int edictptr );
		inline bool Hibernate();
		float GetTickInterval();
		bool FilterTime(float dt);
		void Host_AccumulateTime(float time);
		void Host_CalculateTimeDelta();

		void EmitAmbientSound( int fFlags, float volume, const char *sample, int channel, int entity );
		void EmitSound( int entity, int channel, const char *sample, /*int*/float volume, float attenuation, int fFlags, int pitch );
		int GetGameTimeLeft();
		int ALLOC_STRING(const char *szStr);
		void SetModel(const char *szName, int edictptr);
		int EDICT_NUM_TO_ENTNUM(DWORD edictptr);
		void SetQueryVar(const char *param1, const char *param2);
		void UTIL_TraceLine(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int igmon, const int *ignore, int fromedict, trace_t& ptr);
		byte * LOAD_FILE_FOR_ME(const char *filename, int *pLength);
		void FREE_FILE (byte *buffer);
		char* CVAR_GET_STRING( const char *x );
		void ClientPrint(const char *message, int msgtype, int client);
		void CLIENT_PRINTF(const char *message, int msgtype, int edictptr);

		static double realtime;
		static double curtime;
		static double lasttime;
		static float host_timescale;
		static float interval_per_tick;
		static float DeltaFrameTime;
		static float host_frametime;
		static float host_remainder;
#ifdef USE_SOURCE2013_TICKRATE_METHOD
		static int last_fpsmax;
		static float newtickinterval;
		static float one;
		static float fivetwelve;
		static float zeropointfive;
		static float zero;
		static float twopower23;
		static float oneslash512;
		static float min_tick_interval;
		static float max_tick_interval;
#endif
};

typedef int ( * ENTITY_API_FN ) ( DLL_FUNCTIONS *, int Vers );
typedef void ( WINAPI * ENGINE_API_FN ) ( enginefuncs_t *, globalvars_t * );
#endif

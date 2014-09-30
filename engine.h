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

#define RETURN_CHAR 1
#define RETURN_INT 2

#define PLAYERS_FILE			"config/players.dat"
#define GAMEDLL_LOCATION		"bond/server_dlls/game.dll"

using namespace std;

#define PBUFLEN 512
#define engine_debug 1

# define DLL_EXP __declspec (dllexport)

#define C_DLLEXPORT		extern "C" DLLEXPORT

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
		void ConsoleCommand(char *text);
		void ShowConsoleMessage(char *text);
		virtual float retServFPS() { return 1 / ReadFloat(SERVER_FPS_ADDR); }; //virtual UINT8
		void checkForLatestPatch( void );
		void ClearConsoleScreen( void );
		void ServerThink( void );
		void OnCommandReceived( void );
		int Precache( int type, char* name );
		int IndexOfEdict(int edictptr);
		int ENTINDEX(int edictptr){return IndexOfEdict(edictptr);}
		int NUM_FOR_EDICT_FN(int edictptr);
		int EDICT_NUM(int entitynumber);

		void EmitAmbientSound( int fFlags, float volume, const char *sample, int channel, int entity );
		void EmitSound( int entity, int channel, const char *sample, /*int*/float volume, float attenuation, int fFlags, int pitch );
		int GetGameTimeLeft();
		int ALLOC_STRING(const char *szStr);
		void SetModel(const char *szName, int edictptr);
		int EDICT_NUM_TO_ENTNUM(DWORD edictptr);
		void SetQueryVar(const char *param1, const char *param2);
		void UTIL_TraceLine(DWORD edictptr, const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, int collisionGroup);
		byte * LOAD_FILE_FOR_ME(const char *filename, int *pLength);
		void FREE_FILE (byte *buffer);
		char* CVAR_GET_STRING( const char *x );
		void ClientPrint(const char *message, int msgtype, int client);
		void CLIENT_PRINTF(const char *message, int msgtype, int edictptr);

		static double curtime;
		static double lasttick;
};

typedef int ( * ENTITY_API_FN ) ( DLL_FUNCTIONS *, int Vers );
typedef void ( WINAPI * ENGINE_API_FN ) ( enginefuncs_t *, globalvars_t * );
#endif

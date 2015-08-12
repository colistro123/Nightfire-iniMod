#ifndef GAMEINTERFACE_H
#define GAMEINTERFACE_H

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
#include "player.h"

struct ScreenFade_t
{
	unsigned short 	duration;		// FIXED 16 bit, with SCREENFADE_FRACBITS fractional, seconds duration
	unsigned short 	holdTime;		// FIXED 16 bit, with SCREENFADE_FRACBITS fractional, seconds duration until reset (fade & hold)
	int				fadeFlags;		// flags
	byte			r, g, b, a;		// fade to color ( max alpha )
};

// Position command $position x y 
// x & y are from 0 to 1 to be screen resolution independent
// -1 means center in each dimension
// Effect command $effect <effect number>
// effect 0 is fade in/fade out
// effect 1 is flickery credits
// effect 2 is write out (training room)
// Text color r g b command $color
// Text color r g b command $color2
// fadein time fadeout time / hold time
// $fadein (message fade in time - per character in effect 2)
// $fadeout (message fade out time)
// $holdtime (stay on the screen for this long)

typedef struct hudtextparms_s
{
	float		x;
	float		y;
	int			effect;
	byte		r1, g1, b1, a1;
	byte		r2, g2, b2, a2;
	float		fadeinTime;
	float		fadeoutTime;
	float		holdTime;
	float		fxTime;
	int			channel;
} hudtextparms_t;

class GameInterface {
public:
	GameInterface() { };
    //Destructor
    virtual ~GameInterface() { };
	void TriggerHudMsg(int playerid, char *text, int duration, int towho);
	void CenterPrint(int playerid, char *text, int towho);
	void ResetSetView(int playerid);
	void SetView(int playerid, int toplayerid);
	void ClientPrint(int playerid, int dest, char *message);
	void UTIL_ScreenFade( int playerid, byte r, byte g, byte b, byte a, float fadeTime, float fadeHold, int flags );
	void UTIL_ScreenFadeAll( byte r, byte g, byte b, byte a, float fadeTime, float fadeHold, int flags );
	void UTIL_ScreenFadeOne( int playerid, byte r, byte g, byte b, byte a, float fadeTime, float fadeHold, int flags );
	void UTIL_ShowMenu(int playerid, int slots, int time, char *menu, int mlen); //Does nothing..
	void UTIL_HudMessage(int playerid, const hudtextparms_t &textparms, char *pMessage); //Doesn't work like it should because gearbox re-wrote the function
	void UTIL_SayTextFilter( int senderid, char *pText, int receiver, bool bChat ); //Redirects to hud message.
	void UTIL_ShowMessage(int playerid, char *pString); //Send a private message to a client..
	void UTIL_SetVisionColor(int playerid, int color); //Set the vision color 0-4
	void UTIL_CreateParticleEffect(int playerid, float OriginX, float OriginY, float OriginZ, float DirectionX, float DirectionY, float DirectionZ, byte Count, byte Color, int towho); //Create a particle effect
	void UTIL_CreateTracer(int playerid, float xStart, float yStart, float zStart, float xEnd, float yEnd, float zEnd, int towho); //Create a tracer
	void UTIL_CreateBeamEntsPlayers(int playerid, int toplayerid, int spriteindex, byte startframe, byte frame, byte life, byte width, byte noise, byte r, byte g, byte b, byte bright, byte scroll, int msgtype);
	void UTIL_CreateTempTextMessage(int playerid, float x, float y, byte effect, byte textR, byte textG, byte textB, byte textA, byte fxR, byte fxG, byte fxB, byte fxA, short fadein, short fadeout, short holdtime, short fxtime, char *message, int msgtype);
private:
	void Create_TE_DLIGHT(int MSGTYPE, int edictptr, float x, float y, float z, float exponent, float radius, int red, int green, int blue, int life, float decayRate);
public:
	void UTIL_Create_TE_ELIGHT(int edictptr, float x, float y, float z, int radius, int red, int green, int blue, int life, int decayRate ); //Create entity lights
	void UTIL_Create_TE_DLIGHT(int edictptr, float x, float y, float z, float exponent, float radius, int red, int green, int blue, int life, float decayRate );
	void Create_TE_EXPLOSION( float x, float y, float z, int iSprite, byte scale, byte frameRate, byte flags );
	void Util_Attach_Entity(int playerid, float VertOfsZ, short modelIndex, short life, int msgtype);
	void SendMOTDToClient(int playerid);
	void UTIL_ScreenShake(int playerid, short amplitude, short duration, short frequency, short radius, short command);
};
#endif

static cell AMX_NATIVE_CALL n_TriggerHudMsg(AMX *amx, const cell *params) {
	GameInterface *gui = new GameInterface;
	char *ret = NULL;
    amx_StrParam(amx, params[2], ret);
    gui->TriggerHudMsg( (int)params[1], ret, (int)params[3], (int)params[4] );
	delete gui;
	return true;
}
static cell AMX_NATIVE_CALL n_CenterPrint(AMX *amx, const cell *params) {
	GameInterface *gui = new GameInterface;
	char *ret = NULL;
    amx_StrParam(amx, params[2], ret);
    gui->CenterPrint( (int)params[1], ret, (int)params[3] );
	delete gui;
	return true;
}
static cell AMX_NATIVE_CALL n_SetView(AMX *amx, const cell *params) {
	GameInterface *gui = new GameInterface;
    gui->SetView( (int)params[1], (int)params[2] );
	delete gui;
	return true;
}
static cell AMX_NATIVE_CALL n_ClientPrint(AMX *amx, const cell *params) {
	GameInterface *gui = new GameInterface;
	char *ret = NULL;
    amx_StrParam(amx, params[3], ret);
    gui->ClientPrint( (int)params[1], (int)params[2], ret );
	delete gui;
	return true;
}
static cell AMX_NATIVE_CALL n_UTIL_ScreenFade(AMX *amx, const cell *params) { //int playerid, byte r, byte g, byte b, byte a, float fadeTime, float fadeHold, int flags
	GameInterface *gui = new GameInterface;
    gui->UTIL_ScreenFade( (int)params[1], (byte)params[2], (byte)params[3], (byte)params[4], (byte)params[5], amx_ctof(params[6]), amx_ctof(params[7]), (int)params[8] );
	delete gui;
	return true;
}
static cell AMX_NATIVE_CALL n_UTIL_SayTextFilter(AMX *amx, const cell *params) { //int senderid, char *pText, int receiver, bool bChat
	GameInterface *gui = new GameInterface;
	char *ret = NULL;
    amx_StrParam(amx, params[2], ret);
    gui->UTIL_SayTextFilter( (int)params[1], ret, (int)params[3], params[4] );
	delete gui;
	return true;
}
static cell AMX_NATIVE_CALL n_UTIL_ShowMessage(AMX *amx, const cell *params) { //(int playerid, char *pString)
	GameInterface *gui = new GameInterface;
	char *ret = NULL;
    amx_StrParam(amx, params[2], ret);
    gui->UTIL_ShowMessage( (int)params[1], ret );
	delete gui;
	return true;
}
static cell AMX_NATIVE_CALL n_UTIL_SetVisionColor(AMX *amx, const cell *params) { //(int playerid, int color)
	GameInterface *gui = new GameInterface;
    gui->UTIL_SetVisionColor( (int)params[1], (int)params[2] );
	delete gui;
	return true;
}
static cell AMX_NATIVE_CALL n_UTIL_CreateBeamEntsPlayers(AMX *amx, const cell *params) { //(int playerid, int toplayerid, int spriteindex, byte startframe, byte frame, byte life, byte width, byte noise, byte r, byte g, byte b, byte bright, byte scroll, int msgtype)
	GameInterface *gui = new GameInterface;
    gui->UTIL_CreateBeamEntsPlayers( (int)params[1], (int)params[2], (int)params[3], (byte)params[4], (byte)params[5], (byte)params[6], (byte)params[7], (byte)params[8], (byte)params[9], (byte)params[10], (byte)params[11], (byte)params[12], (byte)params[13], (int)params[14]  );
	delete gui;
	return true;
}
static cell AMX_NATIVE_CALL n_UTIL_Create_TE_DLIGHT(AMX *amx, const cell *params) {
	GameInterface *gui = new GameInterface;
	gui->UTIL_Create_TE_DLIGHT((int) params[1], amx_ctof(params[2]), amx_ctof(params[3]), amx_ctof(params[4]), amx_ctof(params[5]), amx_ctof(params[6]), (int) params[7], (int) params[8], (int) params[9], (int) params[10], amx_ctof(params[11]));
	delete gui;
	return true;
}
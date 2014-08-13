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

class GameInterface {
public:
	GameInterface() { };
    //Destructor
    virtual ~GameInterface() { };
	void TriggerHudMsg(int playerid, char *text, int duration, int towho);
};
#endif

static cell AMX_NATIVE_CALL n_TriggerHudMsg(AMX *amx, const cell *params) {
	GameInterface *gui = new GameInterface;
	char *ret = NULL;
    amx_StrParam_Type(amx, params[2], ret, char*);
    gui->TriggerHudMsg( (int)params[1], ret, (int)params[3], (int)params[4] );
	delete gui;
	return true;
}

#ifndef CON_COMMANDS_H
#define CON_COMMANDS_H

#include <stdio.h>
#include <cstdio>
#include <cmath>
#include <math.h>
#include <conio.h>
#include <iostream>
#include <winsock2.h>
#include <time.h>
#include <fstream>
#include <vector>
#include < extdll.h >
#include <string>
#include "string.h"
#include "addresses.h"

using namespace std;

class CCommands {
public:
	CCommands() { };
	//Functions
	void tryFindCommand(const char* recvcmd);
	int returnCommandParamCount(const char* params);
    //Destructor
    virtual ~CCommands() { };
	char* msg;
	char buffer[1024]; //For messages and such..
};

typedef void (*RCONCmdProc)(const char *params);
//Commands
void rconcmd_players(const char *params);
void rconcmd_availablecommands(const char *params);
void rconcmd_emitambsound(const char *params);
void rconcmd_emitambsoundshort(const char *params);
void rconcmd_precache(const char *params);
void rconcmd_loadplugin(const char *params);
void rconcmd_unloadplugin(const char *params);
void rconcmd_unloadallplugins(const char *params);
#endif

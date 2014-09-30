#ifndef HOOKS_H
#define HOOKS_H

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

using namespace std; 

//void placeJMP(BYTE * address, DWORD jumpTo, DWORD length);
void PlaceJMP( BYTE *bt_DetourAddress, DWORD dw_FunctionAddress, DWORD dw_Size);
void applyPatches( void );
void mainHooks( void );
BYTE* getHookFuncJMPAddr(DWORD fnName);

//Hooks
void OnTriggerEvent();
void OnCommandProcess();
void hookOnServerChangeMap();
void hookOnClientConnect();
void hookOnClientDisconnect();
void hookOnBotConnect();
void hookOnHostSay();
void hookOnPlayerKilled();
void hookOnPlayerSuicided();
void hookOnPlayerEquip();
void hookOnPlayerSpawn();
void hookOnMapLoaded();
void hookOnHudInit();
void hookOnClientTimeOut();
void hookOnFireBullets();
void hookOnDropFakeClient();
void hookOnClientReconnect();

//Some functions we need to call (Thanks naked functions)
void callConCommandModule( DWORD ebx );
void callTextModule(int nfuserid, DWORD ebx);
void callGMModule(int playerid, DWORD edi);
void callOnPreClientConnect(int pPointer);
void callOnClientDisconnect(int clientptr);
void callOnServerChangeMap(const char* mapname);
void callOnClientDeath(int attacker, int receiver);
void callOnClientEquip(int nfuserid);
void callOnClientSpawn(int nfuserid);
void callOnMapLoaded();
void callOnClientTimeOut(int edict);
void callOnFireBullets(int edict, Vector vecSrc, Vector vecShootDir, float dist);
void callOnHudInit(int edictPtr);
void callOnDropFakeClient(int edictPtr);
void callOnClientReconnect(int edict);
#endif

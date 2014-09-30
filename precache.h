#ifndef PRECACHE_H
#define PRECACHE_H

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

//Precache table definition
typedef struct _PrecacheInfo {
	int precachetype;
	char precachename[MAX_STRING_LEN];
	//int id;
} PrecacheInfo;

class Precache {
public:
	Precache() { };
	//Precaching
	int IsItemOnPrecacheTable( const char* actualprecache );
	void AddPrecacheToTable(int type, const char* actualprecache);
	void PreloadCached( bool remove );
	int GetPrecacheID(const char* actualprecache);
	void RemovePrecacheFromTable( int precacheid );
	void RemoveAllPrecacheFromTable( void );
	int TotalPrecached();
    //Destructor
    virtual ~Precache() { };
	char* msg;
	char buffer[1024]; //For messages and such..
};

//Precache
int find_precachetype(const char *name);
#endif

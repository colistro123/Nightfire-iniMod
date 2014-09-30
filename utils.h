#ifndef UTILS_H
#define UTILS_H

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
#include "amx/amx.h"

using namespace std;

class Utils {
public:
	Utils() { };
    //Destructor
    virtual ~Utils() { };
	string ExplodeAndReturn(string stringToExplode, int field, string delimiter);
	vector<string> explode( const string &delimiter, const string &str);
	float GetPointDistance(float px, float py, float pz, float X, float Y, float Z);
	char* msg;
	char buffer[1024]; //For messages and such..
};

static cell AMX_NATIVE_CALL n_GetPointDistance(AMX *amx, const cell *params) {
	Utils *utilPtr = new Utils;
	float dist;
	dist = utilPtr->GetPointDistance(amx_ctof(params[1]), amx_ctof(params[2]), amx_ctof(params[3]), amx_ctof(params[4]), amx_ctof(params[5]), amx_ctof(params[6]));
	delete utilPtr;
	return dist;
}
#endif

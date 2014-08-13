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

using namespace std;

class Utils {
public:
	Utils() { };
    //Destructor
    virtual ~Utils() { };
	string ExplodeAndReturn(string stringToExplode, int field, string delimiter);
	vector<string> explode( const string &delimiter, const string &str);
	char* msg;
	char buffer[1024]; //For messages and such..
};
#endif

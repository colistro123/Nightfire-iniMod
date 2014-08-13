#ifndef HOST_CMD_H
#define HOST_CMD_H

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

class HostCMD {
public:
	HostCMD() { };
	//Functions
	void OnPreClientCommandText(int nfuserid, const char* recvcmd);
	int find_cmdParam(const char *name);
    //Destructor
    virtual ~HostCMD() { };
	char* msg;
	char buffer[1024]; //For messages and such..

};

//Command external (outside of the class) functions


typedef void (*HostCMDFunc)(const char *params);


#endif

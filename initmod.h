#ifndef INITMOD_H
#define INITMOD_H
#include <stdio.h>
#include <cstdio>
#include <cmath>
#include <math.h>
#include <conio.h>
#include <iostream>
#include <winsock2.h>
#include <time.h>
#include "engine.h"

using namespace std;

#define BUFLEN 512	//Max length of buffer


//#define SERVER_FILE "config/server.dat"

class iniMod
{
public:
        //Constructor
        iniMod() { cout<<"Instatiating Constructor()"<<endl;};
        //Destructor
        virtual ~iniMod() { cout<<"Destroying Constructor()"<<endl; };
};
#endif

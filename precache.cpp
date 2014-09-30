#include "precache.h"
#include "engine.h"
#include "utils.h"
#include <iostream>     // std::cout
#include <algorithm>    // std::count

std::vector<_PrecacheInfo> precacheinf;

typedef struct {
	char *name;
	int type;
} PrecacheSpecs;

PrecacheSpecs precachinginfo[] = {
	{"sound", TYPE_SND},
	{"model", TYPE_MDL},
	{"generic", TYPE_GENERIC},
	{"event", TYPE_EVENT}
};

int Precache::IsItemOnPrecacheTable(const char* actualprecache) {
	std::vector<_PrecacheInfo>::iterator it = precacheinf.begin();
	_PrecacheInfo precache;
	int i = 0;
	while(it != precacheinf.end()) {
		precache = *(it++);
		if(strcmp(precache.precachename, actualprecache) == 0) {
			return 1;
		}
	}
	return 0;
}
int Precache::GetPrecacheID(const char* actualprecache) {
	std::vector<_PrecacheInfo>::iterator it = precacheinf.begin();
	_PrecacheInfo precache;
	int i = 0;
	while(it != precacheinf.end()) {
		precache = *(it++);
		if(strcmpi(precache.precachename, actualprecache) == 0) {
			return i;
		}
		i++;
	}
	return 0;
}
void Precache::AddPrecacheToTable(int type, const char* actualprecache) {
	_PrecacheInfo precache;
	memset(&precache, 0, sizeof(PrecacheInfo));
	
	if(!IsItemOnPrecacheTable(actualprecache)) {
		Utils *utils = new Utils();
		precache.precachetype = type;
		sprintf(precache.precachename, "%s", actualprecache);

		precacheinf.push_back(precache);
		delete utils;
	}
}
void Precache::RemoveAllPrecacheFromTable( void ) { //There's actually no point in this since you can't load them when the map is running..
	for (int i=0; i<TotalPrecached(); i++) {
		RemovePrecacheFromTable(i);
	}
}
void Precache::PreloadCached( bool remove ) {
	engineModule *engine = new engineModule;
	if(remove) RemoveAllPrecacheFromTable(); //Remove everything first
	for (std::vector<_PrecacheInfo>::iterator it = precacheinf.begin(); it != precacheinf.end(); it++) {
		if(IsItemOnPrecacheTable(it->precachename)) {
			engine->Precache(it->precachetype, it->precachename);
		}
	}
	delete engine;
}
int Precache::TotalPrecached() {
	engineModule *engine = new engineModule;
	int i = 0;
	for (std::vector<_PrecacheInfo>::iterator it = precacheinf.begin(); it != precacheinf.end(); it++) {
		if(IsItemOnPrecacheTable(it->precachename)) {
			i++;
		}
	}
	return i;
	delete engine;
}
void Precache::RemovePrecacheFromTable( int precacheid ) {
	int i = 0;
	engineModule *engine = new engineModule;
	for (std::vector<_PrecacheInfo>::iterator it = precacheinf.begin(); it != precacheinf.end(); it++) {
		if(i == precacheid) {
			printf("Precache::RemovePrecacheFromTable: Removed %s (%d) from table!\n", it->precachename, i);
			precacheinf.erase(it);
			break;
		}
		i++;
	}
	delete engine;
}
int find_precachetype(const char *name) {
	for(int i=0;i<sizeof(precachinginfo)/sizeof(PrecacheSpecs);i++) {
		if(strcmpi(name,precachinginfo[i].name) == 0) {
			return precachinginfo[i].type;
		}
	}
	return NULL;
}

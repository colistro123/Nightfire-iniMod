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
void Precache::PreloadCached( void ) {
	engineModule *engine = new engineModule;
	std::vector<_PrecacheInfo>::iterator it;
	int i = 0;
	for (it = precacheinf.begin(); it != precacheinf.end(); it++) {
		_PrecacheInfo precache = *(it);
		precache = *(it);
		if(IsItemOnPrecacheTable(precache.precachename)) {
			engine->Precache(precache.precachetype, precache.precachename);
		}
		i++;
	}
	delete engine;
	return;
}
void Precache::RemovePrecacheFromTable( int precacheid ) {
	std::vector<_PrecacheInfo>::iterator it;
	int i = 0;
	for (it = precacheinf.begin(); it != precacheinf.end(); it++) {
		_PrecacheInfo precache = *(it);
		precache = *(it);
		if(i++ == precacheid) {
			precacheinf.clear();
			//precacheinf.erase(it);
			return;
		}
	}
}
int find_precachetype(const char *name) {
	for(int i=0;i<sizeof(precachinginfo)/sizeof(PrecacheSpecs);i++) {
		if(strcmpi(name,precachinginfo[i].name) == 0) {
			return precachinginfo[i].type;
		}
	}
	return NULL;
}

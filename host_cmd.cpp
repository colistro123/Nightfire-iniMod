#include "host_cmd.h"
#include "precache.h"
#include "engine.h"
#include "utils.h"
#include "player.h"
#include "gamemodes.h"
#include <iostream>     // std::cout
#include <algorithm>    // std::count

typedef struct {
	char name;
} HostCMDInfo;
//HostCMDFunc
HostCMDInfo host_cmds[] = {
	{'!'},
	{'#'},
	{'/'}
};

int HostCMD::find_cmdParam(const char *name) {
	for(int i=0;i<sizeof(host_cmds)/sizeof(HostCMDInfo);i++) {
		if(name[0] == host_cmds[i].name) {
			return 1;
		}
	}
	return NULL;
}

void HostCMD::OnPreClientCommandText(int nfuserid, const char* recvcmd) {
	printf("value %s\n", recvcmd);
	Utils *utils = new Utils;
	engineModule *engine = new engineModule;
	if(find_cmdParam(recvcmd)) {
		CBasePlayer *pPlayer = new CBasePlayer;
		CNetwork *gGameMode = new CNetwork;
		int clientid = pPlayer->GetInternalIDFromNFID(nfuserid);
		gGameMode->OnClientCommandText(clientid, recvcmd);
		delete pPlayer;
		delete gGameMode;
	}
	delete engine;
	delete utils;
}

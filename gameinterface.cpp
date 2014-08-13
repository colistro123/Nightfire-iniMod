#include "gameinterface.h"

void GameInterface::TriggerHudMsg(int playerid, char *text, int duration, int towho) {
	CBasePlayer *pPlayer = new CBasePlayer();
	pPlayer->setPointerForPlayerID(playerid);
	int edptr = 0;
	if ( towho != MSG_BROADCAST ){
		edptr = ReadInt32(pPlayer->cptr + client_t::pedicts);
	}
	MESSAGE_BEGIN(edptr, 0, 0x73, towho); //0x73 for GUI messages, is 0x75 used for centered screen messages?
	WRITE_STRING(text); //text
	WRITE_BYTE(1); //spawnflags
	WRITE_BYTE(1); //show on this minimum skill level: 1 = operative, 2 = agent, 3 = 00agent
	WRITE_BYTE(duration); //duration of hudmsg
	MESSAGE_END();
	delete pPlayer;
}

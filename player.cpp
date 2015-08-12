#include "player.h"
#include "gameinterface.h"
#include "engine.h"
#include "utils.h"
#include "gamemodes.h"
#include "baseentity.h"
#include "gettime.h"
#include "plugins.h"

#include <malloc.h>				// malloc, etc
#include <errno.h>				// errno, etc
#include <extdll.h>				// always

//Set default values to the static values we initialized in engine.h
float	CBasePlayer::health = 0;
char*	CBasePlayer::name = "New Agent";
float	CBasePlayer::kills = 0;
int		CBasePlayer::deaths = 0;
int		CBasePlayer::ctfscore1 = 0;
int		CBasePlayer::ctfscore2 = 0;
float	CBasePlayer::x = 0;
float	CBasePlayer::y = 0;
float	CBasePlayer::z = 0;
char*	CBasePlayer::alivestatus = "Dead";
char*	CBasePlayer::bondexe;
//int		CBasePlayer::cptr = 0;
int		CBasePlayer::NumPlayersWithoutDynamicLightSupport = 0;
int		CBasePlayer::edictptr = 0;
//int		CBasePlayer::ptr = 0;
double	CBasePlayer::curtime = 0;
double	CBasePlayer::lasttick = 0;
int		CBasePlayer::currentclient = 0;

char*	CBasePlayer::gamever = 0;
int		CBasePlayer::lagcompensation = 0;
int		CBasePlayer::connected = 0;
int		CBasePlayer::active = 0;
int		CBasePlayer::spawned = 0;
int		CBasePlayer::IsFakeClient = 0;
int		CBasePlayer::userid = 0;
int		CBasePlayer::amountoftimeconnected_secs = 0;
int		CBasePlayer::amountoftimeconnected_mins = 0;

std::vector<_PlayerInfo> players;

int CBasePlayer::GetEdictNum(int playerid){ //Can now replace all the things that get client_t::pedicts with this :)
	int result = getPointerForPlayerID(playerid);
	if ( result != 0 )
		result = ReadInt32(result + client_t::pedicts);
	return result;
}
int CBasePlayer::getPointerForPlayerID( int playerid ) {
	//playerid = GetNFIDFromInternalID(playerid); //Fixed - Not really... So do ID's in Nightfire start from 0 or 1? What the.. Tripping colors...
	//playerid == 0 ? 1 : playerid; //If the playerid is 0, then make it 1, otherwise make it the actual playerid (the sent playerid). (I'm doing this because NF's playerid's start from 1 instead of 0..)
	int ptr, cptr;
	ptr = ReadInt32(0x44A86B4C); //get first pointer
	cptr = ptr + (PLAYER_TABLE_CELLSIZE * playerid);
	//memSetPtr(cptr);
	return cptr;
}
Vector CBasePlayer::GetGunPos(int edict) {
	Vector retvect;
	__asm { 
		//push eax
		push ecx
		push edx
		lea eax, retvect
		mov ecx, edict
		mov ecx, dword ptr ds:[ecx + 0x11c]
		mov ecx, dword ptr ds:[ecx + 0x0A0C] //gets m_pPlayer of edict
		mov ecx, dword ptr ds:[ecx + 0x78]
		mov edx, dword ptr ds:[ecx]
		push eax
		call dword ptr ds:[edx + 0x248] //GetGunPos
		pop edx
		pop ecx
		//pop eax
	}
#ifdef DEBUG
	printf("Player Gun Pos: %f %f %f\n", retvect.x, retvect.y, retvect.z);
#endif
	return retvect;
}
Vector CBasePlayer::EyeAngles(int edict) {
	//same as v_angle
	engineModule *engine = new engineModule;
	int cptr = engine->SV_EdictToClient(edict);
	delete engine;
	if ( cptr ) {
		if ( memGetInt(cptr + client_t::pspawned) ) {
			int adr = ReadInt32(edict + edict::pentity);
			adr = ReadInt32(adr + player::punknown4);
			return Vector((ReadFloat(adr + player::peyeanglesx)), ReadFloat(adr + player::peyeanglesy), ReadFloat(adr + player::peyeanglesz));
		}
	}
	return NULL;
}
Vector CBasePlayer::WeaponAngles(int edict) {
	engineModule *engine = new engineModule;
	int cptr = engine->SV_EdictToClient(edict);
	delete engine;
	if ( cptr ) {
		if ( memGetInt(cptr + client_t::pspawned) ) {
			int adr = ReadInt32(edict + edict::pentity);
			adr = ReadInt32(adr + player::punknown4);
									//you can * this by -3 to get eye angle pitch
			return Vector((ReadFloat(adr + player::pweaponanglesx)), ReadFloat(adr + player::pweaponanglesy), ReadFloat(adr + player::pweaponanglesz));
		}
	}
	return NULL;
}
Vector CBasePlayer::v_angle( int edictptr ) {
	//this function is the same thing as EyeAngles
	edictptr += 0x120;
	//edictptr is now 1c1a6940
	static Vector vec;
	__asm {
		push edx
		push ecx
		lea edx, vec
		mov eax, edictptr
		mov ecx, dword ptr ds:[eax + 0x74]
		mov dword ptr ds:[esp - 4], ecx
		fld dword ptr ss:[esp - 4]
		fstp dword ptr ds:[edx]
		mov ecx, dword ptr ds:[eax + 0x78]
		mov dword ptr ds:[esp - 4], ecx
		fld dword ptr ss:[esp - 4]
		fstp dword ptr ds:[edx + 4]
		mov ecx, dword ptr ds:[eax + 0x7C]
		mov dword ptr ds:[esp - 4], ecx
		fld dword ptr ss:[esp - 4]
		fstp dword ptr ds:[edx + 8]
		pop ecx
		pop edx
	}
#if DEBUG
	printf( "v_angle: %f %f %f\n", vec.x, vec.y, vec.z);
#endif
	return vec; 
}
Vector CBasePlayer::punchangle( int edictptr ) {
	edictptr += 0x120;
	return Vector(ReadFloat(edictptr + 0x68), ReadFloat(edictptr + 0x6C), ReadFloat(edictptr + 0x70));
}
void CBasePlayer::SetPlayerHealth(int playerid, int percent) {
	int cptr = 0;
	cptr = getPointerForPlayerID(playerid);
	if(!HasClientSpawned(playerid)) 
		return;

	int adr = ReadInt32(cptr + client_t::pedicts);
	adr = ReadInt32(adr + edict::pentity);
	adr = ReadInt32(adr + player::punknown4);
	WriteInProcessFloat((adr + player::phealth) , float(percent * 2));
}
float CBasePlayer::GetPlayerHealth(int playerid) {
	int cptr = 0;
	cptr = getPointerForPlayerID(playerid);
	if(!HasClientSpawned(playerid)) 
		return 0;

	int adr = ReadInt32(cptr + client_t::pedicts);
	adr = ReadInt32(adr + edict::pentity);
	adr = ReadInt32(adr + player::punknown4);
	return ReadFloat(adr + player::phealth)/2;
}
int CBasePlayer::IsClientConnectedEx( int playerid ) {
	std::vector<_PlayerInfo>::iterator it = players.begin();
	_PlayerInfo player;
	int i = 1; //This was 0 but nf id's often start from 1 so..
	while(it != players.end()) {
		player = *(it++);
		if(i++ == playerid) {
			return player.connected;
		}
	}
	return 0;
}
int CBasePlayer::OnClientSpawn( int playerid ) {
	printf("OnClientSpawn(%d) has been called.\n", playerid);
	//Scripting call (Gamemode)
	CNetwork *cNetwork = new CNetwork;
	CPlugins *plugin = new CPlugins;
	cNetwork->OnClientSpawn(playerid);
	plugin->OnClientSpawn(playerid);
	delete cNetwork;
	delete plugin;
	return 1;
}
int CBasePlayer::OnClientEquip( int playerid ) {
	printf("OnClientEquip(%d) has been called.\n", playerid);
	//Scripting call (Gamemode)
	CNetwork *cNetwork = new CNetwork;
	CPlugins *plugin = new CPlugins;
	cNetwork->OnClientEquip(playerid);
	plugin->OnClientEquip(playerid);
	delete cNetwork;
	delete plugin;
	return 1;
}
void CBasePlayer::ProcessClientsOnMapLoad( void ) {
	for(int i=0; i<ReadInt32(SVS_MAXCLIENTS); i++) {
		if(!IsClientOnTable(i)) {
			if(IsClientConnected(i)) {
				printf("CBasePlayer::The client was not on the table, so he or she has been added to it. id: %d\n", i);
				UpdateClientsTable( i, ADD_TO_TABLE );
			}
		}
	}
}
bool CBasePlayer::SupportsDynamicLights( int playerid ) {
	std::vector<_PlayerInfo>::iterator it = players.begin();
	_PlayerInfo player;
	while(it != players.end()) {
		player = *(it++);
		if(playerid == player.pinternaluserid)
			return player.supportsdynamiclights;
	}
	return false;
}
float CBasePlayer::GetGameVer( int playerid ){
	std::vector<_PlayerInfo>::iterator it = players.begin();
	_PlayerInfo player;
	while(it != players.end()) {
		player = *(it++);
		if(playerid == player.pinternaluserid)
			return player.gamever;
	}
	return 0.0f;
}
bool CBasePlayer::IsClientConnected( int playerid ) { //This lets us know if the client is connected, but this is straight from the engine. I'll use this to update the list
	int cptr = 0;
	cptr = getPointerForPlayerID(playerid);
	return cptr == 0 ? 0 : ReadInt32(cptr + client_t::pconnected);
}
int CBasePlayer::HasClientSpawned( int playerid ) { //This lets us know if the client has spawned or not, used upon health checks
	int cptr = 0;
	cptr = getPointerForPlayerID(playerid);
	return memGetInt(cptr + client_t::pspawned) ? 1 : 0;
}
int CBasePlayer::GetPlayerDeaths( int playerid ) {
	int cptr = 0;
	cptr = getPointerForPlayerID(playerid);
	if(!HasClientSpawned(playerid))
		return 0;

	int adr = ReadInt32(cptr + client_t::pedicts);
	adr = ReadInt32(adr + edict::pentity);
	return ReadInt32(adr + player::pdeaths);
}
float CBasePlayer::GetPlayerKills( int playerid ) {
	int cptr = 0;
	cptr = getPointerForPlayerID(playerid);
	if(!HasClientSpawned(playerid))
		return 0;

	int adr = ReadInt32(cptr + client_t::pedicts);
	return ReadFloat(adr + edict::pkills); //Return a round value
}
void CBasePlayer::UpdateClients( void ) {
	curtime = Sys_FloatTime() * 1000.0f;
	CNetwork *cNetwork = new CNetwork;
	if ( curtime - lasttick > CLIENT_SCAN_TIME ){ //sanity check
		lasttick = curtime;
		if(NumClientsOnTable() < 1 && NumClients() < 1) return; //Don't do anything.. (Optimization)
		for(int playerid=0; playerid<MaxClients(); playerid++) { //Actually, you were right, it's better to just parse through all the slots (What if we miss one id?, NumClients only goes from 1 to Max Clients Connected, making us skip certain id's since the player id indexes always go up by 1 so a player for example, could have ID 7 but the number of clients being 5, making 7 unreachable in a for loop for example..)
			UpdateHeartBeat(playerid); //Updates the heartbeat for that specific client to later on check if they've disconnected directly from the server
			CheckTimeOut(playerid);
		}
	}
	currentclient = 0;
	delete cNetwork;
}
void CBasePlayer::CheckTimeOut(int playerid) {
	CNetwork *cNetwork = new CNetwork;
	if(IsClientConnected(playerid)) {
		if(IsAClient(playerid)) { //Check if they're a client
			if(LostBeat(playerid)) {
				cNetwork->OnClientDisconnect(playerid, REASON_TIMEOUT);
			}
		}
	}
	delete cNetwork;
}
void CBasePlayer::ShowConnectedClients( void ) { //Just display our updated table
	char cmsg[128];
	std::vector<_PlayerInfo>::iterator it = players.begin();
	_PlayerInfo player;
	Utils *utils = new Utils();
	memset(utils->buffer, 0, sizeof(utils->buffer));
	int i = 0;
	engineModule *engine = new engineModule();
	engine->ClearConsoleScreen();
	if(NumClients() == 0) {
		engine->ShowConsoleMessage("iniMod: No clients connected\n");
		return;
	}
	while(it != players.end()) {
		player = *(it++);
		if(i <= NumClients()) {
			sprintf(cmsg,"ID: %d Name: %s Score: %d Ping: %d Connected: %s Health: %.1f Kills: %d, Deaths: %d\n",i,GetClientName(i).c_str(),player.score,player.ping,IsClientConnected(i) ? "Yes" : "No",GetPlayerHealth(i), (int)floor((float)GetPlayerKills(i)), GetPlayerDeaths(i));
			strcat(utils->buffer, cmsg);
			i++;
		}
	}
	i = 0;
	printf(utils->buffer);
	delete utils;
	delete engine;
	return;
}
void CBasePlayer::UpdateClientsTable( int playerid, int option ) {
	switch(option) {
		case REMOVE_FROM_TABLE: {
			if(IsClientOnTable(playerid)) {
				RemovePlayerFromTable(playerid);
			}
			break;
		}
		case ADD_TO_TABLE: {
			if(!IsClientOnTable(playerid)) {
				AddPlayerToTable(playerid);
			}
			break;
		}
	}
}
void CBasePlayer::RemovePlayerFromTable( int playerid ) {
	for (std::vector<_PlayerInfo>::iterator it = players.begin(); it != players.end();) {
		if(it->pinternaluserid == playerid) {
			printf("CBasePlayer::RemovePlayerFromTable: Removed player from table with index %d!\n", it->pinternaluserid);
			it = players.erase(it);			
		} else {
			++it;
		}
	}
}
int CBasePlayer::IsClientOnTable( int playerid ) {
	for (std::vector<_PlayerInfo>::iterator it = players.begin(); it != players.end(); it++) {
		if(it->pinternaluserid == playerid) {
			return 1;
		}
	}
	return 0;
}
void CBasePlayer::UpdateHeartBeat( int playerid ) {
	int cptr = 0;
	cptr = getPointerForPlayerID(playerid);
	std::vector<_PlayerInfo>::iterator it = players.begin();
	int i = 0;
	CNetwork *cNetwork = new CNetwork();
	while(it != players.end()) {
		_PlayerInfo& player = *(it++); //Pass by reference, to write
		if(playerid == player.pinternaluserid) {
			if(IsClientConnected(playerid)) { //Only update their last heartbeat if the client is connected
				player.plast_heartbeat = GetConnectTime(playerid);
				player.pnext_heartbeat = player.plast_heartbeat++;
				cNetwork->OnClientUpdate(playerid); //Call OnClientUpdate which we will hook later for this client..
			} else {
				player.pnext_heartbeat = 0;
			}
		}
	}
	delete cNetwork;
	return;
}
int CBasePlayer::GetBeat( int playerid, int type ) { //1 for next, 2 for last
	int cptr = 0;
	cptr = getPointerForPlayerID(playerid);
	std::vector<_PlayerInfo>::iterator it = players.begin();
	_PlayerInfo player;
	int i = 0;
	int value = 0;
	while(it != players.end()) {
		player = *(it++);
		if(i++ == playerid) {
			value = (type == LAST_BEAT) ? player.plast_heartbeat : player.pnext_heartbeat;
		}
 	}
	return value;
}
bool CBasePlayer::LostBeat(int playerid) {
	return (GetBeat( playerid, LAST_BEAT ) > GetBeat( playerid, NEXT_BEAT ) + MAX_BEAT_INTERVAL) ? TRUE : FALSE;
}
int CBasePlayer::IsClientOnTableByNFID( int playerid ) {
	std::vector<_PlayerInfo>::iterator it = players.begin();
	_PlayerInfo player;
	int i = 1;
	while(it != players.end()) {
		player = *(it++);
		if(playerid == player.puserid && player.connected == 1) {
			return 1;
		}
	}
	return 0;
}
void CBasePlayer::AddPlayerToTable(int playerid) {
	_PlayerInfo new_player;
	memset(&new_player, 0, sizeof(PlayerInfo));

	int score = 0, 
		ping = 0;

	int cptr = 0;
	cptr = getPointerForPlayerID(playerid);
	int clientid = ReadByte(cptr + client_t::puserid);
	int edictptr = GetEdictNum(playerid);

	if(!IsClientOnTableByNFID(clientid)) {
		/*
		if(!IsClientConnected(playerid)) //If the player is not connected..
			return;
			//Could desync on timeouts so I removed it
		*/
		Utils *utils = new Utils();
		/*
		new_player.score = score;
		new_player.ping = ping;
		*/
		userinfostring = ReadCharArray(cptr + client_t::puserinfo, 511);
		new_player.plagcompensation = ReadByte(cptr + client_t::plagcompensation);
		new_player.pfakeclient = ReadByte(cptr + client_t::pfakeclient);

		new_player.puserinfo = userinfostring;
		new_player.connected = ReadByte(cptr + client_t::pconnected); //The client is now connected, put this on our table
		new_player.gamever = 0.0f;
		new_player.supportsdynamiclights = false;
		if(new_player.pfakeclient == 0) {
		string gamever = GetUserInfoStringValue("gamever", edictptr);
		char string[64];
		strcpy(string, utils->ExplodeAndReturn(gamever, 0, " ").c_str());
		if( strlen(string) > 0 )
			new_player.gamever = stof((const char*)string);
			new_player.supportsdynamiclights = ( new_player.gamever > 5.81f );
		}
		NumPlayersWithoutDynamicLightSupport+= !new_player.supportsdynamiclights;

		strcpy(new_player.name, (char *)GetClientName(playerid).c_str());
		
		new_player.puserid = GetPlayerUserID(edictptr);
		new_player.pconnecttime = Sys_FloatTime() * 1000.0f;
		new_player.pspawned = ReadByte(cptr + client_t::pspawned);
		/*
		new_player.pedicts = ReadByte(cptr + client_t::pedicts);
		new_player.pnext_messageinterval = ReadByte(cptr + client_t::pnext_messageinterval);

		strcpy(new_player.name, (char *)utils->ExplodeAndReturn(userinfostring, 2, "\\").c_str());
		strcpy(new_player.playermodel, (char *)utils->ExplodeAndReturn(userinfostring, 4, "\\").c_str());
		*/
		new_player.pnext_heartbeat = 0;
		new_player.plast_heartbeat = 0;

		new_player.pinternaluserid = playerid; //The internal userid, not the one Nightfire uses.. We need this to loop through the players and get their respective tables..
		players.push_back(new_player);
		delete utils;
	}
	#if INIMOD_DEBUG
	printf("CBasePlayer::AddPlayerToTable(%d)\n", playerid);
	#endif
}
int CBasePlayer::GetConnectTime(int playerid) {
	std::vector<_PlayerInfo>::iterator it = players.begin();
	_PlayerInfo player;
	int i = 0;
	while(it != players.end()) {
		player = *(it++);
		if(playerid == player.pinternaluserid) {
			return (int)floor(((float)(Sys_FloatTime() * 1000.0f)- player.pconnecttime)/1000); //In seconds and round please..;, todo check / 1000
		}
	}
	return 0;
}
int CBasePlayer::GetNFIDFromInternalID(int internalid) { //This allows us to get their NF ID by knowing their internal ID.
	for (std::vector<_PlayerInfo>::iterator it = players.begin(); it != players.end(); it++) {
		if(it->pinternaluserid == internalid) {
			return it->puserid;
		}
	}
	return INVALID_USERID;
}
int CBasePlayer::GetInternalIDFromNFID(int nfuserid) { //This allows us to get their Internal ID by knowing their NF ID, if they're on the table ofc.
	for (std::vector<_PlayerInfo>::iterator it = players.begin(); it != players.end(); it++) {
		if(it->puserid == nfuserid) {
			return it->pinternaluserid;
		}
	}
	return INVALID_USERID;
}
int CBasePlayer::GetFreeIndex() {
	int i = 0; //This was 0 but nf id's often start from 1 so..
	for (std::vector<_PlayerInfo>::iterator it = players.begin(); it != players.end(); it++) {
		if(it->puserid == 0) {
			return i;
		}
		i++;
	}
	return 0;
}
int CBasePlayer::GetLastBeat(int playerid) {
	for (std::vector<_PlayerInfo>::iterator it = players.begin(); it != players.end(); it++) {
		if(it->pinternaluserid == playerid) {
			return it->plast_heartbeat;
		}
	}
	return 0;
}
int CBasePlayer::GetNextBeat(int playerid) {
	std::vector<_PlayerInfo>::iterator it = players.begin();
	_PlayerInfo player;
	int i = 0;
	while(it != players.end()) {
		player = *(it++);
		if(playerid == player.pinternaluserid) {
			return player.pnext_heartbeat;
		}
	}
	return 0;
}
int CBasePlayer::NumClientsOnTable() {
	std::vector<_PlayerInfo>::iterator it = players.begin();
	_PlayerInfo player;
	int i = 0;
	while(it != players.end()) {
		player = *(it++);
		i++;
	}
	return i;
}
bool CBasePlayer::IsAClient(int playerid) {
	return !(bool)ReadByte((getPointerForPlayerID(playerid) + client_t::pfakeclient));
}
string CBasePlayer::FetchInfoForPlayerID( int playerid ) {
	int cptr = 0;
	cptr = getPointerForPlayerID(playerid);
	return ReadCharArray(cptr + client_t::puserinfo, 128);
}
void CBasePlayer::ClientCommand(char *text, int edictptr ){
	DWORD adr = CLIENT_COMMAND;
	__asm {
		push text					// text
		push edictptr				// player pointer
		call adr					// call the func
		add esp, 8					// NOTES: clientcmd requires ; at the end of every command
	}

}
void CBasePlayer::GiveNamedItem(int playerid, char *wepname, int iSubType) {
	int cptr = 0;
	cptr = getPointerForPlayerID(playerid);
	if(cptr == 0)
		return;
	int xadr = ReadInt32(cptr + client_t::pedicts);
	xadr = ReadInt32(xadr + edict::pentity);
	if(xadr == 0)
		return;
	DWORD adr = ADR_GIVENAMEDITEM;
	__asm {
		push iSubType //subtype
		mov ecx, xadr
		push wepname
		call adr
		add esp, 4
	}
}
void CBasePlayer::RemoveAllItems(int playerid, int dukes) {
	int cptr = 0;
	cptr = getPointerForPlayerID(playerid);
	if(cptr == 0)
		return;
	int xadr = ReadInt32(cptr + client_t::pedicts);
	xadr = ReadInt32(xadr + edict::pentity);
	if(xadr == 0)
		return;
	DWORD adr = ADR_WEAPONSTRIP;
	__asm {
		push dukes //Wanna keep the dukes or not? 1 removes them, 0 keeps them
		push dukes //Don't know what this one does
		mov ecx, xadr
		call adr
	}
}
void CBasePlayer::Weapon_SetLast(int playerid, char *wepname) {
	int cptr = 0;
	cptr = getPointerForPlayerID(playerid);
	if(cptr == 0)
		return;
	int xadr = ReadInt32(cptr + client_t::pedicts);
	xadr = ReadInt32(xadr + edict::pentity);
	if(xadr == 0)
		return;
	DWORD adr = ADR_WEAPON_SETLAST;
	__asm {
		push wepname
		mov ecx, xadr
		call adr
		add esp, 4
	}
}
void CBasePlayer::SetPlayerModel(int playerid, char *mdlname) { //TODO FIX: CRASHES
	int cptr = 0;
	cptr = getPointerForPlayerID(playerid);
	if(cptr == 0)
		return;
	int xadr = ReadInt32(cptr + client_t::pedicts);
	int xadr2 = xadr;
	xadr = ReadInt32(xadr + edict::pentity);
	int xadr3 = xadr;
	xadr = ReadInt32(xadr + player::punknown4);
	if(xadr == 0)
		return;
	DWORD adr = ADR_SETMODEL;
	__asm {
		//It's written like this in NF
		mov eax, 1
		mov ecx, xadr
		mov edx, xadr2
		mov esi, xadr3
		push mdlname
		push edx
		call adr
		add esp, 4
	}
}
void CBasePlayer::SpectatePlayer(int playerid, int tospectate) { //Switches to the respective entity without shutting down the server but doesn't switch views
	engineModule *engine = new engineModule;
	CBaseEntity *entity = new CBaseEntity;

	int cptr = 0, targetedict;

	tospectate = GetInternalIDFromNFID(tospectate);
	if ( tospectate == INVALID_USERID )	{
			//engine->ClientPrint("ERROR: Invalid spectator userid!\n", 3, GetEdictNum(playerid));
			engine->CLIENT_PRINTF("ERROR: Invalid spectator userid!\n", 1, GetEdictNum(playerid));
			return;
	}

	//player that sent the command
	cptr = getPointerForPlayerID(playerid);
	if(cptr == 0) return;
	int sourceplayeredict = ReadInt32(cptr + client_t::pedicts);
	int sourceplayerentptr = entity->Instance(sourceplayeredict);
	int sourceplayerindex = engine->IndexOfEdict(sourceplayeredict);

	/*
	//Delete from here
	float originx[3], anglesx[3];

	GetPlayerPos(playerid, originx[0], originx[1], originx[2]);

	originx[2] -= 30;
	anglesx[0] = 90 * 10;
	anglesx[1] = 90 * 10;
	anglesx[2] = 90 * 10;
	int newmodelindex = engine->Precache(TYPE_MDL, "models/w_kowloon.mdl"); //this can only be done during mapload so it has no realtime effect

	int newedictx = engine->CreateEntity("cycler_sprite", (int)originx, (int)anglesx, sourceplayeredict);
	if ( newedictx != 0 ) newedictx = engine->EDICT_NUM(newedictx);
	targetedict = newedictx;

	engine->SetModel("models/w_kowloon.mdl", newedictx); //also sets model index
	return;
	//End of delete
	*/
	
	//player you want to spectate
	cptr = getPointerForPlayerID(tospectate);
	if(cptr == 0) return;
	int spectatethisedict = ReadInt32(cptr + client_t::pedicts);
	targetedict = spectatethisedict;

	if(!targetedict)
		return;

	int spectatethisentptr = entity->Instance(spectatethisedict);
	
	if(entity->CountEntitiesByOwner(sourceplayeredict, "cycler_sprite", 0) > 0) {
		entity->RemoveEntitiesByOwnerAndName(sourceplayeredict, "cycler_sprite"); //Remove all the entities they own, containing "cycler_sprite"
	}

	if ( sourceplayeredict != spectatethisedict ) {
		//START OF CREATING ENTITY
		//int angles, origin;
		//float originx[3], anglesx[3];
		if ( spectatethisentptr == 0) return;
		int unknownptr = ReadInt32(spectatethisentptr + 0x4);
		//spectatethisedict = engine->EDICT_NUM(spectatethisentptr); //this can get the edict ptr from the entptr, but we already have it

		/*
		//This code is pretty neat, can be used to get an edicts pos
		__asm mov eax, unknownptr
		__asm mov ecx, spectatethisedict
		__asm lea ecx, [eax+50]
		__asm mov angles, ecx
		__asm add eax, 0x8
		__asm mov origin, eax
		*/
		//No bullets up the crotch anymore
		float origin[3], angles[3];

		GetPlayerPos(tospectate, origin[0], origin[1], origin[2]);

		origin[2] += 60; //This should be the player's head
		angles[0] = 90 * 10;
		angles[1] = 90 * 10;
		angles[2] = 90 * 10;
		//This won't work because we can't set our targetname, but will work on those that has a targetname
		/*

		char thistargetname[32] = "spectator_camera_";
		char* id = itoa(sourceplayerindex, thistargetname + 17, 10);
		int oldedict = engine->FindEntityByX(thistargetname, "targetname", 0);
		if ( engine->FNullEnt(oldedict))
			engine->SUB_RemoveEx(engine->GetEntityPointer(oldedict)); //remove the already existing entity first
		*/

		int newmodelindex = engine->Precache(TYPE_MDL, "models/shell.mdl"); //this can only be done during mapload so it has no realtime effect

		//int newedict = entity->CreateEntityEx("cycler_sprite", (int)origin, (int)angles, spectatethisedict);
		int newedict = entity->CreateSpectatorEntity("cycler_sprite", (int)origin, (int)angles, spectatethisedict, sourceplayeredict); //Create a spectator entity, that is attached to spectatethisedict
		if ( newedict != 0 ) newedict = engine->EDICT_NUM(newedict);
		targetedict = newedict;

		engine->SetModel("models/shell.mdl", newedict); //also sets model index
		//newedict = engine->FindEntityByX("iamsolid", "targetname", 0);

		int newentptr = entity->Instance(newedict);
		if ( newentptr == 0) return;

		int unknownptr_new = ReadInt32(newentptr + 0x4);

		int movetype = ReadInt32(unknownptr_new + 0x108);
		WriteInt32((unknownptr_new + 0x108), MOVETYPE_FOLLOW ); //set the entity's movetype
		//engine->SetMoveType(ptr, MOVETYPE_FOLLOW);
		int solidtype = ReadInt32(unknownptr_new + 0x10C);
		WriteInt32((unknownptr_new + 0x10C), SOLID_NOT); //must set not solid to our new entity or else it crashes
		//engine->SetSolidType(ptr, SOLID_NOT);

		//int newindexofedict = engine->IndexOfEdict(targetedict); //this is from CSprite::Precache, it's extremely difficult to comprehend, and doesn't seem to work
		//WriteInt32(unknownptr_new + 0x110, newindexofedict); //m_hAttachedToEntity
		//WriteInt32(unknownptr_new+0x114, 0); //m_nAttachment

		int aiment = ReadInt32(unknownptr_new + 0x194);
		WriteInt32((unknownptr_new + 0x194), spectatethisedict); //set the entity to follow the target player
		//engine->EntitySetFollowEntity(edict);
		int drawflags = ReadInt32(unknownptr_new + 0x118);
		//engine->GetDrawFlags(edict);
		int modelindex = ReadInt32(unknownptr_new + 0x0B4); //modelindex is already set by SetModel!
		//engine->GetModelIndex(edict);
		int owner = ReadInt32(unknownptr_new + 0x198); //owner is already set when we created the entity
	
	
		//This works, but only if the entity has always had a targetname, animation, etc. otherwise the value is always 0x00
		/* 
		int adr_animation, adr_modelname, adr_target, adr_targetname;
		__asm {
			mov esi, newentptr
			mov eax, dword ptr ds:[0x4217a168]
			mov eax, dword ptr ds:[eax+0x9c]
			mov edx, dword ptr ds:[esi+0x0fc]
			mov ecx, dword ptr ds:[esi+0x4]
			add edx, eax
			mov adr_animation, edx
			mov edx, dword ptr ds:[ecx+0x0b8]
			mov ecx, dword ptr ds:[ecx+0x1c8]
			add edx, eax
			mov adr_modelname, edx
			add ecx, eax
			mov adr_targetname, ecx
		}
		std::string animation = ReadCharArray(adr_animation, 48);
		std::string targetname = ReadCharArray(adr_targetname, 48);
		std::string model = ReadCharArray(adr_modelname, 48);
	*/
		int target = ReadInt32(unknownptr_new + 0x1C4);
		int targetname = ReadInt32(unknownptr_new + 0x1C8);
	
		//TODO: MISSING TARGET

		int spawnflags = ReadInt32(unknownptr_new + 0x1A0);
		//int touchtype = ReadInt32(unknownptr_new + 0x14); //FIXME

		//END OF CREATING ENTITY
		//int spectatethisedict = engine->EDICT_NUM(cptr);			    //use this for findentitybyclassname
		//int spectatethisedict = cptr;								    //use this for findentitybyx
		/*
		//equivalent of ENTINDEX(pEdict), actually it comes up with something other than IndexOfEdict. still useful, don't delete
		DWORD EDX;
		DWORD EAX;
		int indexofspectatethis;
		EDX = ReadInt32(0x4217A168);
		EAX = ReadInt32(EDX + 0x9C); //entity index
		indexofspectatethis = EAX + ReadInt32(spectatethisedict + 0x2EC);

		//equivalent of ENTINDEX(pEdict)
		int indexofsourceplayer;
		EDX = ReadInt32(0x4217A168);
		EAX = ReadInt32(EDX + 0x9C); //entity index
		indexofsourceplayer = EAX + ReadInt32(sourceplayeredict + 0x2EC); //1c104d10
		*/

		if(spectatethisedict == 0) return;

		/*	
		//Set the angle of the player to the angle of the entity you are spectating
		float localanglex = memGetFloat(spectatethisedict + 0x178);
		float localangley = memGetFloat(spectatethisedict + 0x174);
		float localanglez = memGetFloat(spectatethisedict + 0x170);
	
		//We don't need to write angles
		MESSAGE_BEGIN(sourceplayeredict, 0, UserMessageBegin(MSG_ONE, "SetAngles"), MSG_ONE); //0x73 for GUI messages, is 0x75 used for centered screen messages?
			Vector ang (localanglex, localangley, localanglez);
			WRITE_ANGLE( ang );
		MESSAGE_END();
		*/
	}
	DWORD SetView = ADR_SetView;
	__asm {
		push targetedict
		push sourceplayeredict
		call SetView
		add esp, 8
	}
	 //this will set the view offset of the player
	/*int sourceplayer_unknownptr = ReadInt32(sourceplayerentptr + 0x4)
	WriteInProcessFloat(sourceplayer_unknownptr + 0x174, 0); //view_ofs x
	WriteInProcessFloat(sourceplayer_unknownptr + 0x178, 0); //view_ofs y
	WriteInProcessFloat(sourceplayer_unknownptr + 0x17C, 500.0f); //view_ofs z
	*/
	//GameInterface *gui = new GameInterface;
	//gui->SetView(playerid, tospectate);
	//delete gui;
	delete engine;
	delete entity;
}
string CBasePlayer::GetClientName(int playerid) {
	int cptr = 0;
	cptr = getPointerForPlayerID(playerid);
	string name;
	if(cptr == 0)
		return 0;

	//strcpy(name, ReadCharArray(cptr + client_t::pname, MAX_PLAYER_NAME).c_str());
	name = ReadCharArray(cptr + client_t::pname, MAX_PLAYER_NAME);
	return name;
}
float CBasePlayer::GetPlayerPos(int playerid, float &X, float &Y, float &Z) {
	int cptr = 0;
	cptr = getPointerForPlayerID(playerid);
	if(!HasClientSpawned(playerid)) 
		return 0;

	int adr = ReadInt32(cptr + client_t::pedicts);
	X = ReadFloat(adr + edict::pposX);
	Y = ReadFloat(adr + edict::pposY);
	Z = ReadFloat(adr + edict::pposZ);
	return 0.0;
}
void CBasePlayer::SetPlayerPos(int playerid, float X, float Y, float Z) {
	int cptr = 0;
	cptr = getPointerForPlayerID(playerid);
	if(!HasClientSpawned(playerid)) 
		return;

	int adr = ReadInt32(cptr + client_t::pedicts);
	WriteInProcessFloat((adr + edict::pposX), float(X));
	WriteInProcessFloat((adr + edict::pposY), float(Y));
	WriteInProcessFloat((adr + edict::pposZ), float(Z));
}
int CBasePlayer::GetPlayerUserID(int edictptr) {
	DWORD adr = ADR_GETUSERID;
	int userid;
	__asm {
		push edictptr
		call adr
		mov userid, eax
		add esp, 0x4
	}
	return userid;
}
void CBasePlayer::SetPlayerTeam(int playerid, int team) {
	int cptr, playeridptr;
	cptr = getPointerForPlayerID(playerid);
	playeridptr = ReadInt32(cptr + client_t::pedicts);
	//Needs assembly..
	/*
	MESSAGE_BEGIN(playeridptr, 0, UserMessageBegin(playeridptr, MSG_ONE, -1, "ExplodeHud"), MSG_ONE); //0x73 for GUI messages, is 0x75 used for centered screen messages?
		WRITE_BYTE( team );
	MESSAGE_END();
	*/
}
void CBasePlayer::OnPlayerHudInit(int playerid) {
	printf("OnPlayerHudInit(%d) has been called.\n", playerid);
	GameInterface *gui = new GameInterface;
	gui->SendMOTDToClient(playerid);
	gui->ResetSetView(playerid);
	delete gui;
}
string CBasePlayer::GetUserInfoStringValue(const char *key, int edictptr){
	char stringarray[64] = {0};
	if(edictptr == 0)
		return stringarray;

	int adr = ADR_GETINFOKEYBUFFER;
	int adr2 = ADR_INFOKEYVALUE;
	int mem;
	__asm{
		push edictptr
		call adr
		push key
		push eax
		call adr2
		add esp, 0xC
		mov mem, eax
	}

	for(int i = 0; memGetByte(mem) != 0x0; i++) {
		if(stringarray[i] == '\0') {
			stringarray[i] = memGetByte(mem);
			mem++;
		}
	}
	return stringarray;
}
int CBasePlayer::GetClientPtr(int edictptr) {
	engineModule *engine = new engineModule;
	size_t entnum = engine->NUM_FOR_EDICT_FN( edictptr ); //This was int entnum but it was giving a warning, answer from here: http://stackoverflow.com/questions/8188401/c-warning-c4018-signed-unsigned-mismatch
	delete engine;
	if (entnum < 1 || entnum > ReadInt32(SVS_MAXCLIENTS)) {
		printf("WARNING: tried to GetClientPtr a non-client!\n");
		return 0;
	}
	__asm {
		MOV EAX, entnum
		MOV ECX,DWORD PTR DS:[0x44A86B4C] //get first clientptr
		IMUL EAX,EAX,0x4F18 //multiply entnum by size of cell
		LEA EAX,[ECX+EAX+0xFFFFB0E8] //1c100040
		//LEA ESI,[EAX+0x64] //1c1000a4 keep this in case it's needed later
		MOV entnum, EAX //according to the sdk, we should be returning esi
	}
	return entnum;
}
int CBasePlayer::DisconnectClient(int playerid, const char* szReason) {
	DWORD adr = ADR_DROPCLIENT_FN;
	engineModule *engine = new engineModule;
	CBaseEntity *entity = new CBaseEntity;
	CNetwork *cNetwork = new CNetwork;
	int cptr, edptr;
	cptr = getPointerForPlayerID(playerid);
	if(!cptr) return 0;
	
	edptr = ReadInt32(cptr + client_t::pedicts);
	cptr = GetClientPtr(edptr);
	if(IsClientConnected(playerid)) {
		if(!strlen(szReason)) {
			szReason = "Server dropped the connection.";
		}
		if(IsAClient(playerid)) { //Fake clients cannot be kicked duh..
			MESSAGE_BEGIN(edptr, 0, SVC_DISCONNECT, MSG_ONE);
				WRITE_STRING( (char *)szReason );
			MESSAGE_END();
		}
	}
	cNetwork->OnClientDisconnect(playerid, REASON_DISCONNECT);
	delete engine;
	delete entity;
	delete cNetwork;
	return 1;
}
int CBasePlayer::GetFlags(int playerid) {
	CBaseEntity *entity = new CBaseEntity;
	int newentptr = 0;
	int cptr = getPointerForPlayerID(playerid);
	if(!cptr) return 0;
	int edict = ReadInt32(cptr + client_t::pedicts);
	if(!edict) return 0;
	newentptr = entity->Instance(edict) != 0 ? ReadInt32(entity->Instance(edict)+0x4) : 0;
	if(!newentptr) return 0;
	delete entity;
	if(!newentptr) return 0;
	return ReadInt32(newentptr+0x1a4);
}
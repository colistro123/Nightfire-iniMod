#include "player.h"
#include "gameinterface.h"
#include "engine.h"
#include "utils.h"
#include "gamemodes.h"

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
int		CBasePlayer::cptr = 0;
int		CBasePlayer::edictptr = 0;
int		CBasePlayer::ptr = 0;
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

void CBasePlayer::setPointerForPlayerID( int playerid ) {
	//playerid = GetNFIDFromInternalID(playerid); //Fixed - Not really... So do ID's in Nightfire start from 0 or 1? What the.. Tripping colors...
	//playerid == 0 ? 1 : playerid; //If the playerid is 0, then make it 1, otherwise make it the actual playerid (the sent playerid). (I'm doing this because NF's playerid's start from 1 instead of 0..)
	ptr = ReadInt32(0x44A86B4C);
	cptr = ptr + (PLAYER_TABLE_CELLSIZE * playerid);
	memSetPtr(cptr);
}
void CBasePlayer::SetPlayerHealth(int playerid, int percent) {
	setPointerForPlayerID(playerid);
	if(!HasClientSpawned(playerid)) 
		return;

	int adr = ReadInt32(cptr + client_t::pedicts);
	adr = ReadInt32(adr + edict::pentity);
	adr = ReadInt32(adr + player::punknown4);
	WriteInProcessFloat(adr + player::phealth , float(percent * 2));
}
float CBasePlayer::GetPlayerHealth(int playerid) {
	setPointerForPlayerID(playerid);
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
	//Scripting call (Gamemode)
	CNetwork *cNetwork = new CNetwork;
	cNetwork->OnClientSpawn(playerid);
	delete cNetwork;
	return 1;
}
int CBasePlayer::IsClientConnected( int playerid ) { //This lets us know if the client is connected, but this is straight from the engine. I'll use this to update the list
	setPointerForPlayerID(playerid);
	return memGetInt(client_t::pconnected) == 1 ? 1 : 0;
}
int CBasePlayer::HasClientSpawned( int playerid ) { //This lets us know if the client has spawned or not, used upon health checks
	setPointerForPlayerID(playerid);
	return memGetInt(client_t::pspawned) ? 1 : 0;
}
int CBasePlayer::GetPlayerDeaths( int playerid ) {
	setPointerForPlayerID(playerid);
	if(!HasClientSpawned(playerid))
		return 0;

	int adr = ReadInt32(cptr + client_t::pedicts);
	adr = ReadInt32(adr + edict::pentity);
	return ReadInt32(adr + player::pdeaths);
}
float CBasePlayer::GetPlayerKills( int playerid ) {
	setPointerForPlayerID(playerid);
	if(!HasClientSpawned(playerid))
		return 0;

	int adr = ReadInt32(cptr + client_t::pedicts);
	return ReadFloat(adr + edict::pkills); //Return a round value
}
void CBasePlayer::UpdateClients( void ) {
	ptr = ReadInt32(0x44A86B4C);
	curtime = timeGetTime();
	//CNetwork *cNetwork = new CNetwork;
	if ( curtime - lasttick > 10.0f ){ //sanity check
		lasttick = curtime;
		if(NumClientsOnTable() < 1 && NumClients() < 1) return; //Don't do anything.. (Optimization)
		for(int playerid=0; playerid<MaxClients(); playerid++) { //Actually, you were right, it's better to just parse through all the slots (What if we miss one id?, NumClients only goes from 1 to Max Clients Connected, making us skip certain id's since the player id indexes always go up by 1 so a player for example, could have ID 7 but the number of clients being 5, making 7 unreachable in a for loop for example..)
			UpdateHeartBeat(playerid); //Updates the heartbeat for that specific client to later on check if they've disconnected directly from the server
			//cNetwork->OnPreClientConnect(playerid); //Call the function to check if they've connected
		}
		//cNetwork->OnPreClientDisconnect(); //Call the function to check if they've disconnected
		//delete cNetwork;
	}
	currentclient = 0;
}
void CBasePlayer::ShowConnectedClients( void ) { //Just display our updated table
	char cmsg[128];
	std::vector<_PlayerInfo>::iterator it = players.begin();
	_PlayerInfo player;
	Utils *utils = new Utils();
	//utils->buffer[0] = 0;
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
			sprintf(cmsg,"ID: %d Name: %s Score: %d Ping: %d Connected: %s Health: %.2f Kills: %.1f, Deaths: %d\n",i,player.name,player.score,player.ping,IsClientConnected(i) ? "Yes" : "No",GetPlayerHealth(i), GetPlayerKills(i), GetPlayerDeaths(i));
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
/*
		engine->FetchPlayerTableInfo();
		engine->getGameVer(currentclient);
		engine->ClientCommand("echo Install the latest patch!;cl_disconnect;");
*/
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
	std::vector<_PlayerInfo>::iterator it = players.begin();
	//_PlayerInfo player;
	int i = 0;
	while(it != players.end()) {
		//player = *(it++);
		struct _PlayerInfo player = *it;
		if(i++ == playerid) { //player.puserid
			players.erase(it);
			return;
		}
	}
}
int CBasePlayer::IsClientOnTable( int playerid ) {
	std::vector<_PlayerInfo>::iterator it = players.begin();
	_PlayerInfo player;
	int i = 0;
	while(it != players.end()) {
		player = *(it++);
		if(i++ == playerid) {
			return 1;
		}
	}
	return 0;
}
void CBasePlayer::UpdateHeartBeat( int playerid ) {
	setPointerForPlayerID(playerid);
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
	setPointerForPlayerID(playerid);
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

	setPointerForPlayerID(playerid);
	int clientid = memGetByte(client_t::puserid);
	
	if(!IsClientOnTableByNFID(clientid)) {
		if(!IsClientConnected(playerid)) //If the player is not connected..
			return;
		
		Utils *utils = new Utils();
		new_player.score = score;
		new_player.ping = ping;

		userinfostring = memGetCharArray(client_t::puserinfo);
		new_player.plagcompensation = memGetByte(client_t::plagcompensation);
		new_player.pfakeclient = memGetByte(client_t::pfakeclient);

		new_player.puserinfo = userinfostring;

		new_player.connected = memGetByte(client_t::pconnected); //The client is now connected, put this on our table
		new_player.puserid = memGetByte(client_t::puserid);
		new_player.pconnecttime = timeGetTime();
		new_player.pspawned = memGetByte(client_t::pspawned);
		new_player.pedicts = memGetByte(client_t::pedicts);
		new_player.pnext_messageinterval = memGetByte(client_t::pnext_messageinterval);

		strcpy(new_player.name, (char *)utils->ExplodeAndReturn(userinfostring, 2, "\\").c_str());
		strcpy(new_player.playermodel, (char *)utils->ExplodeAndReturn(userinfostring, 4, "\\").c_str());

		new_player.pnext_heartbeat = 0;
		new_player.plast_heartbeat = 0;

		new_player.pinternaluserid = playerid; //The internal userid, not the one Nightfire uses.. We need this to loop through the players and get their respective tables..
		players.push_back(new_player);
		delete utils;
	}
}
int CBasePlayer::GetConnectTime(int playerid) {
	std::vector<_PlayerInfo>::iterator it = players.begin();
	_PlayerInfo player;
	int i = 0;
	while(it != players.end()) {
		player = *(it++);
		if(playerid == player.pinternaluserid) {
			return (int)floor(((float)timeGetTime() - player.pconnecttime)/1000); //In seconds and round please..
		}
	}
	return 0;
}
int CBasePlayer::GetNFIDFromInternalID(int internalid) { //This allows us to get their NF ID by knowing their internal ID.
	std::vector<_PlayerInfo>::iterator it = players.begin();
	_PlayerInfo player;
	int i = 1; //This was 0 but nf id's often start from 1 so..
	while(it != players.end()) {
		player = *(it++);
		if(i++ == internalid) {
			return player.puserid;
		}
	}
	return 0;
}
int CBasePlayer::GetInternalIDFromNFID(int nfuserid) { //This allows us to get their Internal ID by knowing their NF ID, if they're on the table ofc.
	std::vector<_PlayerInfo>::iterator it = players.begin();
	_PlayerInfo player;
	while(it != players.end()) {
		player = *(it++);
		if(player.puserid == nfuserid) {
			return player.pinternaluserid;
		}
	}
	return 0;
}
int CBasePlayer::GetFreeIndex() { //Finds a free index
	std::vector<_PlayerInfo>::iterator it = players.begin();
	_PlayerInfo player;
	int i = 0; //This was 0 but nf id's often start from 1 so..
	while(it != players.end()) {
		player = *(it++);
		if(player.puserid == 0) {
			return i;
		}
		i++;
	}
	return 0;
}
int CBasePlayer::GetLastBeat(int playerid) {
	std::vector<_PlayerInfo>::iterator it = players.begin();
	_PlayerInfo player;
	int i = 0;
	while(it != players.end()) {
		player = *(it++);
		if(playerid == player.pinternaluserid) {
			return player.plast_heartbeat;
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
int CBasePlayer::IsAClient(int playerid) {
	setPointerForPlayerID(playerid);
	return memGetByte(client_t::pfakeclient) ? 0 : 1;
}
string CBasePlayer::FetchInfoForPlayerID( int playerid ) {
	setPointerForPlayerID(playerid);
	return memGetCharArray(client_t::puserinfo);
}
void CBasePlayer::ClientCommand(char *text ){
	/*
	if ( cptr == 0 ){
		return;
	}else{
		memSetPtr(cptr);
		memOpen(19320);
		if ( memGetPtr() == 0 ){
			memClose();
			return;
		}
	}
	memClose();
	*/
	if ( IsFakeClient )
		return;
	if ( cptr == 0 )    //let's do it the crappy way
		return;
	edictptr = ReadInt32(cptr + client_t::pedicts);
	if ( edictptr == 0 )
		return;
	DWORD adr = CLIENT_COMMAND;
	__asm {
    push text					// text
	push edictptr				// player pointer
	call adr	// call the func
	add esp, 8					// NOTES: clientcmd requires ; at the end of every command
	}
}
void CBasePlayer::GiveNamedItem(int playerid, char *wepname, int iSubType) {
	setPointerForPlayerID(playerid);
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
	setPointerForPlayerID(playerid);
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
	setPointerForPlayerID(playerid);
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
	setPointerForPlayerID(playerid);
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
	//int tempedict = engine->FindEntityByClassname(0, "weapon_raptor"); //func_train
	setPointerForPlayerID(tospectate);
	if(cptr == 0)
		return;
	
	int xadr = ReadInt32(cptr + client_t::pedicts);
	int xadr2 = xadr;
	delete engine;

	setPointerForPlayerID(playerid);
	if(cptr == 0)
		return;
	xadr = ReadInt32(cptr + client_t::pedicts);
	int xadr3 = ReadInt32(xadr + edict::pentity);
	if(xadr == 0)
		return;

	DWORD adr = ADR_SetView;
	__asm {
		/*
		mov eax, xadr //player edict 1c1a6820
		mov ecx, xadr2 //edict to watch
		mov edi, xadr3 //1098dee0
		*/
		//mov esi, xadr2 //entity to watch
		mov eax, xadr2
		mov ecx, xadr
		mov edi, xadr3
		mov esi, xadr
		push eax
		push ecx
		call adr
		add esp, 8
	}
}
char* CBasePlayer::GetClientName(int playerid) {
	setPointerForPlayerID(playerid);
	char* name = new char();
	if(cptr == 0)
		return 0;
	strcpy(name, ReadCharArray(cptr + client_t::pname, MAX_PLAYER_NAME).c_str());
	return name;
}
float CBasePlayer::GetPlayerPos(int playerid, float &X, float &Y, float &Z) {
	setPointerForPlayerID(playerid);
	if(!HasClientSpawned(playerid)) 
		return 0;

	int adr = ReadInt32(cptr + client_t::pedicts);
	X = ReadFloat(adr + edict::pposX);
	Y = ReadFloat(adr + edict::pposY);
	Z = ReadFloat(adr + edict::pposZ);
	return 0.0;
}
void CBasePlayer::SetPlayerPos(int playerid, float X, float Y, float Z) {
	setPointerForPlayerID(playerid);
	if(!HasClientSpawned(playerid)) 
		return;

	int adr = ReadInt32(cptr + client_t::pedicts);
	WriteInProcessFloat(adr + edict::pposX, float(X));
	WriteInProcessFloat(adr + edict::pposY, float(Y));
	WriteInProcessFloat(adr + edict::pposZ, float(Z));
}

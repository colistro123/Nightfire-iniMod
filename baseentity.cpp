#include "baseentity.h"
#include "engine.h"
#include "utils.h"
#include "player.h"
#include <iostream>     // std::cout
#include <algorithm>    // std::count

std::vector<_BaseEntityInfo> baseentityinf;

bool CBaseEntity::IsItemOnBaseEntityTable(int edict) {
	for (std::vector<_BaseEntityInfo>::iterator it = baseentityinf.begin(); it != baseentityinf.end(); it++) {
		if(it->edictptr == edict) {
			return true;
		}
	}
	return false;
}
int CBaseEntity::GetBaseEntityTableIndex(int edict) {
	int i = 0;
	for (std::vector<_BaseEntityInfo>::iterator it = baseentityinf.begin(); it != baseentityinf.end(); it++) {
		if(it->edictptr == edict) {
			return i;
		}
		i++;
	}
	return -1;
}
void CBaseEntity::AddEntityToTable(int edict, const char* entityname, int owneredict) {
	_BaseEntityInfo baseentity;
	memset(&baseentity, 0, sizeof(BaseEntityInfo));
	
	Utils *utils = new Utils();
	baseentity.edictptr = edict;
	baseentity.owneredict = owneredict;
	sprintf(baseentity.entityname, "%s", entityname);

	baseentityinf.push_back(baseentity);
	delete utils;
}
void CBaseEntity::RemoveEntitiesByOwner( int owneredict ) { //Removes entities by the owner of it
	int i = 0;
	for (std::vector<_BaseEntityInfo>::iterator it = baseentityinf.begin(); it != baseentityinf.end(); ) {
		if(it->owneredict == owneredict) {
			StopFollowingEntity(it->edictptr);
			SUB_RemoveEx(it->edictptr);
			printf("CBaseEntity::RemoveEntitiesByOwner: Removed 0x%.8X edict from table with index %d!\n", it->edictptr, i);
			it = baseentityinf.erase(it);
		} else { 
		  ++it;
		}
		i++;
	}
}
void CBaseEntity::RemoveEntitiesByOwnerAndName( int owneredict, const char* szName ) { //Removes entities by the owner of it, including the name of the entity
	int i = 0;
	for (std::vector<_BaseEntityInfo>::iterator it = baseentityinf.begin(); it != baseentityinf.end();) {
		if(it->owneredict == owneredict) {
			if(CountEntitiesByOwner( owneredict, szName, false ) != 0) {
				StopFollowingEntity(it->edictptr);
				SUB_RemoveEx(it->edictptr);
				printf("CBaseEntity::RemoveEntitiesByOwnerAndName: Removed 0x%.8X edict from table with index %d!\n", it->edictptr, i);
				it = baseentityinf.erase(it);
			}
		} else {
			++it;
		}
		i++;
	}
}
void CBaseEntity::RemoveAllEntitiesFromTable( void ) {
	int i = 0;
	for (std::vector<_BaseEntityInfo>::iterator it = baseentityinf.begin(); it != baseentityinf.end(); it++) {
		StopFollowingEntity(it->edictptr);
		SUB_RemoveEx(it->edictptr);
		printf("CBaseEntity::RemoveAllEntitiesFromTable: Removed 0x%.8X edict from table with index %d!\n", it->edictptr, i);
		baseentityinf.erase(it);
		i++;
	}
}
void CBaseEntity::ShowEntityTable ( void ) {
	int i = 0;
	for (std::vector<_BaseEntityInfo>::iterator it = baseentityinf.begin(); it != baseentityinf.end(); it++) {
		printf("Index: %d Edict: 0x%.8X Owner: 0x%.8X\n", i, it->edictptr, it->owneredict);
		i++;
	}
}
void CBaseEntity::OnServerChangeMap( void ) {
	//We'll call this function from OnServerChangeMap
	RemoveAllEntitiesFromTable();
}
void CBaseEntity::EntitiesOnClientDisconnect(int playerid) {
	CBasePlayer *pPlayer = new CBasePlayer;
	int edictptr;

	edictptr = pPlayer->GetEdictNum(playerid);
	if(!edictptr)
		return;

	if(CountEntitiesByOwner(edictptr, "", true) != 0) {
		RemoveEntitiesByOwner(edictptr);
	}
	ShowEntityTable();
	delete pPlayer;
}
void CBaseEntity::RemoveEntityFromTable( int edict ) {
	int i = 0;
	for (std::vector<_BaseEntityInfo>::iterator it = baseentityinf.begin(); it != baseentityinf.end();) {
		if(it->edictptr == edict) {
			printf("CBaseEntity::RemoveEntityFromTable: Removed 0x%.8X edict from table with index %d!\n", edict, i);
			SUB_RemoveEx(edict);
			baseentityinf.erase(it);
		} else {
			++it;
		}
		i++;
	}
}
int CBaseEntity::FindRandomEntityPointer() {
	int entityptr = FindEntityByClassname(0, "info_player_start");
	if ( entityptr == 0 ) { entityptr = FindEntityByClassname(0, "game_player_equip"); }
    if ( entityptr == 0 ) { entityptr = FindEntityByClassname(0, "func_wall"); }
    if ( entityptr == 0 ) { entityptr = FindEntityByClassname(0, "func_breakable"); }
	if ( entityptr == 0 ) { entityptr = FindEntityByClassname(0, "func_button"); }
	if ( entityptr == 0 ) { entityptr = FindEntityByClassname(0, "item_breakable"); }
	if ( entityptr == 0 ) { entityptr = FindEntityByClassname(0, "item_grappletarget"); }
	if ( entityptr == 0 ) { entityptr = FindEntityByClassname(0, "env_sprite"); }
	if ( entityptr == 0 ) { entityptr = FindEntityByClassname(0, "trigger_multiple"); }
	if ( entityptr == 0 ) { entityptr = FindEntityByClassname(0, "trigger_once"); }
	if ( entityptr == 0 ) { entityptr = FindEntityByClassname(0, "func_ladder"); }
	if ( entityptr == 0 ) { entityptr = FindEntityByClassname(0, "weapon_dukes"); }
	//new string[128];
	//strformat(string, sizeof(string), false, "Entity Ptr: %d", entityptr);
    //print(string);
	return entityptr;
}
int CBaseEntity::CreateEntityEx(const char *szName, int Origin, int Angles, int owneredict) {
	int ptr = CreateEntity(szName, Origin, Angles, owneredict);
	AddEntityToTable(ptr, szName, owneredict); 
	return ptr;
}
int CBaseEntity::CreateSpectatorEntity(const char *szName, int Origin, int Angles, int owneredict, int spectatoredict) {
	engineModule *engine = new engineModule;
	int ptr = CreateEntity(szName, Origin, Angles, owneredict);
	if ( !ptr ){
		delete engine;
		return 0;
	}
	int edictptr = engine->EDICT_NUM(ptr);
	AddEntityToTable(edictptr, szName, spectatoredict); //Because the spectator is the "owner" of the entity attached to that specific player
	delete engine;
	return ptr;
}
int CBaseEntity::CountEntitiesByOwner( int owneredict, const char *szName, bool all ) { //Counts the amount of entities someone owns (if they're on the table)
	int i = 0;
	for (std::vector<_BaseEntityInfo>::iterator it = baseentityinf.begin(); it != baseentityinf.end(); it++) {
		if(it->owneredict == owneredict) {
			if(!all && strcmp(szName, it->entityname)) { //If we're searching for how much entities they have based on name
				i++; //Increment
				continue; //Continue to the next loop
			}
			i++; //We're searching for all, this will only be reached if all is set to 1
		}
	}
	return i;
}
void CBaseEntity::SUB_RemoveEx(int edictptr) {
	if(FNullEnt(edictptr) != 0) {
		UTIL_Remove(edictptr);
	}
}
void CBaseEntity::StopFollowingEntity(int edictptr) {
	int newentptr = Instance(edictptr);
	if ( newentptr == 0) return;
	newentptr = ReadInt32(newentptr + 0x4);
	WriteInt32((newentptr + 0x108), MOVETYPE_NONE ); //set the entity's movetype
	WriteInt32((newentptr + 0x10C), NULL); //solid flags
	WriteInt32((newentptr + 0x194), NULL); //aiment Follow
	WriteInt32((newentptr + 0x198), NULL); //Owner
}
/* -------------------------------------------------- Assembly / Game Engine Calls  -------------------------------------------------- */
int CBaseEntity::CreateEntity(const char *szName, int Origin, int Angles, int owneredict) {
	DWORD adr = ADR_CreateEntity;
	int newptr;
	__asm{
		push owneredict
		push Angles
		push Origin
		push szName
		call adr
		mov newptr, eax
		add esp, 0x10
	}
	return newptr;
}
int CBaseEntity::FindEntityByX(const char *szName, const char *classtype, int pStartEntity) { //classname, targetname, globalname, ?
	int edictptr = 0;
	DWORD adr = ADR_FindEntityByX;
	
	__asm push szName
	__asm push classtype
	__asm push pStartEntity
	__asm call adr
	__asm add esp, 0x0C
	__asm mov edictptr, eax
	
	return edictptr;
}
int CBaseEntity::FindEntityByClassname( int pStartEntity, const char *szName ) {
	int edictptr = 0;
	DWORD adr = ADR_FindEntityByClassname;
	__asm push szName
	__asm push pStartEntity
	__asm call adr
	__asm mov edictptr, eax
	__asm add esp, 0x8
	return edictptr;
}
void CBaseEntity::SUB_Remove(DWORD entityptr) {
	DWORD adr;
	adr = ADR_SUBREMOVE;
	__asm mov ecx, entityptr
	__asm call adr
	//__asm add esp, 4
}
void CBaseEntity::UTIL_Remove(DWORD edictptr) {
	DWORD adr;
	adr = ADR_UTIL_REMOVE;
	int entptr = Instance(edictptr);
	__asm {
		push entptr
		call adr
		add esp, 0x4
	}
}
int CBaseEntity::GetBaseEntity(int edictptr){
	int base = GET_BASE_ENTITY;
	__asm {
		push edictptr
		call base 
		mov base, eax
		add esp, 0x4
	}
	return base;
}
int CBaseEntity::Instance(int edictptr){
	//int inst;
	//__asm mov ebx, edictptr
	//__asm mov eax, dword ptr ds:[ebx + 0x11c]
	//__asm mov inst, eax
	if(!edictptr)
		return 0;

	return ReadInt32(edictptr + 0x11C); //retrieves entity pointer for this edict
}
int CBaseEntity::FNullEnt(int edictptr){ //Is this entity's edict null? returns 0 if null
	//MOV EAX,DWORD PTR SS:[entityptr]
	//SUB EAX,DWORD PTR DS:[44A7B9A4]
	int isnull = FNULLENT;
	__asm {
		push edictptr
		call isnull
		mov isnull, eax
		add esp, 0x4
	}
	return isnull;
}
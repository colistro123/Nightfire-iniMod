#ifndef BASEENTITY_H
#define BASEENTITY_H

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

//Precache table definition
typedef struct _BaseEntityInfo {
	int		edictptr;
	int		owneredict;
	char	entityname[MAX_STRING_LEN];
	//int id;
} BaseEntityInfo;

class CBaseEntity {
public:
	CBaseEntity() { };
	bool IsItemOnBaseEntityTable( int edict );
	void AddEntityToTable(int edict, const char* entityname, int owneredict);
	int GetBaseEntityTableIndex(int edict);
	void RemoveEntityFromTable( int edict ); //This will remove the entity from the map as well, if it exists on the table.
	int CreateEntity(const char *szName, const Vector &vecOrigin, const Vector &vecAngles, int owneredict); 
	int CreateEntity(const char *szName, int Origin, int Angles, int owneredict); 
	int FindEntityByX(const char *szName, const char *classtype, int pStartEntity);
	int FindEntityByClassname( int pStartEntity, const char *szName );
	int FindRandomEntityPointer();
	int CreateEntityEx(const char *szName, int Origin, int Angles, int owneredict); //Used for sending entities to the table after creation.
	int CreateSpectatorEntity(const char *szName, int Origin, int Angles, int owneredict, int spectatoredict);
	void RemoveEntitiesByOwner( int owneredict ); //Removes all the entities a player has (Once again, if they're on the table, which means that they have to be created via CreateEntityEx)
	int CountEntitiesByOwner( int owneredict, const char* szName, bool all ); //All to 0 searches by name and all to one doesn't give any priority to the name of the entity
	void SUB_Remove(DWORD entityptr);
	void RemoveEntitiesByOwnerAndName( int owneredict, const char* szName ); //Removes entities by owner and name, pretty self explanatory, you send the entity owner and the name of the entity.
	int GetBaseEntity(int edictptr);
	int GETENT(int edictptr) { return GetBaseEntity(edictptr); }
	int FNullEnt(int edictptr); //Is this entity's edict null?
	int Instance(int edictptr); //Gets Entity Pointer from Edict
	int GetEntityPointer(int edictptr) { return Instance(edictptr); }
	void SUB_RemoveEx(int edictptr); //This wrapper function only deletes an entity if it's not null, otherwise it crashes since it's trying to remove a null entity
	void OnServerChangeMap( void );
	void RemoveAllEntitiesFromTable( void );
	void EntitiesOnClientDisconnect(int playerid);
	void StopFollowingEntity(int edictptr);
	void ShowEntityTable ( void );
	void UTIL_Remove(DWORD edictptr);
	//Destructor
    virtual ~CBaseEntity() { };
	char* msg;
	char buffer[1024]; //For messages and such..
};
#endif
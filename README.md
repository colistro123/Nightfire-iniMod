<img align="centered" src="https://images.indiegogo.com/file_attachments/492549/files/20140408060902-open-source-icon.png?1396962542" alt="iniMod is open source software">

Nightfire-iniMod
================

iniMod is a modification for Nightfire which allows the creation of custom AMX gamemodes.

Native Functions:
================

```

native GetGameTimeLeft();
native TriggerHudMsg(playerid, text[], duration, towho);
native SpectatePlayer(playerid, tospectate);
native RemoveAllItems(playerid, dukes);
native GiveNamedItem(playerid, wepname[], iSubType);
native GetClientName(playerid, const dest[], len);
native GetPlayerPos(playerid, &Float:x, &Float:y, &Float:z);
native SetPlayerPos(playerid, Float:x, Float:y, Float:z);
native EmitAmbientSound(flags, Float:volume, charsample[], channel, edict);
native EmitSound(edict, channel, charsample[], Float:volume, Float:attenuation, fFlags, pitch);
native PrecacheSound(name[]);
native PrecacheModel(name[]);
native PrecacheGeneric(name[]);
native PrecacheEvent(name[]);
native PrecacheUnmodified(name[]);
native FindEntityByClassname(pStartEntity, szName[]);
native ConsoleCommand(text[]);
native EDICT_NUM(entity);

```

Public Functions:
================

```

forward OnServerChangeMap();
forward OnClientConnect(playerid);
forward OnClientDisconnect(playerid, reason);
forward OnGameModeInit();
forward OnGameModeExit();
forward ServerThink();
forward OnClientUpdate(playerid);
forward OnClientCommandText(playerid, cmdname[]);
forward OnClientSpawn(playerid);
forward OnClientDeath(attacker, receiver);

```

Usage:
================

In order to use iniMod you require to have Nightfire and the latest community patch available running on your server. Use Visual Studio to compile iniMod and place the DLL on your main server folder where Bond_ded.exe is located. 

Gamemodes: 
================

In order to compile the ".p" gamemodes to AMX, you require the Quincy IDE. The gamemode name is specified inside the iniMod.cfg file. After that, create a gamemodes folder in your server directory and drop the gamemode there.

iniMod.cfg
================

This file contains the gamemode name your server will be executing upon start up.

```

gamemode:nightfire //The gamemode name, no need to write ".amx" at the end of it, iniMod does that for you.
updaterate:100  //This isn't being used at the moment

```

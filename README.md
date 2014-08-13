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

Other notes:
================

* The code is indented properly but Github isn't wrapping it up properly, causing it to look unindented, just download the files and when you open it, it'll be indented properly.


Thanks to:
================

* 2GoldenBullets - Who wrote most of the assembly calls and provided me with all the addresses required.
* AMXMODX - Lots of useful information in there.
* OllyDBG - Great tool I utilized to reverse engineer the game.
* Valve - Useful HL1 code that helped me intercept some of the game functions. https://github.com/ValveSoftware/halflife
* Quake I - The functions are pretty similar to Nightfire with some differences.

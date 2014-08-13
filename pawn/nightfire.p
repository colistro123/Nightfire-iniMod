#include <nf_gamemode>

#pragma tabsize 0

main()
{
	print("\n----------------------------------\n");
	print("  NF Test Script\n");
	print("----------------------------------\n");
}

new soundsplayed = 0;
new votes[MAX_PLAYERS] = 0;
new spawntimes[MAX_PLAYERS] = 0;
new kills[MAX_PLAYERS] = 0;

/*
const a = 10
const ESoundsInfo: {
	SoundName = 0,
	ETimeLeft,
}
*/
/*
#define ESoundsInfo[
         //.code,
         .SoundName{64},
         //Float: .price
		 .time
     ]
//Sounds

new Sounds[14][ESoundsInfo] = {
	{"space/SFV_one.wav", 1},
	{"space/SFV_two.wav", 2},
	{"space/SFV_three.wav", 3},
	{"space/SFV_four.wav", 4},
	{"space/SFV_five.wav", 5},
	{"space/SFV_six.wav", 6},
	{"space/SFV_seven.wav", 7},
	{"space/SFV_eight.wav", 8},
	{"space/SFV_nine.wav", 9},
	{"space/SFV_ten.wav", 10},
	{"CTF/SFV_gen_07.wav", 300},
	{"CTF/SFV_gen_08.wav", 180},
	{"CTF/SFV_gen_10.wav", 60},
	{"CTF/SFV_gen_11.wav", 30}
}
*/
public OnServerChangeMap()
{
	soundsplayed = 0;
	PrecacheSound("CTF/SFV_gen_07.wav"); //5 minutes remaining
    PrecacheSound("CTF/SFV_gen_08.wav"); //3 minutes remaining
    PrecacheSound("CTF/SFV_gen_10.wav"); //1 minute remaining
    PrecacheSound("CTF/SFV_gen_11.wav"); //30 seconds remaining
    PrecacheSound("space/SFV_ten.wav");
    PrecacheSound("space/SFV_nine.wav");
    PrecacheSound("space/SFV_eight.wav");
    PrecacheSound("space/SFV_seven.wav");
    PrecacheSound("space/SFV_six.wav");
    PrecacheSound("space/SFV_five.wav");
    PrecacheSound("space/SFV_four.wav");
    PrecacheSound("space/SFV_three.wav");
    PrecacheSound("space/SFV_two.wav");
    PrecacheSound("space/SFV_one.wav");
	PrecacheSound("servant_girl/scream3.wav");
	PrecacheSound("servant_girl/scream1.wav");
	PrecacheSound("servant_girl/scream2.wav");
	PrecacheSound("evil_base/klaxon1noloop.wav");
	PrecacheSound("M/M_Bond_dies.wav");
	PrecacheSound("common/common.wav");
	PrecacheSound("common/stinger.wav");
	PrecacheSound("common/mission_success.wav");
	PrecacheSound("CTF/MI6_ctf.wav");
	PrecacheSound("yakuza/die10.wav");
	PrecacheSound("yakuza/die11.wav");
	PrecacheGeneric("enemy_helicopter");
	PrecacheGeneric("enemy_attackcopter");
	PrecacheGeneric("enemy_pathvan");
	PrecacheGeneric("enemy_drake");
	PrecacheGeneric("enemy_generic");
	PrecacheModel("models/dart_tip.mdl");
}

public OnClientConnect(playerid)
{
	new string[128], name[24];
	GetClientName(playerid, name, sizeof(name));
	strformat(string, sizeof(string), false, "%s(%d) has connected to the server", name, playerid);
	print(string);
	return 1;
}

public OnClientDisconnect(playerid, reason)
{
	votes[playerid] = 0; //Clear all the votes for this playerid on disconnect
	kills[playerid] = 0; //Reset the kills for the player that disconnected
	return 1;
}

public OnGameModeInit()
{
	print("The gamemode \"nightfire.amx\" has been loaded successfully!");
	new string[128];
	for(new i=0; i<12; i++){
		strformat(string, sizeof(string), false, "The number is %d", i);
		print(string);
	}
    new hour,minute,second;
	gettime(hour,minute,second);
	strformat(string, sizeof(string), false, "The current time is: %d:%d:%d", hour,minute,second);
	print(string);
	return 1;
}

public OnGameModeExit()
{
	print("The gamemode \"nightfire.amx\" has been unloaded successfully!");
	exit(0);
	return 1;
}

public OnClientUpdate(playerid) {
	//printf("OnClientUpdate has been called for %d", playerid);
	//TriggerHudMsg(playerid, "This server uses iniMod", 5000, 0);
	return 1;
}
new lasttick;
public ServerThink() {
	new gametimeleft = GetGameTimeLeft();
	new entityptr;
	if((tickcount()-lasttick) > 100) {
		lasttick = tickcount();
		entityptr = FindRandomEntityPointer();
		if ( entityptr != 0 ) { playDesiredSound(entityptr, gametimeleft); }
	}
	return 1;
}
FindRandomEntityPointer() {
	new entityptr;
	entityptr = FindEntityByClassname(0, "info_player_start");
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
	if ( entityptr != 0 ) { entityptr = EDICT_NUM(entityptr); }
	//new string[128];
	//strformat(string, sizeof(string), false, "Entity Ptr: %d", entityptr);
    //print(string);
	return entityptr;
}
playDesiredSound(edictptr, gametimeleft) {
	 //EmitAmbientSound = 	flags, float volume, sound, channel, edict
	/*
	for(new i=0; i<sizeof(Sounds); i++) {
		if(gametimeleft <= Sounds[i][time]) {
			EmitAmbientSound(0, 1.0, Sounds[i][SoundName], 6, edictptr);
		}
	}
	*/
	if ( gametimeleft <= 1 && soundsplayed == 13){
		soundsplayed = 14;
		EmitAmbientSound(0, 1.0, "space/SFV_one.wav", 6, edictptr);
	}
	if ( gametimeleft <= 2 && soundsplayed == 12){
		soundsplayed = 13;
		EmitAmbientSound(0, 1.0, "space/SFV_two.wav", 6, edictptr);
	}
	if ( gametimeleft <= 3 && soundsplayed == 11){
		soundsplayed = 12;
		EmitAmbientSound(0, 1.0, "space/SFV_three.wav", 6, edictptr);
	}
	if ( gametimeleft <= 4 && soundsplayed == 10){
		soundsplayed = 11;
		EmitAmbientSound(0, 1.0, "space/SFV_four.wav", 6, edictptr);
	}
	if ( gametimeleft <= 5 && soundsplayed == 9){
		soundsplayed = 10;
		EmitAmbientSound(0, 1.0, "space/SFV_five.wav", 6, edictptr);
	}
	if ( gametimeleft <= 6 && soundsplayed == 8){
		soundsplayed = 9;
		EmitAmbientSound(0, 1.0, "space/SFV_six.wav", 6, edictptr);
	}
	if ( gametimeleft <= 7 && soundsplayed == 7){
		soundsplayed = 8;
		EmitAmbientSound(0, 1.0, "space/SFV_seven.wav", 6, edictptr);
	}
	if ( gametimeleft <= 8 && soundsplayed == 6){
		soundsplayed = 7;
		EmitAmbientSound(0, 1.0, "space/SFV_eight.wav", 6, edictptr);
	}
	if ( gametimeleft <= 9 && soundsplayed == 5){
		soundsplayed = 6;
		EmitAmbientSound(0, 1.0, "space/SFV_nine.wav", 6, edictptr);
	}
	if ( gametimeleft <= 10 && soundsplayed == 4){
		soundsplayed = 5;
		EmitAmbientSound(0, 1.0, "space/SFV_ten.wav", 6, edictptr);
	}
	if ( gametimeleft <= 30 && soundsplayed == 3){
		soundsplayed = 4;
		EmitAmbientSound(0, 1.0, "CTF/SFV_gen_11.wav", 6, edictptr);
	}
	if ( gametimeleft <= 60 && soundsplayed == 2){
		soundsplayed = 3;
		EmitAmbientSound(0, 1.0, "CTF/SFV_gen_10.wav", 6, edictptr);
	}
	if ( gametimeleft <= 180 && soundsplayed == 1){
		soundsplayed = 2;
		EmitAmbientSound(0, 1.0, "CTF/SFV_gen_08.wav", 6, edictptr);
	}
	if ( gametimeleft <= 300 && soundsplayed == 0){
		soundsplayed = 1;
		EmitAmbientSound(0, 1.0, "CTF/SFV_gen_07.wav", 6, edictptr);
	}
}
public OnClientCommandText(playerid, cmdname[]) {
	new cmd[32][32];
	new msg[128], smsg[128];
	split(cmdname,cmd, ' ');
	if(!strcmp(cmd[0], "/triggerhudmsg")) { //Check if we got the right cmd
		new user;
		//Params
		if(strlen(cmd[1]) < 1 || strlen(cmd[2]) < 1) {
			TriggerHudMsg(playerid, "Wrong Syntax: /triggerhudmsg <id> <msg>", 5, 1);
			return 1;
		}
		split(cmdname, cmd, ' ');
  		user = strval(cmd[1]);
		strformat(smsg, sizeof(smsg), false, "%s", cmd[2]);
		strformat(msg, sizeof(msg), false, "PM from %d: %s", playerid, smsg);
		TriggerHudMsg(user, msg, 5, 1);
	} else if(!strcmp(cmd[0], "/vote")) {
		if(votes[playerid] != 0) {
			TriggerHudMsg(playerid, "You have already voted!", 5, 1);
			return 1;
		}
		votes[playerid]++;
		TriggerHudMsg(playerid, "Congratulations, you have voted on the map change!", 5, 1);
	} else if(!strcmp(cmd[0], "/spectate")) {
   		new user;
  		//Params
  		if(strlen(cmd[1]) < 1) {
  			TriggerHudMsg(playerid, "Wrong Syntax: /spectate <user>", 5, 1);
  			return 1;
  		}
  		split(cmdname, cmd, ' ');
    	user = strval(cmd[1]);
  		SpectatePlayer(playerid, user);
    } else if(!strcmp(cmd[0], "/setpos")) {
       		new user;
      		//Params
      		if(strlen(cmd[1]) < 1) {
      			TriggerHudMsg(playerid, "Wrong Syntax: /setpos <playerid> <x> <y> <z>", 5, 1);
      			return 1;
      		}
      		split(cmdname, cmd, ' ');
        	user = strval(cmd[1]);

      		SetPlayerPos(user, strfloat(cmd[2]), strfloat(cmd[3]), strfloat(cmd[4]));
        } else {
		strformat(msg, sizeof(msg), false, "Command \"%s\" not found.", cmd[0]);
		TriggerHudMsg(playerid, msg, 5, 1);
	}
	return 1;
}
public OnClientSpawn(playerid) {
	new msg[128], name[24];
	new Float: X = 1.0, Float: Y = 1.0, Float: Z = 1.0;
	GetPlayerPos(playerid, X, Y, Z);
	strformat(msg, sizeof(msg), false, "Spawned at: X: %f, Y: %f, Z: %f", X, Y, Z);
	print(msg);
	if(++spawntimes[playerid] <= 1) {
		GetClientName(playerid, name, sizeof(name));
		strformat(msg, sizeof(msg), false, "Welcome %s, this server uses iniMod.", name);
		TriggerHudMsg(playerid, msg, 10, 1);
	}
	return 1;
}
public OnClientDeath(attacker, receiver) {
	new msg[128];
	thinkKills(attacker, receiver);
	strformat(msg, sizeof(msg), false, "ID %d has killed you (%d).", attacker, receiver);
	TriggerHudMsg(receiver, msg, 2, 1);
}
thinkKills(attacker, receiver) {
	#pragma unused receiver
	new msg[128], name[24];
	kills[attacker]++;
	new leader = getKillsLeader();
	GetClientName(leader, name, sizeof(name));
	if(attacker == leader) {
		strformat(msg, sizeof(msg), false, "%s is dominating everyone with %d kills.", name, kills[leader]);
   		TriggerHudMsg(leader, msg, 2, MSG_BROADCAST);
	}
}
getKillsLeader() {
	new highest,highestid = -1;
	for(new i=0; i<MAX_PLAYERS; i++) {
		if(kills[i] != 0) {
			if(kills[i] > highest) {
				highest = kills[i];
				highestid = i;
			}
		}
	}
	return highestid;
}
stock split(const strsrc[], strdest[][], delimiter = '|')
{
	new i, li, aNum, len, srclen = strlen(strsrc);
	while(i <= srclen)
	{
		if (strsrc[i] == delimiter || i == srclen)
		{
			len = strmid(strdest[aNum], strsrc, li, i, 128);
			strdest[aNum][len] = 0;
			li = i + 1;
			aNum++;
		}
		i++;
	}
}
strtok(const string[], &index)
{
	new length = strlen(string);
	while ((index < length) && (string[index] <= ' '))
	{
		index++;
	}

	new offset = index;
	new result[20];
	while ((index < length) && (string[index] > ' ') && ((index - offset) < (sizeof(result) - 1)))
	{
		result[index - offset] = string[index];
		index++;
	}
	result[index - offset] = EOS;
	return result;
}
stock sscanf(string[], format[], {Float,_}:...)
{
	#if defined isnull
		if (isnull(string))
	#else
		if (string[0] == 0 || (string[0] == 1 && string[1] == 0))
	#endif
		{
			return format[0];
		}
	#pragma tabsize 4
	new
		formatPos = 0,
		stringPos = 0,
		paramPos = 2,
		paramCount = numargs(),
		delim = ' ';
	while (string[stringPos] && string[stringPos] <= ' ')
	{
		stringPos++;
	}
	while (paramPos < paramCount && string[stringPos])
	{
		switch (format[formatPos++])
		{
			case '\0':
			{
				return 0;
			}
			case 'i', 'd':
			{
				new
					neg = 1,
					num = 0,
					ch = string[stringPos];
				if (ch == '-')
				{
					neg = -1;
					ch = string[++stringPos];
				}
				do
				{
					stringPos++;
					if ('0' <= ch <= '9')
					{
						num = (num * 10) + (ch - '0');
					}
					else
					{
						return -1;
					}
				}
				while ((ch = string[stringPos]) > ' ' && ch != delim);
				setarg(paramPos, 0, num * neg);
			}
			case 'h', 'x':
			{
				new
					num = 0,
					ch = string[stringPos];
				do
				{
					stringPos++;
					switch (ch)
					{
						case 'x', 'X':
						{
							num = 0;
							continue;
						}
						case '0' .. '9':
						{
							num = (num << 4) | (ch - '0');
						}
						case 'a' .. 'f':
						{
							num = (num << 4) | (ch - ('a' - 10));
						}
						case 'A' .. 'F':
						{
							num = (num << 4) | (ch - ('A' - 10));
						}
						default:
						{
							return -1;
						}
					}
				}
				while ((ch = string[stringPos]) > ' ' && ch != delim);
				setarg(paramPos, 0, num);
			}
			case 'c':
			{
				setarg(paramPos, 0, string[stringPos++]);
			}
			case 'p':
			{
				delim = format[formatPos++];
				continue;
			}
			case '\'':
			{
				new
					end = formatPos - 1,
					ch;
				while ((ch = format[++end]) && ch != '\'') {}
				if (!ch)
				{
					return -1;
				}
				format[end] = '\0';
				if ((ch = strfind(string, format[formatPos], false, stringPos)) == -1)
				{
					if (format[end + 1])
					{
						return -1;
					}
					return 0;
				}
				format[end] = '\'';
				stringPos = ch + (end - formatPos);
				formatPos = end + 1;
			}
			case 's', 'z':
			{
				new
					i = 0,
					ch;
				if (format[formatPos])
				{
					while ((ch = string[stringPos++]) && ch != delim)
					{
						setarg(paramPos, i++, ch);
					}
					if (!i)
					{
						return -1;
					}
				}
				else
				{
					while ((ch = string[stringPos++]))
					{
						setarg(paramPos, i++, ch);
					}
				}
				stringPos--;
				setarg(paramPos, i, '\0');
			}
			default:
			{
				continue;
			}
		}
		while (string[stringPos] && string[stringPos] != delim && string[stringPos] > ' ')
		{
			stringPos++;
		}
		while (string[stringPos] && (string[stringPos] == delim || string[stringPos] <= ' '))
		{
			stringPos++;
		}
		paramPos++;
	}
	do
	{
		if ((delim = format[formatPos++]) > ' ')
		{
			if (delim == '\'')
			{
				while ((delim = format[formatPos++]) && delim != '\'') {}
			}
			else if (delim != 'z')
			{
				return delim;
			}
		}
	}
	while (delim > ' ');
	return 0;
}


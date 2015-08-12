#include "gameinterface.h"
#include "engine.h"
#include "utils.h"
#include "gettime.h"
void GameInterface::TriggerHudMsg(int playerid, char *text, int duration, int towho) {
	CBasePlayer *pPlayer = new CBasePlayer();
	int cptr = 0;
	cptr = pPlayer->getPointerForPlayerID(playerid);
	int edptr = 0;
	if ( towho != MSG_BROADCAST ){
		edptr = ReadInt32(cptr + client_t::pedicts);
	}
	MESSAGE_BEGIN(edptr, 0, 0x73, towho); //0x73 for GUI messages, is 0x75 used for centered screen messages?
		WRITE_STRING(text); //text
		WRITE_BYTE(1); //spawnflags
		WRITE_BYTE(1); //show on this minimum skill level: 1 = operative, 2 = agent, 3 = 00agent
		WRITE_BYTE(duration); //duration of hudmsg
	MESSAGE_END();
	delete pPlayer;
}
void GameInterface::CenterPrint(int playerid, char *text, int towho) {
	CBasePlayer *pPlayer = new CBasePlayer();
	int cptr = 0;
	cptr = pPlayer->getPointerForPlayerID(playerid);
	int edptr = 0;
	if ( towho != MSG_BROADCAST ){
		edptr = ReadInt32(cptr + client_t::pedicts);
	}
	MESSAGE_BEGIN(edptr, 0, 26, towho); //0x73 for GUI messages, is 0x75 used for centered screen messages?
		WRITE_STRING(text); //text
	MESSAGE_END();
	delete pPlayer;
}
void GameInterface::ResetSetView(int playerid) {
	SetView(playerid, playerid);
}
void GameInterface::SetView(int playerid, int towho) {
	CBasePlayer *pPlayer = new CBasePlayer();
	int cptr, edptr, cptr2, edptr2;


	cptr = pPlayer->getPointerForPlayerID(playerid);
	edptr = ReadInt32(cptr + client_t::pedicts);
	if ( edptr == 0 ) return;

	cptr2 = pPlayer->getPointerForPlayerID(towho);
	edptr2 = ReadInt32(cptr2 + client_t::pedicts);
	if ( edptr2 == 0 ) return;

	int adr = ADR_SetView;
	__asm {
		push edptr
		push edptr2
		call adr
		add esp, 8
	}
	delete pPlayer;
}
void GameInterface::ClientPrint(int playerid, int dest, char *message) {
	CBasePlayer *pPlayer = new CBasePlayer();
	int cptr, playeridptr;
	cptr = pPlayer->getPointerForPlayerID(playerid);
	playeridptr = ReadInt32(cptr + client_t::pedicts);

	MESSAGE_BEGIN(playeridptr, 0, UserMessageBegin(MSG_ONE, "SayText"), MSG_ONE); //0x73 for GUI messages, is 0x75 used for centered screen messages?
		WRITE_BYTE( dest );
		WRITE_STRING( message );
	MESSAGE_END();
	delete pPlayer;
}
//-----------------------------------------------------------------------------
// Purpose: Fade an entity (player) screen to a specified color.
// Input  : *pEntity - Entity to tell to screen fade on.
// Input  : &color  - Color to fade to
// Input  : fadeTime - Time it takes to fade
// Input  : fadeHold - Time the fade holds for
// Input  : flags - Fade in, Fade Out, Fade_Modulate (don't blend), Fade_StayOut, Fade_Purge (Replaces others)
// Output : 
void GameInterface::UTIL_ScreenFade( int playerid, byte r, byte g, byte b, byte a, float fadeTime, float fadeHold, int flags ) { //UTIL_ScreenFade(int playerid, short duration, short holdtime, short fadeflags, byte r, byte g, byte b, byte a) {
	int newflags = 0;
	CBasePlayer *pPlayer = new CBasePlayer;
	bool fakeclient = pPlayer->IsAClient(playerid);
	delete pPlayer;

	__asm {
		mov eax, flags
		xor edi, edi //set edi to 0
		test al, 0x01
		jnz one
		mov edi, 0x1
	one:
		test al, 0x02
		jz two
		or edi,0x00000003
	two:
		test al, 0x04
		mov newflags, edi
		jz fadeall
	}
	if ( !fakeclient ) 
		UTIL_ScreenFadeOne(playerid, r, g, b, a, fadeTime, fadeHold, newflags);
	return;
	fadeall:
	UTIL_ScreenFadeAll(r, g, b, a, fadeTime, fadeHold, newflags);
}
void GameInterface::UTIL_ScreenFadeAll( byte r, byte g, byte b, byte a, float fadeTime, float fadeHold, int flags ) {
	int unknown, unknown2;
	MESSAGE_BEGIN(0, 0, UserMessageBegin(MSG_BROADCAST, "ScreenFade"), MSG_BROADCAST); 
		WRITE_COORD(fadeTime);
		WRITE_COORD(fadeHold);
		WRITE_BYTE(r);
		WRITE_BYTE(g);
		WRITE_BYTE(b);
		__asm {
			mov ebx, flags
			mov edi, ebx
			and edi, 0x00000001
			mov eax, 0x0
			setnz al
			dec eax
			and eax, 0x000000FF
			mov unknown, eax
			mov unknown2, edi
		}
		WRITE_BYTE(unknown);
		WRITE_BYTE(r);
		WRITE_BYTE(g);
		WRITE_BYTE(b);
		if ( unknown2 == 0 ) 
			a = 255 - a;
		WRITE_BYTE(a);
		__asm {
			mov ebx, flags //might be unnecessary, but keep it just in case
			shr ebx, 0x1
			not bl
			and ebx, 0x00000001
			mov unknown, ebx
		}
		WRITE_BYTE(unknown);
	MESSAGE_END();
}
void GameInterface::UTIL_ScreenFadeOne( int playerid, byte r, byte g, byte b, byte a, float fadeTime, float fadeHold, int flags ) {
	CBasePlayer *pPlayer = new CBasePlayer();
	int cptr, playeridptr;
	cptr = pPlayer->getPointerForPlayerID(playerid);
	playeridptr = ReadInt32(cptr + client_t::pedicts);
	delete pPlayer;
	int unknown, unknown2;
	MESSAGE_BEGIN(playeridptr, 0, UserMessageBegin(MSG_ONE, "ScreenFade"), MSG_ONE);
		WRITE_COORD(fadeTime);
		WRITE_COORD(fadeHold);
		WRITE_BYTE(r);
		WRITE_BYTE(g);
		WRITE_BYTE(b);
		__asm {
			mov ebx, flags
			mov edi, ebx
			and edi, 0x00000001
			mov eax, 0x0
			setnz al
			dec eax
			and eax, 0x000000FF
			mov unknown, eax
			mov unknown2, edi
		}
		WRITE_BYTE(unknown);
		WRITE_BYTE(r);
		WRITE_BYTE(g);
		WRITE_BYTE(b);
		if ( unknown2 == 0 ) 
			a = 255 - a;
		WRITE_BYTE(a);
		__asm{
			mov ebx, flags //might be unnecessary, but keep it just in case
			shr ebx, 0x1
			not bl
			and ebx, 0x00000001
			mov unknown, ebx
		}
		WRITE_BYTE(unknown);
	MESSAGE_END();
}
void GameInterface::UTIL_ShowMenu(int playerid, int slots, int time, char *menu, int mlen) {
	CBasePlayer *pPlayer = new CBasePlayer();
	int cptr, playeridptr;
	cptr = pPlayer->getPointerForPlayerID(playerid);
	playeridptr = ReadInt32(cptr + client_t::pedicts);	

	MESSAGE_BEGIN(playeridptr, 0, UserMessageBegin(MSG_ONE, "ShowMenu"), MSG_ONE);
		WRITE_SHORT( 6 );			//KeysBitSum
		WRITE_CHAR( -1 );			//Display time (-1 means unlimited)
		WRITE_BYTE( FALSE );		// there is no more message to come
		WRITE_STRING( (char*) menu );
	MESSAGE_END();

	delete pPlayer;
}
void GameInterface::UTIL_HudMessage(int playerid, const hudtextparms_t &textparms, char *pMessage) {
	CBasePlayer *pPlayer = new CBasePlayer();
	int cptr, playeridptr;
	cptr = pPlayer->getPointerForPlayerID(playerid);
	playeridptr = ReadInt32(cptr + client_t::pedicts);	

	if(playeridptr)
		MESSAGE_BEGIN(playeridptr, 0, UserMessageBegin(MSG_ONE, "HudMsg"), MSG_ONE);
	//else
	//	MESSAGE_BEGIN(0, 0, MSG_BROADCAST, SVC_TEMPENTITY);

	WRITE_BYTE(textparms.channel & 0xFF);
	WRITE_SHORT(FixedSigned16(textparms.x, (1<<13)));
	WRITE_SHORT(FixedSigned16(textparms.y, (1<<13)));
	WRITE_BYTE(textparms.effect);
	WRITE_BYTE(textparms.r1);
	WRITE_BYTE(textparms.g1);
	WRITE_BYTE(textparms.b1);
	WRITE_BYTE(0);
	WRITE_BYTE(255);
	WRITE_BYTE(255);
	WRITE_BYTE(250);
	WRITE_BYTE(0);
	WRITE_SHORT(FixedUnsigned16(textparms.fadeinTime, (1<<8)));
	WRITE_SHORT(FixedUnsigned16(textparms.fadeoutTime, (1<<8)));
	WRITE_SHORT(FixedUnsigned16(textparms.holdTime, (1<<8)));
	
	if (textparms.effect == 2)
		WRITE_SHORT(FixedUnsigned16(textparms.fxTime, (1<<8)));
	
	WRITE_STRING(pMessage);
	MESSAGE_END();

	delete pPlayer;
}
void GameInterface::UTIL_SayTextFilter( int senderid, char *pText, int receiver, bool bChat ) {
	CBasePlayer *pPlayer = new CBasePlayer();
	int cptr, senderidptr, receiverptr;
	cptr = pPlayer->getPointerForPlayerID(senderid);
	senderidptr = ReadInt32(cptr + client_t::pedicts);	

	cptr = pPlayer->getPointerForPlayerID(receiver);
	receiverptr = ReadInt32(cptr + client_t::pedicts);

	if(receiverptr) {
		MESSAGE_BEGIN(receiverptr, 0, UserMessageBegin(MSG_ONE, "HudMsg"), MSG_ONE);
			( senderidptr ) ? WRITE_BYTE( senderidptr ) : 	WRITE_BYTE( 0 ); // world, dedicated server says
			WRITE_STRING( pText );
			WRITE_BYTE( bChat );
		MESSAGE_END();
	}
	delete pPlayer;
}
void GameInterface::UTIL_ShowMessage(int playerid, char *pString) {
	CBasePlayer *pPlayer = new CBasePlayer();
	int cptr, playeridptr;
	cptr = pPlayer->getPointerForPlayerID(playerid);
	playeridptr = ReadInt32(cptr + client_t::pedicts);

	if(!playeridptr) 
		return;

	MESSAGE_BEGIN(playeridptr, 0, UserMessageBegin(MSG_ONE, "HudText"), MSG_ONE);
		WRITE_STRING( pString );
	MESSAGE_END();
	delete pPlayer;
}
void GameInterface::UTIL_SetVisionColor(int playerid, int color) {
	CBasePlayer *pPlayer = new CBasePlayer();
	int cptr, playeridptr;
	cptr = pPlayer->getPointerForPlayerID(playerid);
	playeridptr = ReadInt32(cptr + client_t::pedicts);

	if(!playeridptr) 
		return;

	MESSAGE_BEGIN(playeridptr, 0, UserMessageBegin(MSG_ONE, "VisionMode"), MSG_ONE);
		WRITE_BYTE( color );
	MESSAGE_END();
	delete pPlayer;
}
void GameInterface::UTIL_CreateParticleEffect(int playerid, float OriginX, float OriginY, float OriginZ, float DirectionX, float DirectionY, float DirectionZ, byte Count, byte Color, int towho) {
	CBasePlayer *pPlayer = new CBasePlayer();
	int cptr = 0;
	cptr = pPlayer->getPointerForPlayerID(playerid);
	int edptr = 0;
	if ( towho != MSG_BROADCAST ){
		edptr = ReadInt32(cptr + client_t::pedicts);
	}
	MESSAGE_BEGIN(edptr, 0, SVC_PARTICLE, towho); //0x73 for GUI messages, is 0x75 used for centered screen messages?
		WRITE_COORD(OriginX);
		WRITE_COORD(OriginY);
		WRITE_COORD(OriginZ);
		WRITE_CHAR(DirectionX * 16);
		WRITE_CHAR(DirectionY * 16);
		WRITE_CHAR(DirectionZ * 16);
		WRITE_BYTE(Count);
		WRITE_BYTE(Color);
	MESSAGE_END();
	delete pPlayer;
}
void GameInterface::UTIL_CreateTracer(int playerid, float xStart, float yStart, float zStart, float xEnd, float yEnd, float zEnd, int towho) {
	CBasePlayer *pPlayer = new CBasePlayer();
	int cptr = 0;
	cptr = pPlayer->getPointerForPlayerID(playerid);
	int edptr = 0;
	if ( towho != MSG_BROADCAST ){
		edptr = ReadInt32(cptr + client_t::pedicts);
	}
	MESSAGE_BEGIN(edptr, 0, SVC_TEMPENTITY, towho); //0x73 for GUI messages, is 0x75 used for centered screen messages?
		WRITE_BYTE(TE_TRACER);
		WRITE_COORD(xStart);
		WRITE_COORD(yStart);
		WRITE_COORD(zStart);
		WRITE_COORD(xEnd);
		WRITE_COORD(yEnd);
		WRITE_COORD(zEnd);
	MESSAGE_END();
	delete pPlayer;
}
void GameInterface::UTIL_CreateBeamEntsPlayers(int playerid, int toplayerid, int spriteindex, byte startframe, byte frame, byte life, byte width, byte noise, byte r, byte g, byte b, byte bright, byte scroll, int msgtype) {
	CBasePlayer *pPlayer = new CBasePlayer();
	int cptr, cptr2;
	int edptr, edptr2;
	float X, Y, Z, FX, FY, FZ;

	pPlayer->GetPlayerPos(playerid, X, Y, Z);
	pPlayer->GetPlayerPos(toplayerid, FX, FY, FZ);

	edptr = 0;
	edptr2 = 0;
	if ( msgtype != MSG_BROADCAST ) {
		cptr = pPlayer->getPointerForPlayerID(playerid);
		cptr2 = pPlayer->getPointerForPlayerID(toplayerid);
		edptr = ReadInt32(cptr + client_t::pedicts);
		edptr2 = ReadInt32(cptr2 + client_t::pedicts);
	}

	MESSAGE_BEGIN(edptr2, 0, SVC_TEMPENTITY, msgtype); //0x73 for GUI messages, is 0x75 used for centered screen messages?
		WRITE_BYTE(TE_BEAMPOINTS);
		WRITE_COORD(X);	// START POSITION
		WRITE_COORD(Y);
		WRITE_COORD(Z);
		WRITE_COORD(FX);	// END POSITION
		WRITE_COORD(FY);
		WRITE_COORD(FZ);
		WRITE_SHORT(spriteindex);	// SPRITE INDEX
		WRITE_BYTE(startframe);	// STARTING FRAME
		WRITE_BYTE(frame);	// FRAME RATE IN 0.1'S
		WRITE_BYTE(life);	// LIFE IN 0.1'S
		WRITE_BYTE(width);	// LINE WIDTH IN 0.1'S
		WRITE_BYTE(noise);	// NOISE AMPLITUDE IN 0.01'S
		WRITE_BYTE(r);	// RED
		WRITE_BYTE(g);	// GREEN
		WRITE_BYTE(b);	// BLUE
		WRITE_BYTE(bright);	// BRIGHTNESS
		WRITE_BYTE(scroll);	// SCROLL SPEED IN 0.1'S
	MESSAGE_END();
	delete pPlayer;
}
void GameInterface::UTIL_CreateTempTextMessage(int playerid, float x, float y, byte effect, byte textR, byte textG, byte textB, byte textA, byte fxR, byte fxG, byte fxB, byte fxA, short fadein, short fadeout, short holdtime, short fxtime, char *message, int msgtype) {
	CBasePlayer *pPlayer = new CBasePlayer();
	int cptr;
	int edptr;

	edptr = 0;
	if ( msgtype != MSG_BROADCAST ) {
		cptr = pPlayer->getPointerForPlayerID(playerid);
		edptr = ReadInt32(cptr + client_t::pedicts);
	}

	MESSAGE_BEGIN(edptr, 0, SVC_TEMPENTITY, msgtype); //0x73 for GUI messages, is 0x75 used for centered screen messages?
		WRITE_BYTE( TE_TEXTMESSAGE );
		WRITE_BYTE( 1 & 255 );
		WRITE_SHORT( FixedSigned16( x, FLOAT(1<<13) ) );
		WRITE_SHORT( FixedSigned16( y, FLOAT(1<<13) ) );
		WRITE_BYTE( effect );
		WRITE_BYTE( textR );
		WRITE_BYTE( textG );
		WRITE_BYTE( textB );
		WRITE_BYTE( textA );
		WRITE_BYTE( 255 );
		WRITE_BYTE( 255 );
		WRITE_BYTE( 255 );
		WRITE_BYTE( fxA );
		WRITE_SHORT( FixedUnsigned16( fadein, (1<<8) ) );
		WRITE_SHORT( FixedUnsigned16( fadeout, (1<<8) ) );
		WRITE_SHORT( FixedUnsigned16( holdtime, (1<<8) ) );
	MESSAGE_END();
	delete pPlayer;
}
void GameInterface::UTIL_Create_TE_ELIGHT(int edictptr, float x, float y, float z, int radius, int red, int green, int blue, int life, int decayRate ) {
	engineModule *engine = new engineModule;
	//All game versions support entity lights
	MESSAGE_BEGIN(0, 0, SVC_TEMPENTITY, MSG_BROADCAST);
		WRITE_BYTE( TE_ELIGHT );
		WRITE_SHORT( engine->ENTINDEX( edictptr ) + 0x1000 );		// entity, attachment
		WRITE_COORD( x );		// origin
		WRITE_COORD( y );
		WRITE_COORD( z );
		WRITE_COORD( radius );	// radius
		WRITE_BYTE( red );	// R
		WRITE_BYTE( green );	// G
		WRITE_BYTE( blue );	// B
		WRITE_BYTE( life );	// life * 10
		WRITE_COORD( decayRate ); // decay
	MESSAGE_END();

	delete engine;
}
void GameInterface::Create_TE_DLIGHT(int MSGTYPE, int edictptr, float x, float y, float z, float exponent, float radius, int red, int green, int blue, int life, float decayRate) {
	engineModule *engine = new engineModule;
	CBasePlayer *pPlayer = new CBasePlayer;
	MESSAGE_BEGIN(edictptr, 0, SVC_TEMPENTITY, MSGTYPE);
		WRITE_BYTE( TE_DLIGHT );
		WRITE_SHORT( engine->ENTINDEX(edictptr) );
		WRITE_COORD( x );  // origin
		WRITE_COORD( y );
		WRITE_COORD( z );
		WRITE_COORD( exponent ); //brightness
		WRITE_COORD( radius ); //radius
		WRITE_BYTE( red ); // R
		WRITE_BYTE( green ); // G
		WRITE_BYTE( blue ); // B
		WRITE_BYTE ( life ); //life
		WRITE_COORD ( decayRate ); //decay
	MESSAGE_END();	
	delete engine;
	delete pPlayer;
}
void GameInterface::UTIL_Create_TE_DLIGHT(int edictptr, float x, float y, float z, float exponent, float radius, int red, int green, int blue, int life, float decayRate ) {
	if ( !edictptr ){
		CBasePlayer *pPlayer = new CBasePlayer;
		if ( !pPlayer->NumPlayersWithoutDynamicLightSupport ) {
			//All players support dynamic lights, send a single message notifying everyone about this light
			Create_TE_DLIGHT( MSG_BROADCAST, edictptr, x, y, z, exponent, radius, red, green, blue, life, decayRate);
		}else{
			//send individual messages to each client that DOES support dynamic lights, ignoring those who don't so they don't crash
			for(int i=0; i<ReadInt32(SVS_MAXCLIENTS); i++) {
				if(pPlayer->IsClientConnected(i) && pPlayer->IsAClient(i) && pPlayer->SupportsDynamicLights(i)) {
					edictptr = pPlayer->GetEdictNum(i);
					Create_TE_DLIGHT( MSG_ONE_UNRELIABLE, edictptr, x, y, z, exponent, radius, red, green, blue, life, decayRate);
				}
			}
		}
		delete pPlayer;
	}else{ //send a private message to a specific person notifying them of this light, but only if they support dynamic lights
		CBasePlayer *pPlayer = new CBasePlayer;
		if ( ReadByte((pPlayer->GetClientPtr(edictptr)) + client_t::pfakeclient) == 0 ){
			Utils *util = new Utils;
			string gamever = pPlayer->GetUserInfoStringValue("gamever", edictptr);
			char string[64];
			strcpy(string, util->ExplodeAndReturn(gamever, 0, " ").c_str()); 
			delete util;
			if( strlen(string) > 0 ) {
				if ( stof((const char*)string) > 5.81f) {
					Create_TE_DLIGHT( MSG_ONE_UNRELIABLE, edictptr, x, y, z, exponent, radius, red, green, blue, life, decayRate);
				}
			}
		}
	}
}
void GameInterface::Create_TE_EXPLOSION( float x, float y, float z, int iSprite, byte scale, byte frameRate, byte flags ) {
	MESSAGE_BEGIN(0, 0, SVC_TEMPENTITY, MSG_PVS);
	WRITE_BYTE( TE_EXPLOSION );
	WRITE_COORD( x );				// position (X)
	WRITE_COORD( y );				// position (Y)
	WRITE_COORD( z	);				// position (Z)
	WRITE_SHORT( iSprite );			// sprite index
	WRITE_BYTE( scale );			// scale in 0.1's
	WRITE_BYTE( frameRate );		// framerate
	WRITE_BYTE( flags );			// flags
	MESSAGE_END();
	//#define TE_EXPLFLAG_NONE		0	// all flags clear makes default Half-Life explosion
	//#define TE_EXPLFLAG_NOADDITIVE	1	// sprite will be drawn opaque (ensure that the sprite you send is a non-additive sprite)
	//#define TE_EXPLFLAG_NODLIGHTS	2	// do not render dynamic lights
	//#define TE_EXPLFLAG_NOSOUND		4	// do not play client explosion sound
	//#define TE_EXPLFLAG_NOPARTICLES	8	// do not draw particles
}
void GameInterface::Util_Attach_Entity(int playerid, float VertOfsZ, short modelIndex, short life, int msgtype) { //Kicks the client..
	CBasePlayer *pPlayer = new CBasePlayer();
	engineModule *engine = new engineModule;
	int cptr;
	int edptr;

	edptr = 0;
	if ( msgtype != MSG_BROADCAST ) {
		cptr = pPlayer->getPointerForPlayerID(playerid);
		edptr = ReadInt32(cptr + client_t::pedicts);
	}

	int entindex = engine->ENTINDEX(edptr);

	MESSAGE_BEGIN(0, 0, SVC_TEMPENTITY, msgtype);
		WRITE_BYTE( TE_PLAYERATTACHMENT );
		WRITE_BYTE( entindex );		// byte	(entity index of player)
		WRITE_COORD( VertOfsZ );						// coord (vertical offset) ( attachment origin.z = player origin.z + vertical offset)
		WRITE_SHORT( modelIndex );		// short (model index) of tempent
		WRITE_SHORT( life );						// short (life * 10 ) e.g. 40 = 4 seconds
	MESSAGE_END();
	delete pPlayer;
	delete engine;
}
void GameInterface::SendMOTDToClient(int playerid){
	// read from the MOTD.txt file
	int length, char_count = 0;
	int cptr, edictptr;
	char *pFileList;
	engineModule *engine = new engineModule;
	CBasePlayer *pPlayer = new CBasePlayer;
	char *aFileList = pFileList = (char*)engine->LOAD_FILE_FOR_ME( (char *)engine->CVAR_GET_STRING( "motdfile" ), &length );
	char destination[128];

	if(!pPlayer->IsAClient(playerid))
		return;

	cptr = pPlayer->getPointerForPlayerID(playerid);

	if(!cptr) 
		return;

	edictptr = ReadInt32(cptr + client_t::pedicts);

	if(!edictptr) 
		return;

	if ( pFileList ) engine->CLIENT_PRINTF("------------------MESSAGE OF THE DAY------------------\n", client_printf_console, edictptr);

	// Send the message of the day
	// read it chunk-by-chunk,  and send it in parts
	while ( pFileList && *pFileList && char_count < MAX_MOTD_LENGTH )
	{
		char chunk[MAX_MOTD_CHUNK+1];
		
		if ( strlen( pFileList ) < MAX_MOTD_CHUNK )
		{
			strcpy( chunk, pFileList );
		}
		else
		{
			strncpy( chunk, pFileList, MAX_MOTD_CHUNK );
			chunk[MAX_MOTD_CHUNK] = 0;		// strncpy doesn't always append the null terminator
		}

		char_count += strlen( chunk );
		if ( char_count < MAX_MOTD_LENGTH )
			pFileList = aFileList + char_count; 
		else
			*pFileList = 0;

		sprintf(destination, "%s\n", chunk);
		engine->CLIENT_PRINTF(destination, client_printf_console, edictptr);
		//ClientPrint(playerid, 2, chunk);
	}
	engine->FREE_FILE( (byte *)aFileList );
	delete pPlayer;
	delete engine;
}
void GameInterface::UTIL_ScreenShake(int playerid, short amplitude, short duration, short frequency, short radius, short command) {
	CBasePlayer *pPlayer = new CBasePlayer();
	int cptr, playeridptr;
	cptr = pPlayer->getPointerForPlayerID(playerid);
	if(!cptr)
		return;

	playeridptr = ReadInt32(cptr + client_t::pedicts);

	if(!playeridptr)
		return;

	MESSAGE_BEGIN(playeridptr, 0, UserMessageBegin(MSG_ONE, "ScreenShake"), MSG_ONE); //0x73 for GUI messages, is 0x75 used for centered screen messages?
		WRITE_BYTE( amplitude * 5 );	// amplitude
		WRITE_BYTE( frequency * 5 );	// frequency
		WRITE_BYTE( duration * 1 );		// duration
		WRITE_BYTE( radius ); //RADIUS
		WRITE_BYTE( command ); //Command, start, shake stop, etc.
		WRITE_BYTE( 1 ); //bAirShake - if this is false, then it will only shake players standing on the ground. (Doesn't work with this on 0)
	MESSAGE_END();
	delete pPlayer;
}
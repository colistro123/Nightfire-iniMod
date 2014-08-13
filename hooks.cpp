#include "hooks.h"
#include "addresses.h"
#include "con_commands.h"
#include "host_cmd.h"
#include "gamemodes.h"
#include "player.h"

//Global variables 
BYTE *retJMP;
DWORD myData = 0;
DWORD tempRegister = 0;
DWORD tempRegister2 = 0;
DWORD tempEAX;
DWORD tempEBX;
DWORD tempECX;
DWORD tempEDX;
DWORD tempESI;
DWORD tempEDI;
DWORD tempESP;
DWORD tempEBP;
DWORD tempEIP;
DWORD tempEFL;
char value[1024] = {0};
typedef struct {
	BYTE *address;
	DWORD retFunc;
	DWORD len;
	BYTE *retJMPAddr;
} HookCallInfo;

HookCallInfo funcHooks[] = {
	{(BYTE*)ADR_INVALID_CMD, (DWORD)OnCommandProcess, 6, (BYTE*)ADR_INVALID_CMD+0x5}, //patch address, return function (our code), length, return jump address
	{(BYTE*)ADR_CHANGEMAP_FN, (DWORD)hookOnServerChangeMap, 6, (BYTE*)ADR_CHANGEMAP_FN+0x5},
	{(BYTE*)ADR_CONNECT_FN, (DWORD)hookOnClientConnect, 6, (BYTE*)ADR_CONNECT_FN+0x5},
	{(BYTE*)ADR_DISCONNECT_FN, (DWORD)hookOnClientDisconnect, 6, (BYTE*)ADR_DISCONNECT_FN+0x5},
	{(BYTE*)ADR_TRIGGER_EVT, (DWORD)OnTriggerEvent, 6, (BYTE*)ADR_TRIGGER_EVT+0x5},
	{(BYTE*)ADR_ADDBOT_FN, (DWORD)hookOnBotConnect, 6, (BYTE*)ADR_ADDBOT_FN+0x5},
	{(BYTE*)ADR_HOSTSAY, (DWORD)hookOnHostSay, 6, (BYTE*)ADR_HOSTSAY+0x5},
	{(BYTE*)ADR_PLAYERKILLED_FN, (DWORD)hookOnPlayerKilled, 6, (BYTE*)ADR_PLAYERKILLED_FN+0x5}
	//{(BYTE*)ADR_PLAYERSUICIDED_FN, (DWORD)hookOnPlayerSuicided, 6, (BYTE*)ADR_PLAYERSUICIDED_FN+0x5}
};

void mainHooks() {
	printf("iniMod: Hooking game engine and applying memory patches..\n");
	applyPatches();
}

void applyPatches( void ) {
	for(int i=0;i<sizeof(funcHooks)/sizeof(HookCallInfo);i++) {
		PlaceJMP((BYTE*)funcHooks[i].address, funcHooks[i].retFunc, funcHooks[i].len);
	}
	printf("iniMod: Done hooking game engine and applying memory patches!\n");
}

void PlaceJMP( BYTE *bt_DetourAddress, DWORD dw_FunctionAddress, DWORD dw_Size)
{
	DWORD dw_OldProtection, dw_Distance;
	VirtualProtect(bt_DetourAddress, dw_Size, PAGE_EXECUTE_READWRITE, &dw_OldProtection);
	dw_Distance = (DWORD)(dw_FunctionAddress - (DWORD)bt_DetourAddress) - 5;
	*bt_DetourAddress = 0xE9;
	*(DWORD*)(bt_DetourAddress + 0x1) = dw_Distance;
	for(int i = 0x5; i < dw_Size; i++) *(bt_DetourAddress + i) = 0x90;
	VirtualProtect(bt_DetourAddress, dw_Size,  dw_OldProtection, NULL);
    return;
}

BYTE* getHookFuncJMPAddr(DWORD fnName) {
	for(int i=0;i<sizeof(funcHooks)/sizeof(HookCallInfo);i++) {
		if(funcHooks[i].retFunc == fnName) {
			return (BYTE*)funcHooks[i].retJMPAddr;
		}
	}
	printf("iniMod: One or more of the provided function addresses weren't found\n");
	return 0;
}
__declspec(naked) void OnTriggerEvent() {
	//Start doing our stuff
	tempRegister = NUM_FOR_EDICT;
	__asm mov dword ptr ds:[tempECX], ecx
	__asm mov dword ptr ds:[tempEBX], ebx
	__asm mov dword ptr ds:[tempEAX], eax
	__asm mov dword ptr ds:[tempEDI], edi
	//For EBX
	__asm mov ebx, dword ptr ds:[esi+4]
	__asm push ebx //edictptr
	__asm call tempRegister //get number for this edict
	__asm test eax, eax
	__asm je invalid //jump if it's a bad pointer
		__asm mov ecx,dword ptr ds:[0x44A86B4C] //get the first playerptr
		__asm imul eax, eax, 0x4F18 //integer multiply number by size of tablebuffer
		__asm lea eax, [ecx+eax-0x4F18] //get THIS player pointer
		__asm mov eax, dword ptr ds:[eax + 0x4b80] //store userid in eax
		__asm mov dword ptr ds:[tempRegister2], eax //store userid into tempRegister
		callGMModule(tempRegister2, tempEDI);
	invalid:
	__asm mov ecx, tempECX  //restore the original registers in a bad way
	__asm mov ebx, tempEBX
	__asm mov eax, tempEAX

	__asm add esp, 4
	//Original code
	__asm POP EBP
	__asm POP EDI
	__asm POP EBX
	__asm retn
}

__declspec(naked) void hookOnServerChangeMap() {
	//retJMP = getHookFuncJMPAddr((DWORD)hookOnClientConnect);
	//if(retJMP) { //If retJMP is not a valid address, it'll crash
		callOnServerChangeMap("");
		__asm retn
		//__asm mov dword ptr ds:[tempRegister], ebx
		
		//__asm add esp,0x14
		//__asm POP EDI
		//__asm jmp retJMP
	//}
}

__declspec(naked) void hookOnClientConnect() {
		__asm add esp,0x0A4C //restore original value since we intercepted
		__asm push edx //store the old value of edx
		__asm test eax, eax
		__asm je eaxis0 //player disconnected before finishing or invalid password entered
			__asm mov edx, dword ptr ds:[eax + 0x4b80] //store userid in edx
			__asm mov dword ptr ds:[tempRegister], edx //store userid into tempRegister
			callOnPreClientConnect(tempRegister);
		eaxis0:
		__asm pop edx //restore old value of edx
		__asm retn
}
__declspec(naked) void hookOnBotConnect() {
		tempRegister = NUM_FOR_EDICT;
		retJMP = getHookFuncJMPAddr((DWORD)hookOnBotConnect);
		if(retJMP){
			__asm push edx //store edx
			__asm push eax //store eax
			__asm push ecx //store ecx
			__asm test esi, esi //see if esi is 0
			__asm je invalid //bot disconnected before finishing or invalid password entered
				__asm push esi //edictptr
				__asm call tempRegister //get number for this edict
				__asm pop esi //restore esi just in case
				__asm test eax, eax
				__asm je invalid //jump if it's a bad pointer
					__asm mov ecx,dword ptr ds:[0x44A86B4C] //get the first playerptr
					__asm imul eax, eax, 0x4F18 //integer multiply number by size of tablebuffer
					__asm lea eax, [ecx+eax-0x4F18] //get THIS player pointer
					__asm mov edx, dword ptr ds:[eax + 0x4b80] //store userid in edx
					__asm mov dword ptr ds:[tempRegister], edx //store userid into tempRegister
					callOnPreClientConnect(tempRegister);
invalid:
			__asm pop edx
			__asm pop eax
			__asm pop ecx
			__asm push edi //execute original commands
			__asm push 0x4210D324 //execute original commands
			__asm jmp retJMP
		}
}

__declspec(naked) void hookOnClientDisconnect() {
	__asm mov dword ptr ds:[tempRegister], eax
	__asm mov dword ptr ds:[tempEAX], eax //STORE original value of registers in a really bad way since PUSH wasn't working properly
	__asm mov dword ptr ds:[tempECX], ecx
	__asm mov dword ptr ds:[tempEDX], edx
	retJMP = getHookFuncJMPAddr((DWORD)hookOnClientDisconnect);
	callOnClientDisconnect(tempRegister);
	__asm mov eax, tempEAX  //restore the original registers in a bad way
	__asm mov ecx, tempECX
	__asm mov edx, tempEDX

	__asm push eax //restore intercepted code
	__asm mov eax, dword ptr ds:[0x4217A168] //restore intercepted code
	__asm jmp retJMP //jump to original code
}

__declspec(naked) void hookOnHostSay() {
	tempRegister = NUM_FOR_EDICT;
	__asm mov dword ptr ds:[tempECX],ecx
	__asm push edx
	__asm push eax
	__asm mov dword ptr ds:[tempEDI], edi
	__asm push edi //edictptr
	__asm call tempRegister //get number for this edict
	__asm pop edi //restore esi just in case
	__asm test eax, eax
	__asm je invalid //jump if it's a bad pointer
			__asm mov ecx,dword ptr ds:[0x44A86B4C] //get the first playerptr
			__asm imul eax, eax, 0x4F18 //integer multiply number by size of tablebuffer
			__asm lea eax, [ecx+eax-0x4F18] //get THIS player pointer
			__asm mov eax, dword ptr ds:[eax + 0x4b80] //store userid in eax
			__asm mov dword ptr ds:[tempRegister2], eax //store userid into tempRegister
	invalid:
	__asm mov ecx, dword ptr ds:[tempECX]
	__asm pop eax
	__asm mov edi, dword ptr ds:[tempEDI]
	__asm pop edx
		
	retJMP = getHookFuncJMPAddr((DWORD)hookOnHostSay);
	__asm mov dword ptr ds:[tempRegister], esi //Store ascii in tempRegister

	__asm lea eax, [esp+14] //original code
	__asm push eax     
	__asm add esp, 0x10
	callTextModule(tempRegister2, tempRegister); //User, address of text
	__asm jmp retJMP
}

__declspec(naked) void OnCommandProcess() {
	retJMP = getHookFuncJMPAddr((DWORD)OnCommandProcess);
	if(retJMP) { //If retJMP is not a valid address, it'll crash
		__asm test ebx, ebx
		__asm je InvalidEBX
			__asm mov dword ptr ds:[tempRegister], ebx
			__asm mov tempRegister2, ebx
			__asm mov ebx, dword ptr ds:[tempRegister] //restore the register to the original value
			callConCommandModule(tempRegister2);
		InvalidEBX:
		__asm mov bl, byte ptr ds:[0x448BE0D4]
		__asm jmp retJMP
		memset(value, 0, sizeof(value));
	}
}
__declspec(naked) void hookOnPlayerKilled() {
	//EBX = Killer | ESI = Player Killed (Victim)
	tempRegister = NUM_FOR_EDICT;
	__asm mov dword ptr ds:[tempECX], ecx
	__asm mov dword ptr ds:[tempEBX], ebx
	__asm mov dword ptr ds:[tempEAX], eax
	__asm mov dword ptr ds:[tempEDI], edi
	//For ESI
	__asm push esi //edictptr
	__asm call tempRegister //get number for this edict
	__asm test eax, eax
	__asm je invalid //jump if it's a bad pointer
			__asm mov ecx,dword ptr ds:[0x44A86B4C] //get the first playerptr
			__asm imul eax, eax, 0x4F18 //integer multiply number by size of tablebuffer
			__asm lea eax, [ecx+eax-0x4F18] //get THIS player pointer
			__asm mov eax, dword ptr ds:[eax + 0x4b80] //store userid in eax
			__asm mov dword ptr ds:[tempRegister2], eax //store userid into tempRegister
	//For EBX
	__asm push ebx //edictptr
	__asm call tempRegister //get number for this edict
	__asm test eax, eax
	__asm je invalid //jump if it's a bad pointer
			__asm mov ecx,dword ptr ds:[0x44A86B4C] //get the first playerptr
			__asm imul eax, eax, 0x4F18 //integer multiply number by size of tablebuffer
			__asm lea eax, [ecx+eax-0x4F18] //get THIS player pointer
			__asm mov eax, dword ptr ds:[eax + 0x4b80] //store userid in eax
			__asm mov dword ptr ds:[tempRegister], eax //store userid into tempRegister
	invalid:
	retJMP = getHookFuncJMPAddr((DWORD)hookOnPlayerKilled);
	__asm mov ecx, dword ptr ds:[tempECX]
	__asm mov ebx, dword ptr ds:[tempEBX]
	__asm mov eax, dword ptr ds:[tempEAX]
	__asm mov edi, dword ptr ds:[tempEDI]
	__asm add esp, 4 //restore the code we replaced with the jump
	callOnClientDeath(tempRegister2, tempRegister);
	__asm jmp retJMP
}
__declspec(naked) void hookOnPlayerSuicided() { //TO FIX, INSTA CRASH, DOESN'T WORK
	//EBP = Player ptr
	retJMP = getHookFuncJMPAddr((DWORD)hookOnPlayerSuicided);

	tempRegister = NUM_FOR_EDICT;
	__asm mov dword ptr ds:[tempECX], ecx
	__asm mov dword ptr ds:[tempEBP], ebp
	__asm mov dword ptr ds:[tempEAX], eax
	__asm mov dword ptr ds:[tempEDI], edi
	__asm mov dword ptr ds:[tempEDX], edx
	//For EBP
	__asm push ebp //edictptr
	__asm call tempRegister //get number for this edict
	__asm test eax, eax
	__asm je invalid //jump if it's a bad pointer
			__asm mov ecx,dword ptr ds:[0x44A86B4C] //get the first playerptr
			__asm imul eax, eax, 0x4F18 //integer multiply number by size of tablebuffer
			__asm lea eax, [ecx+eax-0x4F18] //get THIS player pointer
			__asm mov eax, dword ptr ds:[eax + 0x4b80] //store userid in eax
			__asm mov dword ptr ds:[tempRegister2], eax //store userid into tempRegister
	invalid:

	//printf("%d suicided\n", tempRegister2);

	__asm mov ecx, dword ptr ds:[tempECX]
	__asm mov ebp, dword ptr ds:[tempEBP]
	__asm mov eax, dword ptr ds:[tempEAX]
	__asm mov edi, dword ptr ds:[tempEDI]
	__asm mov edx, dword ptr ds:[tempEDX]

	//The code we replaced
	__asm mov ebx, eax
	__asm mov eax, dword ptr ss:[esp+20]

	__asm add esp, 4
	__asm jmp retJMP
}
//Text
void callTextModule(int nfuserid, DWORD ebx) {
	std::string stringcontainingcommand = ReadCharArray(ebx, 1024);
	const char *charmap = stringcontainingcommand.c_str();
	sprintf(value, "%s", charmap);
	HostCMD *hostCMD = new HostCMD;
	hostCMD->OnPreClientCommandText(nfuserid, value);
	delete hostCMD;
	memset(value, 0, sizeof(value));
	//value[0] = 0;
}
//Commands
void callConCommandModule(DWORD ebx) {
	std::string stringcontainingcommand = ReadCharArray(ebx, 1024);
	const char *charmap = stringcontainingcommand.c_str();
	sprintf(value, "%s", charmap);

	CCommands *command = new CCommands;
	command->tryFindCommand(value);
	delete command;
	memset(value, 0, sizeof(value));
	//value[0] = 0;
}
//Gamemode
//Decide what to call by doing strcmp
void callGMModule(int playerid, DWORD edi) {
	std::string stringcontainingcommand = ReadCharArray(edi, 1024);
	const char *charmap = stringcontainingcommand.c_str();
	sprintf(value, "%s", charmap);
	CBasePlayer *pPlayer = new CBasePlayer;
	if(strcmp(charmap, "game_playerspawn") == 0) {
		pPlayer->OnClientSpawn(playerid); //In case we later on want to do something there
	}
	delete pPlayer;
	memset(value, 0, sizeof(value));
}
void callOnServerChangeMap( const char* recvcmd ) {
	CNetwork *cNetwork = new CNetwork;
	cNetwork->OnServerChangeMap();
	delete cNetwork;
	memset(value, 0, sizeof(value));
	//value[0] = 0;
}
void callOnPreClientConnect(int pPointer) {
	CNetwork *cNetwork = new CNetwork;
	CBasePlayer *pPlayer = new CBasePlayer;
	for(int playerid=0; playerid<pPlayer->MaxClients(); playerid++) {
		if(!pPlayer->IsClientOnTable(playerid)) { //Find an appropiate internal id index
			cNetwork->OnPreClientConnect(playerid);
			break; //Get out of the loop
		}
	}
	delete cNetwork;
	delete pPlayer;
}
void callOnClientDisconnect(int nfuserid) {
	CNetwork *cNetwork = new CNetwork;
	CBasePlayer *pPlayer = new CBasePlayer;
	int playerid = pPlayer->GetInternalIDFromNFID(nfuserid);
	cNetwork->OnClientDisconnect(playerid, REASON_DISCONNECT);
	delete cNetwork;
	delete pPlayer;
}
void callOnClientDeath(int attacker, int receiver) { //Todo put the weapon used as a parameter, gotta do the same for ASM
	CNetwork *cNetwork = new CNetwork;
	cNetwork->OnClientDeath(attacker, receiver);
	delete cNetwork;
}

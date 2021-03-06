#include "hooks.h"
#include "addresses.h"
#include "con_commands.h"
#include "host_cmd.h"
#include "gamemodes.h"
#include "player.h"
#include "engine.h"
#include "gameinterface.h"
#include "utils.h"
#include "plugins.h"
#include "gettime.h"

//Global variables 
BYTE *retJMP;
DWORD myData = 0;
Vector storeFloats();
float tempFloat[3];
float tempFloat1 = 0;
float tempFloat2 = 0;
float tempFloat3 = 0;
float tempFloat4 = 0;
float tempFloat5 = 0;
float tempFloat6 = 0;
byte tempByte1 = 0;
DWORD tempRegister = 0;
DWORD tempRegister2 = 0;
DWORD tempRegister3 = 0;
DWORD tempRegister4 = 0;
DWORD tempRegister5 = 0;
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
DWORD tempCall = 0;
char value[1024] = {0};
typedef struct {
	BYTE *address;
	DWORD retFunc;
	DWORD len;
	BYTE *retJMPAddr;
} HookCallInfo;

HookCallInfo funcHooks[] = {
	//{(BYTE*)ADR_INVALID_CMD, (DWORD)OnCommandProcess, 6, (BYTE*)ADR_INVALID_CMD+0x5}, //patch address, return function (our code), length, return jump address
	{(BYTE*)ADR_CHANGEMAP_FN, (DWORD)hookOnServerChangeMap, 6, (BYTE*)ADR_CHANGEMAP_FN+0x5}, //something is fishy with this function in release mode, causing mod_loadbrush errors when clients join!
	{(BYTE*)ADR_CONNECT_FN, (DWORD)hookOnClientConnect, 6, (BYTE*)ADR_CONNECT_FN+0x5},
	{(BYTE*)ADR_DROPCLIENT_HOOK, (DWORD)hookOnClientDisconnect, 6, (BYTE*)ADR_DROPCLIENT_HOOK+0x5}, //Deprecated, now uses ADR_DROPCLIENT_FN
	{(BYTE*)ADR_TRIGGER_EVT, (DWORD)OnTriggerEvent, 6, (BYTE*)ADR_TRIGGER_EVT+0x5},
	{(BYTE*)ADR_ADDBOT_FN, (DWORD)hookOnBotConnect, 6, (BYTE*)ADR_ADDBOT_FN+0x5},
	{(BYTE*)ADR_HOSTSAY, (DWORD)hookOnHostSay, 6, (BYTE*)ADR_HOSTSAY+0x5},
	{(BYTE*)ADR_PLAYERKILLED_FN, (DWORD)hookOnPlayerKilled, 6, (BYTE*)ADR_PLAYERKILLED_FN+0x5},
	{(BYTE*)ADR_PLAYERSPAWN, (DWORD)hookOnPlayerSpawn, 6, (BYTE*)ADR_PLAYERSPAWN+0x5},
	{(BYTE*)ADR_PLAYEREQUIP, (DWORD)hookOnPlayerEquip, 6, (BYTE*)ADR_PLAYEREQUIP+0x5},
	{(BYTE*)ADR_MAPLOADED, (DWORD)hookOnMapLoaded, 6, (BYTE*)ADR_MAPLOADED+0x5},
	{(BYTE*)ADR_TIMEOUT_HOOK, (DWORD)hookOnClientTimeOut, 6, (BYTE*)ADR_TIMEOUT_HOOK+0x5},
	{(BYTE*)ADR_FIRE_BULLETS, (DWORD)hookOnFireBullets, 6, (BYTE*)ADR_FIRE_BULLETS+0x5},
	{(BYTE*)ADR_INIT_HUD, (DWORD)hookOnHudInit, 6, (BYTE*)ADR_INIT_HUD+0x5},
	{(BYTE*)ADR_FAKECLIENTMAPCHANGE, (DWORD)hookOnDropFakeClient, 6, (BYTE*)ADR_FAKECLIENTMAPCHANGE+0x5}, //This gets called when a fake client gets kicked
	{(BYTE*)ADR_RECONNECT_FN, (DWORD)hookOnClientReconnect, 8, (BYTE*)ADR_RECONNECT_FN+0x5},
	//{(BYTE*)ADR_HOST_RUNFRAME, (DWORD)hookOn_Host_RunFrame, 0x0A, (BYTE*)ADR_HOST_RUNFRAME+0x5},
	//{(BYTE*)ADR_MAINRUNFRAME, (DWORD)hookOnMainRunFrame, 0x0A, (BYTE*)ADR_MAINRUNFRAME+0x5},
	{(BYTE*)ADR_SV_DISCONNECTCLIENT, (DWORD)hookOnSV_DisconnectClient, 5, (BYTE*)ADR_SV_DISCONNECTCLIENT+0x5},
	{(BYTE*)ADR_NEW_INVALID_CMD, (DWORD)hookOnInvalidCommand, 5, (BYTE*)ADR_NEW_INVALID_CMD+0x5}
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
	printf("iniMod: Done hooking game engine and applying memory patches.\n");
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
		__asm mov eax, dword ptr ds:[eax + useridoffset] //store userid in eax
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
		__asm {
			push eax
			push ecx
			push edx
		}
		callOnServerChangeMap("");
		__asm {
			pop edx
			pop ecx
			pop eax
			retn
		}
		//__asm mov dword ptr ds:[tempRegister], ebx
		
		//__asm add esp,0x14
		//__asm POP EDI
		//__asm jmp retJMP
	//}
}

__declspec(naked) void hookOnHudInit() {
	__asm {
		mov tempEAX, eax
		mov tempEBX, ebx
		mov tempECX, ecx
		mov tempEDX, edx
		mov tempEDI, edi
		mov tempESI, esi
		mov edi, dword ptr ds:[edi + 0x4]   //EDICT_NUM
		mov edi, dword ptr ds:[edi + 0x204] //EDICT_NUM
		mov tempRegister, edi
	}
	callOnHudInit(tempRegister);
	retJMP = getHookFuncJMPAddr((DWORD)hookOnHudInit);
	 __asm {
		mov eax, tempEAX
		mov ebx, tempEBX
		mov ecx, tempECX
		mov edx, tempEDX
		mov edi, tempEDI
		mov esi, tempESI

		MOV ECX,DWORD PTR DS:[0x4217CA9C]
		jmp retJMP //jump to original code
	 }
}
__declspec(naked) void hookOnMapLoaded() {
	__asm mov dword ptr ds:[tempEAX], eax //STORE original value of registers in a really bad way since PUSH wasn't working properly
	retJMP = getHookFuncJMPAddr((DWORD)hookOnMapLoaded);
	callOnMapLoaded();
	__asm mov ecx, dword ptr ds:[0x44a86b50]
	__asm mov eax, tempEAX  //restore the original registers in a bad way
	__asm jmp retJMP
}
__declspec(naked) void hookOnClientConnect() {
	__asm {
		MOV ECX,DWORD PTR DS:[host_client]
		mov ECX, dword ptr ds:[ECX + useridoffset] //store userid in edx
		mov dword ptr ds:[tempECX], ecx //store userid into tempRegister
	}
	callOnClientConnect(tempECX);
	retJMP = getHookFuncJMPAddr((DWORD)hookOnClientConnect);
	__asm {
		MOV ECX,DWORD PTR DS:[host_client] //restore intercepted code
		jmp retJMP
	}
	/*
		//old stuff used with address 0x4308BA29
		__asm add esp,0x0A4C //restore original value since we intercepted
		__asm push edx //store the old value of edx
		__asm test eax, eax
		__asm je eaxis0 //player disconnected before finishing or invalid password entered
			__asm mov edx, dword ptr ds:[eax + client_t::puserid] //store userid in edx
			__asm mov dword ptr ds:[tempRegister], edx //store userid into tempRegister
			callOnClientConnect(tempRegister);
		eaxis0:
		__asm pop edx //restore old value of edx
		__asm retn
	*/
}
__declspec(naked) void hookOnClientReconnect() { //client already has a slot, reconnect him using the same one
		__asm {
			mov tempEAX, eax
			mov tempEBX, ebx
			mov tempECX, ecx
			mov tempEDX, edx
			mov tempEDI, edi
			mov tempESI, esi
			mov tempEBP, ebp
			mov tempRegister, ebp
		}
			tempRegister2 = ReadInt32(tempRegister + client_t::pedicts);
			callOnClientReconnect(tempRegister2);
			retJMP = getHookFuncJMPAddr((DWORD)hookOnClientReconnect);
		__asm {
			mov eax, tempEAX
			mov ebx, tempEBX
			mov ecx, tempECX
			mov edx, tempEDX
			mov edi, tempEDI
			mov esi, tempESI
			mov ebp, tempEBP

			//restore original
			sub esp,0x14
			mov ecx,0x5
			jmp retJMP
		}
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
					__asm mov edx, dword ptr ds:[eax + useridoffset] //store userid in edx
					__asm mov dword ptr ds:[tempRegister], edx //store userid into tempRegister
					callOnClientConnect(tempRegister);
invalid:
			__asm pop edx
			__asm pop eax
			__asm pop ecx
			__asm push edi //execute original commands
			__asm push 0x4210D324 //execute original commands
			__asm jmp retJMP
		}
}

__declspec(naked) void hookOnClientDisconnect() { //I already changed the addresses for you
	__asm{
		mov tempRegister, esi
		mov tempEAX, eax
		mov tempEBX, ebx
		mov tempECX, ecx
		mov tempEDX, edx
		mov tempEDI, edi
		mov tempESI, esi
	}
	retJMP = getHookFuncJMPAddr((DWORD)hookOnClientDisconnect);
	callOnClientDisconnect(tempRegister);
	__asm{
		mov eax, tempEAX
		mov ebx, tempEBX
		mov ecx, tempECX
		mov edx, tempEDX
		mov edi, tempEDI
		mov esi, tempESI

		MOV EAX,DWORD PTR DS:[ESI+0x4B78] //restore old
		jmp retJMP //jump to original code
	}
}
__declspec(naked) void hookOnClientTimeOut() {
	//New style
	__asm {
		mov tempRegister, esi //STORE original value of registers in a really bad way since PUSH wasn't working properly
		mov tempEAX, eax
		mov tempEBX, ebx
		mov tempECX, ecx
		mov tempEDX, edx
		mov tempEDI, edi
		mov tempESI, esi
		mov esi, dword ptr ds:[esi+19320] //client_t::pedicts
		mov tempRegister, esi
	}

	callOnClientTimeOut(tempRegister);
	retJMP = getHookFuncJMPAddr((DWORD)hookOnClientTimeOut);

	__asm {
		mov eax, tempEAX
		mov ebx, tempEBX
		mov ecx, tempECX
		mov edx, tempEDX
		mov edi, tempEDI
		mov esi, tempESI

		push eax //overwritten
		push 0x430F0C7C //overwritten
		jmp retJMP //jump to original code
	}
}
__declspec(naked) void hookOnFireBullets() {
	//New style
	__asm {
		mov tempEAX, eax
		mov tempEBX, ebx
		mov tempECX, ecx
		mov tempEDX, edx
		mov tempEDI, edi
		mov tempESI, esi
		mov tempEBP, ebp
		mov ebx, dword ptr ds:[ecx+0x4] //entity?
		mov ebx, dword ptr ds:[ebx+0x204] //client_t::pedicts
		mov tempRegister, ebx //edictptr of player
		//mov eax, dword ptr ds:[esp+0xc] //arg3
		//mov ebx, dword ptr ds:[esp+0x10] //arg4
		//mov ecx, dword ptr ds:[esp+0x14] //arg5
		fld dword ptr ds:[esp+0xc]
		fstp dword ptr ds :[tempFloat1]
		fld dword ptr ds:[esp+0x10]
		fstp dword ptr ds:[tempFloat2]
		fld dword ptr ds:[esp+0x14]
		fstp dword ptr ds:[tempFloat3]
		//Screen Vectors & Distance
		fld dword ptr ds:[esp+0x18]
		fstp dword ptr ds :[tempFloat4]
		fld dword ptr ds:[esp+0x1c]
		fstp dword ptr ds:[tempFloat5]
		fld dword ptr ds:[esp+0x20]
		fstp dword ptr ds:[tempFloat6]
		//mov tempRegister2, eax
		//mov tempRegister3, ebx
		//mov tempRegister4, ecx
	}

	callOnFireBullets(tempRegister, tempFloat1, tempFloat2, tempFloat3, tempFloat4, tempFloat6, 0, tempFloat5);

	retJMP = getHookFuncJMPAddr((DWORD)hookOnFireBullets);

	__asm {
		mov eax, tempEAX
		mov ebx, tempEBX
		mov ecx, tempECX
		mov edx, tempEDX
		mov edi, tempEDI
		mov esi, tempESI
		mov ebp, tempEBP
		
		//4217A168
		sub esp, 0x11c
		//mov eax, dword ptr ds:[0x4217a168]
		jmp retJMP //jump to original code
	}
}
__declspec(naked) void hookOnDropFakeClient() {
	//New style
	__asm {
		mov tempEAX, eax
		mov tempEBX, ebx
		mov tempECX, ecx
		mov tempEDX, edx
		mov tempEDI, edi
		mov tempESI, esi
		mov tempEBP, ebp

		mov tempRegister, esi
	}

	callOnDropFakeClient(tempRegister);
	//retJMP = getHookFuncJMPAddr((DWORD)hookOnFireBullets);

	__asm {
		mov eax, tempEAX
		mov ebx, tempEBX
		mov ecx, tempECX
		mov edx, tempEDX
		mov edi, tempEDI
		mov esi, tempESI
		mov ebp, tempEBP
		
		pop ebp
		pop edi
		pop esi
		pop ebx
		retn

		//jmp retJMP //jump to original code
	}
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
			__asm mov eax, dword ptr ds:[eax + useridoffset] //store userid in eax
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
__declspec(naked) void hookOnInvalidCommand() {
	retJMP = getHookFuncJMPAddr((DWORD)hookOnInvalidCommand);
	if(retJMP) { //If retJMP is not a valid address, it'll crash
		__asm pushad
		__asm test ebx, ebx
		__asm je InvalidEBX
			__asm mov dword ptr ds:[tempRegister], ebx
			__asm mov tempRegister2, ebx
			__asm mov ebx, dword ptr ds:[tempRegister] //restore the register to the original value
			callConCommandModule(tempRegister2);
		InvalidEBX:
		__asm popad
		__asm push eax
		__asm mov eax, 0x430439F0 //CON_DPrintf
		__asm call eax
		__asm pop eax
		__asm jmp retJMP
		memset(value, 0, sizeof(value));
	}
}
/*
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
*/
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
			__asm mov eax, dword ptr ds:[eax + useridoffset] //store userid in eax
			__asm mov dword ptr ds:[tempRegister2], eax //store userid into tempRegister
	//For EBX
	__asm push ebx //edictptr
	__asm call tempRegister //get number for this edict
	__asm test eax, eax
	__asm je invalid //jump if it's a bad pointer
			__asm mov ecx,dword ptr ds:[0x44A86B4C] //get the first playerptr
			__asm imul eax, eax, 0x4F18 //integer multiply number by size of tablebuffer
			__asm lea eax, [ecx+eax-0x4F18] //get THIS player pointer
			__asm mov eax, dword ptr ds:[eax + useridoffset] //store userid in eax
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
			__asm mov eax, dword ptr ds:[eax + useridoffset] //store userid in eax
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
__declspec(naked) void hookOnPlayerSpawn() {
	tempRegister = NUM_FOR_EDICT;
	__asm mov dword ptr ds:[tempECX], ecx
	__asm mov dword ptr ds:[tempEAX], eax
	__asm mov dword ptr ds:[tempESI], esi
	__asm mov dword ptr ds:[tempEDX], edx
	//For ESI
	__asm mov esi, dword ptr ds:[esi+4]
	__asm push esi //edictptr
	__asm call tempRegister //get number for this edict
	__asm test eax, eax
	__asm je invalid //jump if it's a bad pointer
			__asm mov ecx,dword ptr ds:[0x44A86B4C] //get the first playerptr
			__asm imul eax, eax, 0x4F18 //integer multiply number by size of tablebuffer
			__asm lea eax, [ecx+eax-0x4F18] //get THIS player pointer
			__asm mov eax, dword ptr ds:[eax + useridoffset] //store userid in eax
			__asm mov dword ptr ds:[tempRegister2], eax //store userid into tempRegister
	invalid:

	retJMP = getHookFuncJMPAddr((DWORD)hookOnPlayerSpawn);
	callOnClientSpawn(tempRegister2);
	__asm add esp, 4

	__asm mov ecx, dword ptr ds:[tempECX]
	__asm mov eax, dword ptr ds:[tempEAX]
	__asm mov esi, dword ptr ds:[tempESI]
	__asm mov edx, dword ptr ds:[tempEDX]

	//The code we replaced
	__asm pop edi
	__asm test eax, eax
	__asm mov eax, dword ptr ds:[esi+4]
	
	__asm jmp retJMP
}
__declspec(naked) void hookOnPlayerEquip() {
	tempRegister = NUM_FOR_EDICT;
	__asm mov dword ptr ds:[tempECX], ecx
	__asm mov dword ptr ds:[tempEAX], eax
	__asm mov dword ptr ds:[tempEDI], edi
	__asm mov dword ptr ds:[tempEDX], edx
	__asm mov dword ptr ds:[tempEBX], ebx
	__asm mov dword ptr ds:[tempESI], esi
	//For EAX
	//__asm mov edx, dword ptr ds:[esi+4]
	__asm push eax //edictptr
	__asm call tempRegister //get number for this edict
	__asm test eax, eax
	__asm je invalid //jump if it's a bad pointer
			__asm mov ecx,dword ptr ds:[0x44A86B4C] //get the first playerptr
			__asm imul eax, eax, 0x4F18 //integer multiply number by size of tablebuffer
			__asm lea eax, [ecx+eax-0x4F18] //get THIS player pointer
			__asm mov eax, dword ptr ds:[eax + useridoffset] //store userid in eax
			__asm mov dword ptr ds:[tempRegister2], eax //store userid into tempRegister
	invalid:

	//retJMP = getHookFuncJMPAddr((DWORD)hookOnPlayerEquip);
	callOnClientEquip(tempRegister2);
	__asm add esp, 4

	__asm mov ecx, dword ptr ds:[tempECX]
	__asm mov eax, dword ptr ds:[tempEAX]
	__asm mov edi, dword ptr ds:[tempEDI]
	__asm mov edx, dword ptr ds:[tempEDX]
	__asm mov ebx, dword ptr ds:[tempEBX]
	__asm mov esi, dword ptr ds:[tempESI]

	//The code we replaced
	__asm mov ecx, dword ptr ds:[esi+4]
	__asm mov dword ptr ds:[ecx+0x16c], 0x3F800000
	__asm pop esi
	__asm pop ebx
	__asm retn 4
	
	//__asm jmp retJMP
}
__declspec(naked) void hookOn_Host_RunFrame() {
	__asm {
		//mov tempEAX, eax
		mov tempEBX, ebx
		//mov tempECX, ecx
		mov tempEDX, edx
		mov tempEDI, edi
		mov tempESI, esi
		mov tempEBP, ebp
		mov dword ptr ds:[tempECX], ecx
		fld dword ptr ds:[tempECX] //time
		fstp dword ptr ds :[tempFloat1]
	}
	tempByte1 = callOn_Host_RunFrame(tempFloat1);
	retJMP = getHookFuncJMPAddr((DWORD)hookOn_Host_RunFrame);

	__asm {
		//mov eax, tempEAX
		mov ebx, tempEBX
		//mov ecx, tempECX
		mov edx, tempEDX
		mov edi, tempEDI
		mov esi, tempESI
		mov ebp, tempEBP
		mov al, tempByte1
		//4217A168
		//add esp, 0x0
		//mov eax, dword ptr ds:[0x4217a168]
		jmp retJMP //jump to original code
	}
}
__declspec(naked) void hookOnMainRunFrame() {
	__asm {
		mov tempEAX, eax
		mov tempEBX, ebx
		mov tempECX, ecx
		mov tempEDX, edx
		mov tempEDI, edi
		mov tempESI, esi
		mov tempEBP, ebp
		fld dword ptr ds:[tempEAX]
		fstp dword ptr ds:[tempFloat1] //store frametime
	}
	tempByte1 = callOnMainRunFrame(tempFloat1);
	retJMP = getHookFuncJMPAddr((DWORD)hookOnMainRunFrame);

	__asm {
		mov eax, tempEAX
		mov ebx, tempEBX
		mov ecx, tempECX
		mov edx, tempEDX
		mov edi, tempEDI
		mov esi, tempESI
		mov ebp, tempEBP
		
		cmp tempByte1, 0
		jnz RunFrame
			add esp,0x0c
			jmp retJMP //jump to original code
RunFrame:
		mov eax, dword ptr ds:[tempFloat2]
		push eax
		mov eax, 0x43059090
		call eax
		//call dword ptr ds:[0x41b608] //host_runframe which then calls _host_runframe
		add esp,0x10
	}
		retJMP = getHookFuncJMPAddr((DWORD)hookOnMainRunFrame);
		__asm jmp retJMP
}
__declspec(naked) void hookOnSV_DisconnectClient() {
	__asm { 
		mov dword ptr ss:[esp - 4], eax
		mov eax, dword ptr ss:[esp+0x10]
		mov tempEAX, eax
		mov eax, dword ptr ss:[esp - 4]
	}
	callOnSV_DisconnectClient(tempEAX);
	retJMP = getHookFuncJMPAddr((DWORD)hookOnSV_DisconnectClient);
	__asm {
		MOV EAX,DWORD PTR DS:[0x42179EFC] //restore what we intercepted
		jmp retJMP
	}
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
	memset(value, 0, sizeof(value));
}
void callOnServerChangeMap( const char* recvcmd ) {
	CNetwork *cNetwork = new CNetwork;
	CPlugins *plugin = new CPlugins;
	cNetwork->OnServerChangeMap();
	plugin->OnServerChangeMap();
	delete cNetwork;
	delete plugin;
	//memset(value, 0, sizeof(value));
	//value[0] = 0;
}
void callOnClientConnect(int pPointer) {
	CNetwork *cNetwork = new CNetwork;
	CBasePlayer *pPlayer = new CBasePlayer;
	CPlugins *plugin = new CPlugins;
	for(int playerid=0; playerid<pPlayer->MaxClients(); playerid++) {
		if(!pPlayer->IsClientOnTable(playerid)) { //Find an appropiate internal id index
			cNetwork->OnClientConnect(playerid);
			plugin->OnClientConnect(playerid);
			break; //Get out of the loop
		}
	}
	delete cNetwork;
	delete pPlayer;
	delete plugin;
}
void callOnClientDisconnect(int clientptr) {
	CNetwork *cNetwork = new CNetwork;
	CBasePlayer *pPlayer = new CBasePlayer;
	CPlugins *plugin = new CPlugins;
	int edictptr = ReadInt32(clientptr + client_t::pedicts);
	int nfuserid = pPlayer->GetPlayerUserID(edictptr);
	int playerid = pPlayer->GetInternalIDFromNFID(nfuserid);
	cNetwork->OnClientDisconnect(playerid, REASON_DISCONNECT);
	plugin->OnClientDisconnect(playerid, REASON_DISCONNECT);
	delete cNetwork;
	delete pPlayer;
	delete plugin;
}
void callOnClientDeath(int attacker, int receiver) { //Todo put the weapon used as a parameter, gotta do the same for ASM
	CNetwork *cNetwork = new CNetwork;
	CPlugins *plugin = new CPlugins;
	cNetwork->OnClientDeath(attacker, receiver);
	plugin->OnClientDeath(attacker, receiver);
	delete cNetwork;
	delete plugin;
}
void callOnClientSpawn(int nfuserid) {
	CBasePlayer *pPlayer = new CBasePlayer;
	pPlayer->OnClientSpawn(nfuserid);
	delete pPlayer;
}
void callOnClientEquip(int nfuserid) {
	CBasePlayer *pPlayer = new CBasePlayer;
	pPlayer->OnClientEquip(nfuserid);
	delete pPlayer;
}
void callOnMapLoaded() {
	CNetwork *cNetwork = new CNetwork;
	CPlugins *plugin = new CPlugins;
	cNetwork->OnMapLoaded();
	plugin->OnMapLoaded();
	delete cNetwork;
	delete plugin;
}
void callOnClientTimeOut(int edict) {
	/*
	CNetwork *cNetwork = new CNetwork;
	CBasePlayer *pPlayer = new CBasePlayer;
	int playerid = pPlayer->GetPlayerUserID(edict);
	playerid = pPlayer->GetInternalIDFromNFID(playerid); //Convert NFID to InternalID
	cNetwork->OnClientDisconnect(playerid, REASON_TIMEOUT);
	delete cNetwork;
	delete pPlayer;
	*/
}
#include "sourcemathlib.h"
void callOnFireBullets(int edict, float vecSrcX, float vecSrcY, float vecSrcZ, float vecShootDirX, float vecShootDirY, float vecShootDirZ, float dist) {
	GameInterface *gui = new GameInterface;
#if DEBUG
	CBasePlayer *pPlayer = new CBasePlayer;
	engineModule *engine = new engineModule;
	int playerid = pPlayer->GetPlayerUserID(edict);
	playerid = pPlayer->GetInternalIDFromNFID(playerid); //Convert NFID to InternalID
	//printf("callOnFireBullets(%d, X:%f, Y:%f, Z:%f, ScreenX:%f, ScreenY:%f, Dist:%f)\n", playerid, vecSrcX, vecSrcY, vecSrcZ, vecShootDirX, vecShootDirY, (float)dist);

	//Vector vecSrc = pPlayer->GetGunPos(edict); //getgunpos is correct
	
	Vector vecSrc; 
	pPlayer->GetPlayerPos(playerid, vecSrc.x, vecSrc.y, vecSrc.z);
	/*
	engine->UTIL_MakeVectors( pPlayer->v_angle(edict) );
	Vector vecAiming = engine->v_forward(); //+ pPlayer->punchangle(edict);
	*/
	Vector vecAiming;
	AngleVectors(pPlayer->EyeAngles(edict), &vecAiming);

	trace_t tr;
	engine->UTIL_TraceLine(vecSrc, vecSrc + vecAiming * 8192, 0, 0, edict, tr);
	gui->UTIL_Create_TE_DLIGHT(0, tr.endpos.x , tr.endpos.y , tr.endpos.z, 2, 125, 255, 255, 255, 50, 50.0f );
	gui->UTIL_Create_TE_ELIGHT(edict, tr.endpos.x, tr.endpos.y,tr.endpos.z, 100, 255, 255, 170, 1, 100 );
	//gui->UTIL_ScreenShake(playerid, ~pPlayer->GetFlags(playerid) & FL_DUCKING ? 200 : screenY < 0.05f || screenY > 0.05f ? 5000*((screenX < 0 ? -screenX : screenX)*(screenY < 0 ? -screenY : screenY)) : 2500, 1, 200, 2, 0);
	//gui->UTIL_ScreenShake(playerid, pPlayer->GetFlags(playerid) & IN_DUCK ? 50 : 5000, 1, 250, 2, 0);
	delete pPlayer;
	delete engine;
#else
	engineModule *engine = new engineModule;
	gui->UTIL_Create_TE_DLIGHT(0, vecSrcX , vecSrcY , vecSrcZ, 2, 125, 255, 255, 170, 1, 100.0f );
	gui->UTIL_Create_TE_ELIGHT(edict, vecSrcX, vecSrcY, vecSrcZ, 100, 255, 255, 170, 1, 100 );
	delete engine;
#endif
	delete gui;
}
void callOnHudInit(int edictPtr) {
	CBasePlayer *pPlayer = new CBasePlayer;
	//delete gui;
	int playerid;
	playerid = pPlayer->GetPlayerUserID(edictPtr);
	playerid = pPlayer->GetInternalIDFromNFID(playerid); //Convert NFID to InternalID
	pPlayer->OnPlayerHudInit(playerid);
	delete pPlayer;
	//delete engine;
}
void callOnDropFakeClient(int edictPtr) {
	CBasePlayer *pPlayer = new CBasePlayer;
	CNetwork *cNetwork = new CNetwork;
	CPlugins *plugin = new CPlugins;
	//delete gui;
	int playerid;
	playerid = pPlayer->GetPlayerUserID(edictPtr);
	playerid = pPlayer->GetInternalIDFromNFID(playerid); //Convert NFID to InternalID
	cNetwork->OnClientDisconnect(playerid, REASON_DISCONNECT);
	plugin->OnClientDisconnect(playerid, REASON_DISCONNECT);
	delete pPlayer;
	delete cNetwork;
	delete plugin;
	//delete engine;
}
void callOnClientReconnect(int edict) {
	CBasePlayer *pPlayer = new CBasePlayer;
	CNetwork *cNetwork = new CNetwork;
	int playerid;
	playerid = pPlayer->GetPlayerUserID(edict);//problem here, if player reconnects, their userid is different so it returns -1
	playerid = pPlayer->GetInternalIDFromNFID(playerid); //Convert NFID to InternalID
	cNetwork->OnClientReconnect(playerid);
	delete pPlayer;
	delete cNetwork;
}
bool callOn_Host_RunFrame(float time) {
	//Nightfire's Method
	/*
	//Host_AccumulateTime
	double realtime = time + ReadDouble(0x448BE138);
	WriteInProcessDouble(REALTIME_ADR, realtime);


	//Host_FilterTime
	float fpsmaxft = (1 / float(ReadInt(0x448be624 + 0x14)));
	float host_frametime = (realtime - m_flPreviousTime);
	if ( host_frametime >= (fpsmaxft) ){
		m_flPreviousTime = realtime;
		host_frametime = min( host_frametime, MAX_FRAMETIME );
		host_frametime = max( host_frametime, MIN_FRAMETIME );
		WriteInProcessDouble(HOST_FRAMETIME_ADR, host_frametime);
		return 1;
	}else{
		return 0;
	}
	*/


	//Source 2007 Method
	engineModule *engine = new engineModule;
	engine->Host_AccumulateTime(time);
	engine->Host_CalculateTimeDelta();
	delete engine;
	return 1; //tell the engine to always run this frame since we don't filter time in the old spot anymore
}
//static double			m_flCurrentTime;
//static double			m_flPreviousTime;
static double m_flFrameTime;
bool callOnMainRunFrame(float frametime){

	engineModule *engine = new engineModule;
	engine->GetTickInterval();

	//Not needed because Bond_ded does this for us, no need to waste cpu cycles doing it again
	/*
	// Get current time
	m_flCurrentTime	= Sys_FloatTime();
	// Determine dt since we last checked
	float dt = m_flCurrentTime - m_flPreviousTime;
	// Remember old time
	m_flPreviousTime = m_flCurrentTime;
	// Accumulate current time delta into the true "frametime"
	m_flFrameTime += dt;
	*/

	m_flFrameTime += frametime;
	/*
	// If the time is < 0, that means we've restarted. 
	// Set the new time high enough so the engine will run a frame 
	if ( m_flFrameTime < 0.0f ){
		tempFloat1 = m_flFrameTime;
		return 1;
	}*/

	bool runframe = engine->FilterTime(m_flFrameTime);
	if ( runframe ){
		tempFloat2 = m_flFrameTime;
		m_flFrameTime = 0;
	}
	delete engine;
	return runframe;


}
void callOnSV_DisconnectClient(int edict){
	__asm mov dword ptr ds:[tempESP], esp //compiler is messing up ESP!!
	engineModule *engine = new engineModule;
	int clientptr = engine->SV_EdictToClient(edict);
	delete engine;
	if ( clientptr ) {
		callOnClientDisconnect(clientptr);
	}
	__asm mov esp, dword ptr ds:[tempESP]
}
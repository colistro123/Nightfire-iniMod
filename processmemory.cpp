#include "processmemory.h"
#include "engine.h"
int _memCurrent;

BYTE ReadByte(DWORD Address) {
	return *(BYTE*)Address; //typecast variable to a pointer of a byte and dereference the pointer, giving you the value from the game
}
void WriteInProcessByte(DWORD Address, BYTE Value) {
	*(BYTE*)Address = Value; //typecast variable to a pointer of a byte and dereference the pointer, letting you change what it holds
}
void WriteInProcessFloat(DWORD Address, FLOAT Value) {
	*(FLOAT*)Address = Value; //typecast variable to a pointer of a byte and dereference the pointer, letting you change what it holds
}
void WriteInProcessInt32(DWORD Address, INT32 Value) {
	*(INT32*)Address = Value; //typecast variable to a pointer of a byte and dereference the pointer, letting you change what it holds
}
void WriteInProcessChar(DWORD Address, CHAR Value) {
	*(char*)Address = Value; //typecast variable to a pointer of a byte and dereference the pointer, letting you change what it holds
}
FLOAT ReadFloat(DWORD Address) {
	return *(FLOAT*)Address;
}
double ReadDouble(DWORD Address){
	return *(double*)Address;
}
int	ReadInt(DWORD Address){
	return *(UINT*)Address;
}
//New
UINT32 ReadInt32(DWORD Address){
	return *(UINT32*)Address;
}
void WriteInt32(DWORD Address, UINT32 Value) {
	*(UINT32*)Address = Value; //typecast variable to a pointer of a byte and dereference the pointer, letting you change what it holds
}
void WriteMem(DWORD dwOffset, DWORD dwValue, int len){
    unsigned long Protection;    
    VirtualProtect((void*)dwOffset, 1, PAGE_READWRITE, &Protection);
    memcpy((void*)dwOffset, (const void*)dwValue, len);
    VirtualProtect((void*)dwOffset, 1, Protection, 0);
}
BYTE Read(DWORD address, int length){ //this is completeyl broken
    LPVOID buffer = LPVOID(length - 1);
	LPCVOID adr = LPCVOID(address);
    int _read = 0;
	int _read2 = 0;
	int _Position;
    ReadProcessMemory(GetCurrentProcess(), adr, buffer, length, (SIZE_T *)_read2);
    _read = _read2;
    _Position = (int)address + _read;

    return (BYTE)buffer;
}
std::string ReadCharArray(DWORD Address, int length){
	//BYTE data = Read(Address, length);
	char value[512] = {0};
	int count = 0;
	while ( count < length ){
		if ( *(BYTE*)Address == 0x00) {
			length = count;
		} else {
			value[count] = *(CHAR*)Address;
			Address ++;
			count++;
		}
	}
	std::string ret(value);
	return ret;
}
void WriteCharArray(DWORD Address, char * string, bool ZeroTheEnd){
	DWORD AddressBkp = Address;

	int max_length = 1024;
	int pos = 0;
	while ( pos < max_length ){
		if(string[pos] == 0x00){
			break;
		}

		*(char*)Address = string[pos];

		Address++;
		pos++;
	}
	if ( ZeroTheEnd ){
		Address++;
		*(char*)Address = 0x00; //this didnt work
	}
}
//enums
BYTE memGetByte(int ptr){
    return ReadByte(_memCurrent + ptr);
}
BYTE memGetInt(int ptr){
    return ReadInt32(_memCurrent + ptr);
}
std::string memGetCharArray(int ptr){
	return ReadCharArray(_memCurrent + ptr, 512);
}
FLOAT memGetFloat(int ptr) {
	return ReadByte(_memCurrent + ptr);
}
//end of enums
//stack
void memSetPtr(int ptr) {
    _memCurrent = ptr;
}
int memGetPtr() {
     return _memCurrent;
}
void memOpen(int ptr){
	_memStack stack;
    stack.push(_memCurrent);
    memSetPtr(memGetInt(ptr));
}
void memClose(){
	//TODO: THIS NEEDS A TRY STATEMENT TO AVOID CRASHES
	_memStack stack;
    _memCurrent = stack.pop();
}
//server
void WRITE_BYTE(int value){
	DWORD func = ADR_WRITE_BYTE;
	__asm {
		push value
		call func
		add esp,4
	}
}
void WRITE_SHORT(int value){
	DWORD func = ADR_WRITE_SHORT;
	__asm {
		push value
		call func
		add esp,4
	}
}
void WRITE_LONG(int value){
	DWORD func = ADR_WRITE_LONG;
	__asm {
		push value
		call func
		add esp,4
	}
}
void WRITE_CHAR(int value){
	DWORD func = ADR_WRITE_CHAR;
	__asm {
		push value
		call func
		add esp,4
	}
}
void WRITE_COORD(float value){
	DWORD func = ADR_WRITE_COORD;
	__asm {
		push value
		call func
		add esp,4
	}
}
void WRITE_ANGLE(Vector value){
	DWORD func = ADR_WRITE_ANGLE;
	__asm {
		push value
		call func
		add esp,4
	}
}
void WRITE_STRING(char *value){
	DWORD func = ADR_WRITE_STRING;
	__asm {
		push value
		call func
		add esp,4
	}
}
void WRITE_ENTITY(int value){
	DWORD func = ADR_WRITE_ENTITY;
	__asm {
		push value
		call func
		add esp,4
	}
}
void MESSAGE_BEGIN(int edictptr, int unknown, int msgtype, int towho){
	DWORD func = ADR_MESSAGE_BEGIN;
	__asm {
		push edictptr
		push unknown
		push msgtype
		push towho
		call func
		add esp,0x10
	}
}
/*
int UserMessageBegin(int edictptr, int broadcast, int unknown2, char* msgtype) {
	DWORD func = ADR_USERMESSAGEBEGIN;
	DWORD ret;
	__asm {
		push edictptr
		push broadcast
		push unknown2
		push msgtype
		call func
		add esp, 0x10
		mov ret, eax;
	}
	return (int)ret;
}
*/
int UserMessageBegin(int towho, char* msgtype) {
	DWORD func = ADR_USERMESSAGEBEGIN; //this is supposed to be usermessagebegin
	DWORD ret;
	__asm {
		push towho //msg_broadcast, msg_one, etc
		push msgtype //SayText
		call func
		mov ret, eax;
		add esp,0x8
	}
	return (int)ret;
}
void MESSAGE_END(){
	DWORD func = ADR_MESSAGE_END;
	__asm {
		call func
	}
}

unsigned short FixedUnsigned16( float value, float scale )
{
	int output;

	output = value * scale;
	if ( output < 0 )
		output = 0;
	if ( output > 0xFFFF )
		output = 0xFFFF;

	return (unsigned short)output;
}

short FixedSigned16(float value, float scale)
{
	int output = (int)(value * scale);

	if (output > 32767)
		output = 32767;
	else if (output < -32768)
		output = -32768;

	return (short)output;
}
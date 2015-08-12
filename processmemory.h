#ifndef PROCESSMEMORY_H
#define PROCESSMEMORY_H

#include <stdio.h>
#include <cstdio>
#include <cmath>
#include <math.h>
#include <conio.h>
#include <iostream>
#include <winsock2.h>
#include <time.h>
#include <fstream>
#include <string>
#include <vector>
#include < extdll.h >
#include "addresses.h"

// Stack of integers.  This is the class definition.
class _memStack {
        // Node structure.
        struct node {
                int val;        // Data in this node.
                node *next;     // Next node.

                // Construct a newly-created node.
                node(int v, node *n) { val = v; next = n; }

                // Delete all the nodes.
                void killall() { 
                        if(next) next->killall();
                        delete this;
                }

                // Print all the nodes.
                void printall(std::ostream &strm) { 
                        strm << val << " ";
                        if(next) next->printall(strm);
                }
        } *head;

 public:
        // Nice new empty.
        _memStack() { head = 0; }

        // Make it go away.
        ~_memStack() { if(head != 0) head->killall(); }

        // Operations.
        bool empty() const { return head == 0; }
        void push(int i) { head = new node(i, head); }
		int pop() { return 0; };
        int top() const { return head->val; }

        // Print contents
        void print(std::ostream &) const;
};

//If we're defining global vars, make sure they're defined as extern and then define them once in the CPP file.
extern int _memCurrent;
/*
BYTE ReadByte(DWORD Address);
double ReadDouble(DWORD Address);
int ReadInt(DWORD Address);
void WriteInProcessByte(DWORD Address, BYTE Value);
FLOAT ReadFloat(DWORD Address);
void WriteInProcessFloat(DWORD Address, FLOAT Value);
void WriteInProcessDouble(DWORD Address, DOUBLE Value);
void WriteInProcessInt32(DWORD Address, INT32 Value);
void WriteInProcessChar(DWORD Address, char* Value);
//New
void WriteInt32(DWORD Address, UINT32 Value);
UINT32 ReadInt32(DWORD Address);
*/
BYTE Read(DWORD address, int length);
std::string ReadCharArray(DWORD Address, int length);
void WriteCharArray(DWORD Address, char *string, bool ZeroTheEnd);
//enums
/*
BYTE memGetByte(int ptr);
BYTE memGetInt(int ptr);
FLOAT memGetFloat(int ptr);
std::string memGetCharArray(int ptr);
void WriteMem(DWORD dwOffset, DWORD dwValue, int len);

//end of enums
//stacks
void memSetPtr(int ptr);
int memGetPtr();
void memOpen(int ptr);
void memClose();
*/
#define ReadByte(Address) *(BYTE*)(Address)
#define WriteInProcessByte(Address, Value) *(BYTE*)(Address) = Value
#define WriteInProcessFloat(Address, Value) *(FLOAT*)(Address) = Value
#define WriteInProcessDouble(Address, Value) *(DOUBLE*)(Address) = Value
#define WriteInProcessInt32(Address, Value) *(INT32*)(Address) = Value //typecast variable to a pointer of a byte and dereference the pointer, letting you change what it holds
#define WriteInProcessChar(Address, Value) *(char*)(Address) = Value
#define ReadFloat(Address) *(FLOAT*)(Address)
#define ReadDouble(Address) *(double*)(Address)
#define	ReadInt(Address) *(UINT*)(Address)

//New
#define ReadInt32(Address) *(UINT32*)(Address)
#define WriteInt32(Address, Value) *(UINT32*)(Address) = Value //typecast variable to a pointer of a byte and dereference the pointer, letting you change what it holds

#define memGetByte(ptr) ReadByte((_memCurrent + ptr))
#define memGetInt(ptr) ReadInt32((_memCurrent + ptr))
#define memGetCharArray(ptr) ReadCharArray((_memCurrent + ptr), 512)
#define memGetFloat(ptr) ReadByte((_memCurrent + ptr))
//end of enums
//stack
#define memSetPtr(ptr) _memCurrent = ptr
#define memGetPtr()  _memCurrent
//WRITE_BYTE, ETC
void WRITE_BYTE(int value);
void WRITE_SHORT(int value);
void WRITE_LONG(int value);
void WRITE_CHAR(int value);
void WRITE_COORD(float value);
void WRITE_ANGLE(Vector value);
void WRITE_STRING(char *value);
void WRITE_ENTITY(int value);
void MESSAGE_BEGIN(int edictptr, int unknown, int msgtype, int towho);
//int UserMessageBegin(int edictptr, int broadcast, int unknown2, char* msgtype);
int UserMessageBegin(int towho, char* msgtype);
void MESSAGE_END();
//Signed, unsigned fixes..
unsigned short FixedUnsigned16( float value, float scale );
short FixedSigned16(float value, float scale);
//cmd's
//BYTE cmdGive = {0xA1, 0x38, 0x2E, 0xB8, 0x44, 0x5, 0x78, 0x4B, 0x0, 0x0, 0x8B, 0x8, 0x8B, 0x89, 0x1C, 0x1, 0x0, 0x0, 0x68, 0x3C, 0x2E, 0xB8, 0x44, 0xE8, 0xDF, 0x6, 0xFC, 0xFE, 0xC7, 0x5, 0x34, 0x2E, 0xB8, 0x44, 0x0, 0x0, 0x0, 0x0, 0x61, 0xC3};
#endif

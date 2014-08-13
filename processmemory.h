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

BYTE ReadByte(DWORD Address);
double ReadDouble(DWORD Address);
int ReadInt(DWORD Address);
void WriteInProcessByte(DWORD Address, BYTE Value);
FLOAT ReadFloat(DWORD Address);
void WriteInProcessFloat(DWORD Address, FLOAT Value);
void WriteInProcessInt32(DWORD Address, INT32 Value);
void WriteInProcessChar(DWORD Address, char* Value);
//New
UINT32 ReadInt32(DWORD Address);
BYTE Read(DWORD address, int length);
std::string ReadCharArray(DWORD Address, int length);
//enums
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
void MESSAGE_END();
//cmd's
//BYTE cmdGive = {0xA1, 0x38, 0x2E, 0xB8, 0x44, 0x5, 0x78, 0x4B, 0x0, 0x0, 0x8B, 0x8, 0x8B, 0x89, 0x1C, 0x1, 0x0, 0x0, 0x68, 0x3C, 0x2E, 0xB8, 0x44, 0xE8, 0xDF, 0x6, 0xFC, 0xFE, 0xC7, 0x5, 0x34, 0x2E, 0xB8, 0x44, 0x0, 0x0, 0x0, 0x0, 0x61, 0xC3};
#endif

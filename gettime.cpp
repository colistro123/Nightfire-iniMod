#include "gettime.h"

#define ADR_SYS_FLOATTIME	0x430949b0 //in engine.dll		//0x00403D90 //this is in Bond/Bond_ded! Careful with EXE changes
double Sys_FloatTime(void) {
	int adr = ADR_SYS_FLOATTIME;
	double newtime;
	__asm{
		call adr
		fstp qword ptr ds:[newtime]
	}
	return newtime;
}
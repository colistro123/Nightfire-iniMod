//Taken from AMXMODX
#ifndef AMXUTILS_H
#define AMXUTILS_H
#include "amx/amxaux.h"
//#include "amx/amxcons.h"
#include "format.h"


cell* get_amxaddr(AMX *amx,cell amx_addr);
char* get_amxstring(AMX *amx,cell amx_addr,int id, int& len);
char* format_amxstring(AMX *amx, const cell *params, int parm, int &len);
int set_amxstring(AMX *amx,cell amx_addr,const char *source,int max);
#endif

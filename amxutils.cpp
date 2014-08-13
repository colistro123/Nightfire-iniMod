//Taken from AMXMODX
#include "amxutils.h"

cell* get_amxaddr(AMX *amx,cell amx_addr)
{
  return (cell *)(amx->base + (int)(((AMX_HEADER *)amx->base)->dat + amx_addr));
}
char* get_amxstring(AMX *amx,cell amx_addr,int id, int& len)
{
	static char buffor[4][3072];
	register cell* source = (cell *)(amx->base + (int)(((AMX_HEADER *)amx->base)->dat + amx_addr));
	register char* dest = buffor[id];
	char* start = dest;
	while ((*dest++=(char)(*source++)))
		;
	len = --dest - start;
	return start;
}
char* format_amxstring(AMX *amx, const cell *params, int parm, int &len)
{
	static char outbuf[4096];
	cell *addr = get_amxaddr(amx, (cell)params[parm++]);
	len = atcprintf(outbuf, sizeof(outbuf)-1, addr, amx, (cell *)params, &parm);
	return outbuf;
}
int set_amxstring(AMX *amx,cell amx_addr,const char *source,int max);
int set_amxstring(AMX *amx,cell amx_addr,const char *source,int max)
{
  cell* dest = (cell *)(amx->base + (int)(((AMX_HEADER *)amx->base)->dat + amx_addr));
  cell* start = dest;
  while (max--&&*source)
    *dest++=(cell)*source++;
  *dest = 0;
  return dest-start;
}

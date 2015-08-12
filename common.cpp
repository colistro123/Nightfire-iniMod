#include "common.h"
// wordbreak parsing set
typedef struct characterset_s
{
	char set[256];
} characterset_t;
// wordbreak parsing set
static characterset_t	g_BreakSet, g_BreakSetIncludingColons;

#define COM_TOKEN_MAX_LENGTH 1024
char	com_token[COM_TOKEN_MAX_LENGTH];

char	g_TempStringBuf[2048];
bool com_ignorecolons = false;  // YWB:  Ignore colons as token separators in COM_Parse


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pSetBuffer - pre-build group buffer
//			character - character to lookup
// Output : int - 1 if the character was in the set
//-----------------------------------------------------------------------------
#define IN_CHARACTERSET( SetBuffer, character )		((SetBuffer).set[(character)])
/*
==============
COM_Parse

Parse a token out of a string
==============
*/
char *COM_Parse (char *data)
{
	int             c;
	int             len;
	characterset_t	*breaks;
	
	breaks = &g_BreakSetIncludingColons;
	if ( com_ignorecolons )
		breaks = &g_BreakSet;
	
	len = 0;
	com_token[0] = 0;
	
	if (!data)
		return 0;
		
// skip whitespace
skipwhite:
	while ( (c = *data) <= ' ')
	{
		if (c == 0)
			return 0;                    // end of file;
		data++;
	}
	
// skip // comments
	if (c=='/' && data[1] == '/')
	{
		while (*data && *data != '\n')
			data++;
		goto skipwhite;
	}
	

// handle quoted strings specially
	if (c == '\"')
	{
		data++;
		while (1)
		{
			c = *data++;
			if (c=='\"' || !c)
			{
				com_token[len] = 0;
				return data;
			}
			com_token[len] = c;
			len++;
		}
	}

// parse single characters
	if ( IN_CHARACTERSET( *breaks, c ) )
	{
		com_token[len] = c;
		len++;
		com_token[len] = 0;
		return data+1;
	}

// parse a regular word
	do
	{
		com_token[len] = c;
		data++;
		len++;
		c = *data;
		if ( IN_CHARACTERSET( *breaks, c ) )
			break;
	} while (c>32);
	
	com_token[len] = 0;
	return data;
}
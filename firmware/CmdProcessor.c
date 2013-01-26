
//*****************************************************************************
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "CmdProcessor.h"
//*****************************************************************************
char *Cmd_SkipSpace(char *ptr)
{	// Skip to next non-blank
	while(isspace(*ptr))
		++ptr;
	return ptr;
}

//*****************************************************************************
char *Cmd_SkipChars(char *ptr)
{	// Skip to next blank
//	while(isalnum(*ptr))
	while(*ptr && !isspace(*ptr))
		++ptr;
	return ptr;
}

//*****************************************************************************
int Cmd_Lookup(const struct cmdtable *tbl, char *line)
{
	char *cmd, *arg;

	arg = cmd = Cmd_SkipSpace(line);
	while ( *arg )
	{
		if (( *arg == '=' ) || ( *arg == ':' ))
		{
			*arg++ = 0;
			break;
		}
		else
			arg++;
	}
	if ( *arg == 0 )
		arg = NULL;
	
	while ( tbl->cmd != NULL )
	{
		// split into cmd and arg buffers.
		if ( cstrcmp(tbl->cmd, cmd) == 0 )
		{
			if ( tbl->func )
			{
				return (*tbl->func)(arg);    // execute command
			}
		}
		tbl++;
	}
	return -1;
}

//*****************************************************************************
//*****************************************************************************


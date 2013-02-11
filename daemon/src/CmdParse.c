//	***************************************************************************
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "CmdParse.h"
//	***************************************************************************
//	***************************************************************************
struct CmdFunc
{
	const char *cmd;
	Cmd_Callback func;
};

struct CmdFunc_List
{
	int size;
	int allocated;
	struct CmdFunc *func_list;
};

//	***************************************************************************
CmdFuncList_t CmdParse_CreateFuncList(void)
{
	CmdFuncList_t list;

	list = calloc(1, sizeof(struct CmdFunc_List));
	if ( list != NULL )
	{
		list->size = 1;
		list->allocated = 0;
		list->func_list = calloc(1, sizeof(struct CmdFunc));
	}
	return list;
}

//	***************************************************************************
void CmdParse_DestroyList(CmdFuncList_t *list)
{
	CmdFuncList_t ptr;

	ptr = *list;
	if ( ptr->func_list )
		free(ptr->func_list);
	free(*list);
	*list = NULL;
}

//	***************************************************************************
//	***************************************************************************
void CmdParse_AddCallback(CmdFuncList_t list,const char *cmd, Cmd_Callback call)
{
	struct CmdFunc *element;

	if ( list->allocated == list->size )
	{// make sure there is an empty slot
		list->func_list = realloc(list->func_list, (list->size * sizeof(struct CmdFunc)));
		element = &list->func_list[list->size];
		list->size++;
	}
	else
	{
		element = list->func_list;
		while ( element->cmd != NULL )
			element++;
	}
	element->cmd = cmd;
	element->func = call;
	list->allocated++;
}

//	***************************************************************************
void CmdParse_RemoveCallback(CmdFuncList_t list,const char *cmd)
{
	struct CmdFunc *element;
	int i;

	for ( i = 0; i < list->size; i++ )
	{
		element = &list->func_list[i];
		if ( strcmp(cmd, element->cmd) == 0 )
		{
			element->cmd = NULL;
			element->func = NULL;
			list->allocated--;
			break;
		}
	}
}

//	***************************************************************************
//	***************************************************************************
char *CmdParse_SkipSpace(char *ptr)
{	// Skip to next non-blank
	while(isspace(*ptr))
		++ptr;
	return ptr;
}

//	***************************************************************************
char *CmdParse_SkipChars(char *ptr)
{	// Skip to next blank
//	while(isalnum(*ptr))
	while(*ptr && !isspace(*ptr))
		++ptr;
	return ptr;
}


//	***************************************************************************
//	***************************************************************************
int CmdParse_ProcessString(const CmdFuncList_t list, char *string, int fd)
{
	struct CmdFunc *element;
	char *cmd;
	char *arg;
	char *end;
	int rv = -1, i;

	end = strchr(string, '\r');
	if ( end != NULL )
	{
		*end++ = 0; // terminate this string.
		cmd = CmdParse_SkipSpace(string);	// remove any preceding white space.

		arg = strchr(string, ':');
		if ( arg )
		{
			*arg++ = 0;
			if ( *arg == 0 )
				arg = NULL;
		}

		for ( i = 0; i < list->size; i++  )
		{// scan list
			element = &list->func_list[i] ;
			if ( element->cmd == NULL )
				break;
			if ( strncmp(element->cmd, cmd, strlen(element->cmd)) == 0 )
			{
				rv = (*element->func)(fd, arg);
				break;
			}
		}
		memmove(string, end, strlen(end));
	}
	return rv;
}


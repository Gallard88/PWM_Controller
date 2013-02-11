//	***************************************************************************
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "CmdParse.h"
//	***************************************************************************
//	***************************************************************************
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
	int size;

	element = &list->func_list[0];
	if ( list->allocated == list->size )
	{// make sure there is an empty slot
		size = list->size * sizeof(struct CmdFunc);
		fprintf(stderr,"Add new, %d, %d, %d %d\n", list->size, sizeof(struct CmdFunc), list->allocated, size);
		list->func_list = realloc(list->func_list, (list->size * sizeof(struct CmdFunc)));
		element = &list->func_list[list->size];
		list->size += 1;
	}
	else
	{
		element = list->func_list;
		while ( element->cmd != NULL )
			element++;
	}
	element->cmd = cmd;
		fprintf(stderr,"Add Func\n");
	element->func = call;
		fprintf(stderr,"End Func\n");
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
int CmdParse_ProcessString(const struct CmdFunc *table, char *string, int fd)
{
	struct CmdFunc *element;
	char *cmd;
	char *arg;
	char *end;
	int rv = 0;

	end = strchr(string, '\n');
	if ( end != NULL )
	{
		printf("Line found\n");
		*end++ = 0; // terminate this string.
		cmd = CmdParse_SkipSpace(string);	// remove any preceding white space.

		printf("Cmd: %s", cmd);
		arg = strchr(string, ':');
		if ( arg )
		{
			*arg++ = 0;
			if ( *arg == 0 )
				arg = NULL;
			else
				printf("Arg: %s", arg);
		}
/*
		element = (struct CmdFunc *)table;
		while ( element->cmd )
		{// scan list
			if ( strncmp(element->cmd, cmd, strlen(element->cmd)) == 0 )
			{
				rv = (*element->func)(fd, arg);
				break;
			}

		}
*/
//		memmove(string, end, strlen(end));
	}
	else
		printf("No Line\n");
	return rv;
}


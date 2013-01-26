//*****************************************************************************
#ifndef __CMD_PROCESSOR__
#define __CMD_PROCESSOR__
//*****************************************************************************
const struct cmdtable
{
	const char * const cmd;
	int (*func)(char *buf);
};

char *Cmd_SkipSpace(char *ptr);

char *Cmd_SkipChars(char *ptr);

void Cmd_Lookup(const struct cmdtable *tbl, char *line);

//*****************************************************************************
//*****************************************************************************
#endif


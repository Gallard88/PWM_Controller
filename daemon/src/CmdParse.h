//	***************************************************************************
#ifndef __CMD_PARSE__
#define __CMD_PARSE__
//	***************************************************************************
typedef  struct CmdFunc_List* CmdFuncList_t;
typedef  int (*Cmd_Callback)(int fd, char *arg) ;

// Utility Functions
char *CmdParse_SkipSpace(char *ptr);
char *CmdParse_SkipChars(char *ptr);

//	***************************************************************************
CmdFuncList_t CmdParse_CreateFuncList(void);
void CmdParse_DestroyList(CmdFuncList_t *list);

void CmdParse_AddCallback(CmdFuncList_t list,const char *cmd, Cmd_Callback call);
void CmdParse_RemoveCallback(CmdFuncList_t list,const char *cmd);

int CmdParse_ProcessString(const CmdFuncList_t list, char *string, int fd);

//	***************************************************************************
//	***************************************************************************
#endif
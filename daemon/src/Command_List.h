// *****************
// *****************
#ifndef __CMD_LIST_H__
#define __CMD_LIST_H__
// *****************
#include "CmdParse.h"

// *****************
void CL_Create_Shared_Memory( void );
void CL_CLearSharedMemory(void);

// *****************
void CL_SetConnected(void);
void CL_SetDisconnected(void);

// *****************
// Send data commands
int Send_GetTime(int fd);
int Send_SystemTime(int fd, unsigned long time);
int Send_TempData(int fd, int update);
int Send_VoltData(int fd, int update);
int Send_CurrentData(int fd, int update);
int Send_PWMChanelData(int fd);
int Send_Restart(int fd);

// *****************
// Receive data commands
int Receive_Current(int fd, char *buf);
int Receive_Temp(int fd, char *buf);
int Receive_Firmware(int fd, char *buf);

// *****************
extern const struct CmdFunc Cmd_Table[];

//void Build_CmdList(CmdFuncList_t list);

// *****************
// *****************
#endif

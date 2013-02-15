// *****************
// *****************
#ifndef __CMD_LIST_H__
#define __CMD_LIST_H__
// *****************
#include "parson.h"
#include "CmdParse.h"

// *****************
// Send data commands
int Send_GetTime(int fd);
int Send_SystemTime(int fd, unsigned long time);
int Send_TempData(int fd, JSON_Object *J_Object);
int Send_VoltData(int fd, JSON_Object *J_Object);
int Send_CurrentData(int fd, JSON_Object *J_Object);
int Send_PWMChanelData(int fd, int ch, float duty);
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

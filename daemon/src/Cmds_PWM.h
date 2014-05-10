#ifndef __PWM_CMDS__
#define __PWM_CMDS__

// *****************
// *****************
#include "CmdParse.h"

extern const struct CmdFunc Cmd_Table[];

// *****************
void PWM_CreateSharedMemory(void);
void PWM_ClearSharedMemory(void);

// *****************
void PWM_SetConnected(void);
void PWM_SetDisconnected(void);

int PWM_SetStartup(int fd);
int PWM_Send_Restart(int fd);

int PWM_Send_ChanelData(int fd);


// *****************
// *****************
#endif

/** @package

	PWM_Cmds.h

	Copyright(c) Toshiba 2000

	Author: TOM BURNS
	Created: TB  28/01/2013 3:17:33 PM
	Last change: TB 28/01/2013 3:31:45 PM
*/
//*****************************************************************************
#ifndef __PWM_CMDS__
#define __PWM_CMDS__
//*****************************************************************************

void PWM_Cmds_Init(void);
void PWM_Cmds_Run(void);

int Read_Firmware(char *buf);
extern const char Firmware_Version[];
extern const char Firmware_Date[];
extern const char Firmware_Time[];

//*****************************************************************************
extern int Ext_Current;

extern const struct cmdtable USB_CmdTable[];
extern const struct cmdtable Ext_CmdTable[];

//*****************************************************************************
//*****************************************************************************
#endif


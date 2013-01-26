
//*****************************************************************************
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "CmdProcessor.h"
#include "PWM_Cmds.h"
#include "pwm.h"
#include "uart1.h"
#include "uart3.h"
//#include "conf_eeprom.h"

//*****************************************************************************
int Time(char *buf)
{
	char resp[50];
  
	if ( buf == NULL )
	{// read
	  csprintf(resp, "Time set\r\n");
	  U1_TxPuts(resp);
	}
	else
	{
	 // hh:mm:ss dd-mm-yyyy
	  csprintf(resp, "Time: 13:41:00 27-04-2013\r\n");
	  U1_TxPuts(resp);
	  return 0;
	}
}

//*****************************************************************************
int Update_Pwm(char *buf)
{
	char cmd[50];
	int ch;
	int duty;

	if ( buf == NULL )
		return -1;

	buf = Cmd_SkipSpace(buf);
	ch = atoi(buf);

	buf = Cmd_SkipSpace(Cmd_SkipChars(buf));
	duty = atoi(buf);

	if ( ch == 0 )
		return -1;
	else
	if ( ch > PWM_NUM_CHANELS )
	{// echo to expansion board.
		csprintf(cmd,"ch: %d %d\r\n", ch - PWM_NUM_CHANELS, duty );
		U3_TxPuts(cmd);		
	}
	else
	{
		PWM_SetDutyCycle(ch, duty);
	}
	csprintf(cmd,"ch update: %d %d\r\n", ch - PWM_NUM_CHANELS, duty );
	U1_TxPuts(cmd);
	return 0;
}

//*****************************************************************************
void Send_Temp(void )
{
	char cmd[50];

	csprintf(cmd,"temp: %d\r\n", 30 );
	U1_TxPuts(cmd);
}

//*****************************************************************************
int System_Temp(char *buf)
{
	char cmd[50];
	int temp;

	if ( buf == NULL )
	{
		Send_Temp();
	}
	else
	{	
		buf = Cmd_SkipSpace(buf);
		temp = atoi(buf);
//		EEprom_Write_2(EE_TEMP_LIMIT, temp);
		// disable current limit in expansion board.
		csprintf(cmd,"temp: 0\r\n");
		U3_TxPuts(cmd);
		U1_TxPutsf("Temp Limit set\r\n");
	}
	return 0;
}

//*****************************************************************************
void Send_Current( void )
{
	char cmd[50];

	csprintf(cmd,"current: %d\r\n", 0 );
	U1_TxPuts(cmd);
}

//*****************************************************************************
int System_Current(char *buf)
{
	char cmd[50];
	int curr;

	if ( buf == NULL )
	{
		Send_Current();
		return -1;
	}
	else
	{
		buf = Cmd_SkipSpace(buf);
		curr = atoi(buf);
//		EEprom_Write_2(EE_CURRENT_LIMIT, curr);

		// disable temp limit in expansion board.
		csprintf(cmd,"current: 0\r\n", curr);
		U3_TxPuts(cmd);
		U1_TxPutsf("Current limit set\r\n");
		return 0;
	}
}

//*****************************************************************************
int Update_Rate(char *buf)
{
	return 0;
}

//*****************************************************************************
int Expansion_Current(char *buf)
{
	return 0;
}

//*****************************************************************************
int Read_Firmware(char *buf)
{
	return 0;
}

//*****************************************************************************
const struct cmdtable USB_CmdTable[] =
{	// Put strings with more chars to check near top of the list
	"time",				&Time,
	"ch",				&Update_Pwm,
	"temp",				&System_Temp,
	"current",			&System_Current,
	"update",			&Update_Rate,
	"firmware",			&Read_Firmware,
	NULL,				NULL
};

const struct cmdtable Ext_CmdTable[] =
{
	"current",			&Expansion_Current,
	NULL,		NULL
};

//*****************************************************************************

//*****************************************************************************
//*****************************************************************************


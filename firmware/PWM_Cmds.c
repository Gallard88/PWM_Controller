/** @package 

	PWM_Cmds.c
	
	Copyright(c) Toshiba 2000
	
	Author: TOM BURNS
	Created: TB  26/01/2013 11:55:10 AM
	Last change: TB 26/01/2013 12:13:21 PM
*/
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
#include "eeprom.h"
#include "conf_eeprom.h"

//*****************************************************************************
int Time(char *buf)
{
	char resp[50];
  
	if ( buf != NULL )
	{// read
	  csprintf(resp, "Time set\r\n");
	  U1_TxPuts(resp);
	  return 0;
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

	if ( ch > PWM_NUM_CHANELS )
	{// echo to expansion board.
		csprintf(cmd,"pwm: %d %d\r\n", ch - PWM_NUM_CHANELS, duty );
		U3_TxPuts(cmd);
		return 0;
	}
	else
	if ( ch > 0 )
	{
		PWM_SetDutyCycle(ch, duty);
	}
	return 0;
}

//*****************************************************************************
int System_Temp(char *buf)
{
    char cmd[50];
	int temp;

	if ( buf != NULL )
	{
		buf = Cmd_SkipSpace(buf);
		temp = atoi(buf);
		EEprom_Write_2(EE_TEMP_LIMIT, temp);
		// disable current limit in expansion board.
		csprintf(cmd,"temp: 0\r\n");
		U3_TxPuts(cmd);
		return 0;
	}
	return -1;
}

//*****************************************************************************
int System_Current(char *buf)
{
	char cmd[50];
	int curr;

	if ( buf != NULL )
	{
		buf = Cmd_SkipSpace(buf);
		curr = atoi(buf);
		EEprom_Write_2(EE_CURRENT_LIMIT, curr);

		// disable temp limit in expansion board.
		csprintf(cmd,"current: 0\r\n", curr);
		U3_TxPuts(cmd);
		return 0;
	}
	return -1;
}

//*****************************************************************************
int Update_Rate(char *buf)
{
	char *module;
	int rate;

	module = Cmd_SkipSpace(buf);
	buf = Cmd_SkipChars(module);
	*buf = 0;
	buf++;
	rate = atoi(buf);

	if ( cstrcmp("temp", module) == 0 )
	{
		EEprom_Write_2(EE_TEMP_UPDATE, rate);
  	return 0;
	}
	else
	if ( cstrcmp("current", module ) == 0 )
	{
		EEprom_Write_2(EE_CURRENT_UPDATE, rate);
  	return 0;
	}
	return -1;
}

//*****************************************************************************
int Ext_Current;
//*****************************************************************************
int Expansion_Current(char *buf)
{
/*
  int value;
  
  if ( buf == NULL )
    return -1;
  buf = Cmd_SkipSpace(buf);
  value = atoi(buf);
  while ( *buf )
  {
    if ( *buf == '.')
	{
	  buf++;
	  break; 
	}
    buf++;
  }

  value = (value * 10) + atoi(buf);
  Ext_Current = value;
*/  
  return 0;
}

//*****************************************************************************
int Read_Firmware(char *buf)
{
	char cmd[50];

	csprintf(cmd,"Firmware - To do\r\n" );
	U1_TxPuts(cmd);
	return 0;
}

//*****************************************************************************
const struct cmdtable USB_CmdTable[] =
{	// Put strings with more chars to check near top of the list
	"time",				&Time,
	"pwm",				&Update_Pwm,
	"temp",				&System_Temp,
	"current",		&System_Current,
	"update",			&Update_Rate,
	"firmware",		&Read_Firmware,
	NULL,					NULL
};

const struct cmdtable Ext_CmdTable[] =
{
	"current",			&Expansion_Current,
	NULL,		NULL
};

//*****************************************************************************
//*****************************************************************************


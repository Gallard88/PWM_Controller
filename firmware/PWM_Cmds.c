/** @package 

	PWM_Cmds.c
	
	Copyright(c) Toshiba 2000
	
	Author: TOM BURNS
	Created: TB  26/01/2013 11:55:10 AM
	Last change: TB 2/02/2013 10:50:10 AM
*/
//*****************************************************************************
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "adc.h"
#include "CmdProcessor.h"
#include "PWM_Cmds.h"
#include "pwm.h"
#include "uart1.h"
#include "uart3.h"
#include "eeprom.h"
#include "conf_eeprom.h"

//*****************************************************************************
//*****************************************************************************
int Ext_Current;

unsigned int Current_Update_Timer;
unsigned int Temperature_Update_Timer;

#define TEMP_AVG_BUFFER_SIZE	8
int Temp_Avg_Buffer[TEMP_AVG_BUFFER_SIZE];
ADC_Avg_Filter Temp_AVG =
{
	0,	// ch
	2, 	// scale_div
	2, 	// scale_mult
	Temp_Avg_Buffer, // *buffer
	0, // buf_ofs
	TEMP_AVG_BUFFER_SIZE, // buf_size
	0, // offset
	0, // avg
};

#define CURR_AVG_BUFFER_SIZE	4
int Current_Avg_Buffer[CURR_AVG_BUFFER_SIZE];

ADC_Avg_Filter Curr_AVG =
{
	1,	// ch
	2, 	// scale_div
	2, 	// scale_mult
	Current_Avg_Buffer, // *buffer
	0, // buf_ofs
	CURR_AVG_BUFFER_SIZE, // buf_size
	0, // offset
	0, // average
};

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
/*
int Read_Firmware(char *buf)
{
	char cmd[50];

	csprintf(cmd,"Firmware - To do\r\n" );
	U1_TxPuts(cmd);
	return 0;
}
*/
//*****************************************************************************
int Restart(char *buf)
{
	U3_TxPutsf("restart\r\n");
	while ( 1 );
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
//	"firmware",		&Read_Firmware,
	"restart",		&Restart,
	NULL,					NULL
};

const struct cmdtable Ext_CmdTable[] =
{
	"current",			&Expansion_Current,
	NULL,		NULL
};

//*****************************************************************************
void Run_Current_Sensor(void)
{
  int time, value;
  char cmd[50];

  // run timers, see if we need to send any commands via USB.
  time = EEpromRead_2_default(EE_CURRENT_UPDATE, 5);
  if (( time > 0 ) && ( Current_Update_Timer > time ))
  {
    Current_Update_Timer = 0;
    value = ADC_RunAvgFilter( &Curr_AVG);
    csprintf(cmd,"current: %d\r\n", value);
    U1_TxPuts(cmd);
  }
  Current_Update_Timer++;
}

//*****************************************************************************
void Run_Temp_Sensor(void)
{
  int time, value;
  char cmd[50];

  time = EEpromRead_2_default(EE_TEMP_UPDATE, 1);
  if (( time > 0 ) && ( Temperature_Update_Timer > time ))
  {
    Temperature_Update_Timer = 0;
    value = ADC_RunAvgFilter( &Temp_AVG );
    csprintf(cmd,"temp: %d.%d\r\n", value / 10, value % 10);
    U1_TxPuts(cmd);
  }
  Temperature_Update_Timer++;
}

//*****************************************************************************
void PWM_Cmds_Run(void)
{
  Run_Current_Sensor();
  Run_Temp_Sensor();
	if ( Temp_AVG.average > EEpromRead_2_default(EE_TEMP_LIMIT, 600) )
	{
    PWM_SetAlarm(0x40);
	}
	else
	{
    PWM_SetAlarm(0x40);
	}
/*
	if ( Temp_AVG.average > EEpromRead_2_default(EE_CURRENT_LIMIT, 200) )
	{
    PWM_SetAlarm(0x40);
	}
	else
	{
    PWM_SetAlarm(0x40);
	}
*/
}

//*****************************************************************************
void PWM_Cmds_Init(void)
{
	// Current Sensor
	ADC_LoadAvgFilter( &Curr_AVG);

	// Temp Sensor
	ADC_LoadAvgFilter( &Temp_AVG);
}

//*****************************************************************************
//*****************************************************************************


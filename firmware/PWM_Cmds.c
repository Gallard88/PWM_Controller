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
unsigned int Volt_Update_Timer;

#define TEMP_AVG_BUFFER_SIZE	8
int Temp_Avg_Buffer[TEMP_AVG_BUFFER_SIZE];
ADC_Avg_Filter Temp_AVG =
{
	0,	// ch
	1, 	// scale_div
	1, 	// scale_mult
	Temp_Avg_Buffer, // *buffer
	0, // buf_ofs
	TEMP_AVG_BUFFER_SIZE, // buf_size
	0,   // pre offset
	282, // post offset
	0, // avg
};
/**
619 = 25.9 C
617 = 24.9 C
616 = 24.8 C
613 = 23
**/

#define VOLT_AVG_BUFFER_SIZE	8
int Volt_Avg_Buffer[VOLT_AVG_BUFFER_SIZE];
ADC_Avg_Filter Volt_AVG =
{
	1,	// ch
	8, 	// scale_div
	160, 	// scale_mult
	Volt_Avg_Buffer, // *buffer
	0, // buf_ofs
	VOLT_AVG_BUFFER_SIZE, // buf_size
	0, // pre offset
	0, // post offset
	0, // avg
};
/**
216 = 11.8V
= 0.54 ratio for 100mV Res.

*/
#define CURR_AVG_BUFFER_SIZE	4
int Current_Avg_Buffer[CURR_AVG_BUFFER_SIZE];

ADC_Avg_Filter Curr_AVG =
{
	15,	// ch
	8, 	// scale_div
	136, 	// scale_mult
	Current_Avg_Buffer, // *buffer
	0, // buf_ofs
	CURR_AVG_BUFFER_SIZE, // buf_size
	517, // pre offset
	0, // post offset
	0 // average
};
/**
517 = 0A.
522 = 0.266A
528 = 0.577
538 = 1.1

scale fact.
13m, 256d.
pre offset = 517.
*/

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
	unsigned char ch;
	int duty;

	if ( buf == NULL )
		return -1;

	buf = Cmd_SkipSpace(buf);
	ch = atoi(buf);

	buf = Cmd_SkipSpace(Cmd_SkipChars(buf));
	duty = atoi(buf);

	if ( ch >= PWM_NUM_CHANELS )
	{// echo to expansion board.
		csprintf(cmd,"pwm: %d %d\r\n", ch - PWM_NUM_CHANELS, duty );
		U3_TxPuts(cmd);
	}
	else
		PWM_SetDutyCycle(ch, duty);
		
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
		U1_TxPutsf("Temp updated");
		return 0;
	}
	else
	if ( cstrcmp("current", module ) == 0 )
	{
		EEprom_Write_2(EE_CURRENT_UPDATE, rate);
		U1_TxPutsf("Current updated");
  		return 0;
	}
	if ( cstrcmp("volt", module ) == 0 )
	{
		EEprom_Write_2(EE_VOLT_UPDATE, rate);
		U1_TxPutsf("Volt updated");
		return 0;
	}
	U3_TxPutsf("update: volt 0\r\n");
	U3_TxPutsf("update: current 1\r\n");
	U3_TxPutsf("update: temp 0\r\n");
	return -1;
}

//*****************************************************************************
int Expansion_Current(char *buf)
{
  int value;
  char cmd[50];
  
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
  return 0;
}

//*****************************************************************************
int Read_Firmware(char *buf)
{
	char cmd[100];

	csprintf(cmd, "Firmware: %S, %S, %S\r\n", Firmware_Version, Firmware_Time, Firmware_Date );
	U1_TxPuts(cmd);
	return 0;
}

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
	"firmware",		&Read_Firmware,
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
  ADC_RunAvgFilter( &Curr_AVG);
  time = EEpromRead_2_default(EE_CURRENT_UPDATE, 5);
  if (( time > 0 ) && ( Current_Update_Timer > time ))
  {
    Current_Update_Timer = 0;
	if ( Curr_AVG.average < 0 )
	   value = 0;
	else
	   value = Curr_AVG.average;
	value += Ext_Current;
    csprintf(cmd,"current: %d.%d\r\n", value /10, value %10);
    U1_TxPuts(cmd);
  }
  Current_Update_Timer++;
}

//*****************************************************************************
void Run_Temp_Sensor(void)
{
  int time, value;
  char cmd[50];

  ADC_RunAvgFilter( &Temp_AVG );
  time = EEpromRead_2_default(EE_TEMP_UPDATE, 10);
  if (( time > 0 ) && ( Temperature_Update_Timer > time ))
  {
    Temperature_Update_Timer = 0;
    value = Temp_AVG.average;
    csprintf(cmd,"temp: %d\r\n", value);
    U1_TxPuts(cmd);
  }
  Temperature_Update_Timer++;
}

//*****************************************************************************
void Run_Volt_Sensor(void)
{
  int time, value;
  char cmd[50];

  ADC_RunAvgFilter( &Volt_AVG );
  time = EEpromRead_2_default(EE_VOLT_UPDATE, 10);
  if (( time > 0 ) && ( Volt_Update_Timer > time ))
  {
    Volt_Update_Timer = 0;
    value = Volt_AVG.average;
    csprintf(cmd,"volt: %d.%d\r\n", value /10, value %10);
    U1_TxPuts(cmd);
  }
  Volt_Update_Timer++;
}

//*****************************************************************************
void PWM_Cmds_Run(void)
{
  Run_Current_Sensor();
  Run_Temp_Sensor();
  Run_Volt_Sensor();  
}

//*****************************************************************************
void PWM_Cmds_Init(void)
{
	// Current Sensor
	ADC_LoadAvgFilter( &Curr_AVG);

	// Temp Sensor
	ADC_LoadAvgFilter( &Temp_AVG);
	
	// Temp Sensor
	ADC_LoadAvgFilter( &Volt_AVG);
}

//*****************************************************************************
//*****************************************************************************


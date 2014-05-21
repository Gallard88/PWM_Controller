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

  csprintf(cmd,"pwm: %d %d\r\n", ch, duty );
  U1_TxPuts(cmd);
  if ( ch >= PWM_NUM_CHANELS )
  { // echo to expansion board.
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
  else if ( cstrcmp("current", module ) == 0 )
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
int Ext_Current;
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
{ // Put strings with more chars to check near top of the list
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
unsigned int CURRENT_Update_Timer;

//*****************************************************************************
void Run_Current_Sensor(void)
{
  int time;
  long raw, value;
  char cmd[50];

  time = EEpromRead_2_default(EE_CURRENT_UPDATE, 5);
  if (( time > 0 ) && ( CURRENT_Update_Timer > time ))
  {
    CURRENT_Update_Timer = 0;
    raw = ((long)ADC_Read(15) * 124) >> 8;				 
    value = raw - 512;		  // normalise...
    csprintf(cmd,"current: %d.%d\r\n", value /10, value %10);
    U1_TxPuts(cmd);
  }
  CURRENT_Update_Timer++;
}

//*****************************************************************************
unsigned int Temperature_Update_Timer;
#define ZERO_CELCIUS  273				  
/**
619 = 25.9 C
617 = 24.9 C
616 = 24.8 C
613 = 23
**/
//*****************************************************************************
void Run_Temp_Sensor(void)
{
  int time;
  long raw, value;
  char cmd[50];

  time = EEpromRead_2_default(EE_TEMP_UPDATE, 10);
  if (( time > 0 ) && ( Temperature_Update_Timer > time ))
  {
    Temperature_Update_Timer = 0;
    raw = ((long)ADC_Read(0) * 124) >> 8;				 
    value = raw - ZERO_CELCIUS;
    csprintf(cmd,"temp: %d\r\n", value);
    U1_TxPuts(cmd);
  }
  Temperature_Update_Timer++;
}

//*****************************************************************************
unsigned int Volt_Update_Timer;
/**
401 = 7.9 V
609 = 11.97 V
= 0.546 ratio for 100mV Res.
*/
//*****************************************************************************
void Run_Volt_Sensor(void)
{
  int time; 
  long value;
  int millivolt;
  char cmd[50];
  char data[50];

  time = EEpromRead_2_default(EE_VOLT_UPDATE, 10);
  if (( time > 0 ) && ( Volt_Update_Timer > time ))
  {
    Volt_Update_Timer = 0;
    value = (ADC_Read(1)* 50) >> 8;
	millivolt = value;
	// There is a very strange bug that I can't work out where the second modifier
	// in csprintf is always 0, regardless of what it should be.
	// Also if I add a third modifier (%d %d %d), but only 2 arguments (,A, B)
	// the middle value is 0, but 1 & 2 are correct.
	// hence to fix this I have split the string over 2 statements.
    csprintf(cmd,"volt: %d.", value/10 );
	csprintf(data,"%d\r\n", millivolt%10);
	strcat(cmd, data);
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
}

//*****************************************************************************
//*****************************************************************************





// *****************************************************************************
/*
 *	PWM Controller.
 *	Compiler: ICC7 Pro.
 *	Target: ATMega 2560.
 *	Special Options.
 *	"Strings in FLASH only" = true
 *	PRINTF Version = long ( + long, and modifiers)
 *	Treat 'const' as '__flash' = true.
 *	Accept Extensions (C++ comments, binary constants) = true.
 *
	Last change: TB 26/01/2013 12:50:07 PM
 */
// *****************************************************************************
//#include ".h"

#include <iom2560v.h>
#include <string.h>
#include <stdio.h>

#include "Adc.h"
#include "eeprom.h"
#include "conf_eeprom.h"
#include "CmdProcessor.h"
#include "pwm.h"
#include "PWM_Cmds.h"
#include "timer.h"
#include "uart1.h"
#include "uart3.h"


// comms time out command
// *****************************************************************************
#define LINEBUF_SIZE	256

char USB_LineBuf[LINEBUF_SIZE];
char Ext_LineBuf[LINEBUF_SIZE];

unsigned int Current_Update_Timer;
unsigned int Temperature_Update_Timer;

// adc ch 0 = temp
// adc ch 1 = current

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

// *****************************************************************************
void Run_USB_Serial(void)
{// here every 10ms
	int rv;
	int size;

	do
	{
		size = strlen(USB_LineBuf);
		rv = U1_RxGetLine( USB_LineBuf + size,  LINEBUF_SIZE - size );
		if ( rv < 0 )
			USB_LineBuf[0] = 0;
		else if ( rv > 0 )
		{
		    if ( strlen(USB_LineBuf) )
			{
				U1_TxPutsf("Cmd >");
				U1_TxPuts(USB_LineBuf);
				U1_TxPutsf("<\r\n");
				Cmd_Lookup( USB_CmdTable, USB_LineBuf);
  				USB_LineBuf[0] = 0;
			}
		}
	}
	while ( rv != 0 );
}
// *****************************************************************************
/*
void Run_External_Serial(void)
{// here every 10ms
	int rv;
	int size;

	do
	{
		size = strlen(Ext_LineBuf);
		rv = U3_RxGetLine( Ext_LineBuf + size,  LINEBUF_SIZE - size );
		if ( rv < 0 )
			Ext_LineBuf[0] = 0;
		else
		{
			U1_TxPutsf("Cmd > ");
			U1_TxPuts(USB_LineBuf);
			U1_TxPutsf("<\r\n");
//			Cmd_Lookup(Ext_CmdTable, Ext_LineBuf);
		}
	}
	while ( rv != 0 );
}
*/
// *****************************************************************************
void IO_Init(void)
{
 				  //   76543210
			   	  // 0b0000OI00
 	DDRC = 0x01;		  
	PORTC = 0x01;

 				  //   76543210
			   	  // 0b0000OI00
 	DDRD = 0x04;		  
	PORTD = 0x0;
	
 				  //   76543210
			   	  // 0b0000OI00
 	DDRF = 0x00;
	PORTF = 0xFC;

 				  //   76543210
			   	  // 0b0000OI00
 	DDRG = 0x03;		  
	PORTG = 0x3;

 				  //   76543210
			   	  // 0b000000OI
 	DDRJ = 0x02;		  
	PORTJ = 0x0;
}

// *****************************************************************************
int main( void )
{
	int time;
	char cmd[50];
	int value;

	//-----------------------------------------------
	// initialise port pins.
	IO_Init();
	asm("wdr");

	// start watchdog

	//-----------------------------------------------
	// Initialise hardware.

	Timer_Init();	// Heart Beat Timer.

	// ADC - To do

	U1_Init ( 115200 );	// USB Coms.
	U3_Init ( 115200 );	// Expansion port.

	// I2C - To read time data from clock. - To do

	PWM_Initialise();

	//-----------------------------------------------
	// Initialise Sub-modules.

	// Current Sensor.  - To do
	ADC_LoadAvgFilter( &Curr_AVG);
	// Temp Sensor.
	ADC_LoadAvgFilter( &Temp_AVG);
	// RTC Clock  - To do

	//-----------------------------------------------
	// Run main loop.
	U1_TxPutsf("Hello World\r\n");
	PORTC ^= 0x1;
	for ( ; ; )
	{
		asm(" sei");
		asm("wdr");
		if ( Timer_Is1ms() )
		{
			// run current sensor
			// run temp sensor.
		}
		if ( Timer_Is10ms() )
		{
			// USB command handler
			Run_USB_Serial();

			// Expansion command handler
//			Run_External_Serial();
		}

		if ( Timer_Is100ms() )
		{
			// run timers, see if we need to send any commands via USB.
			time = EEpromRead_2_default(EE_CURRENT_UPDATE, 5);
			if (( time > 0 ) && ( Current_Update_Timer > time ))
			{
				Current_Update_Timer = 0;
				value = 	ADC_RunAvgFilter( &Curr_AVG);

				csprintf(cmd,"current: %d\r\n", value);
				U1_TxPuts(cmd);
			}
			Current_Update_Timer++;

		}

		if ( Timer_Is1s() )
		{
			time = EEpromRead_2_default(EE_TEMP_UPDATE, 1);
			if (( time > 0 ) && ( Temperature_Update_Timer > time ))
			{
				Temperature_Update_Timer = 0;

				value = ADC_RunAvgFilter( &Temp_AVG );
				csprintf(cmd,"temp: %d.%d\r\n", value / 10, value % 10);
				U1_TxPuts(cmd);
			}
			Temperature_Update_Timer++;

		  PORTG ^= 0x1;
		}
	}
}

// *****************************************************************************
// *****************************************************************************



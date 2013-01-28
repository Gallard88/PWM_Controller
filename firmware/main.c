


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
	Last change: TB 28/01/2013 3:42:58 PM
 */
// *****************************************************************************

#include <iom2560v.h>
#include <string.h>
#include <stdio.h>

#include "Adc.h"
#include "eeprom.h"
#include "CmdProcessor.h"
#include "pwm.h"
#include "PWM_Cmds.h"
#include "timer.h"
#include "uart1.h"
#include "uart3.h"

// *****************************************************************************
const char Firmware_Version[] = "V 1.0";
const char Firmware_Date[] = __DATE__;
const char Firmware_Time[] = __TIME__;

// *****************************************************************************
#define LINEBUF_SIZE	256

char USB_LineBuf[LINEBUF_SIZE];
char Ext_LineBuf[LINEBUF_SIZE];

// *****************************************************************************
void Run_USB_Serial(void)
{// here every 10ms
	int rv, cmd;
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
				cmd = Cmd_Lookup( USB_CmdTable, USB_LineBuf);
				if ( cmd < 0 )
					U1_TxPutsf("?\r\n");
				else
					U1_TxPutsf("OK\r\n");
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
void Print_Version(void)
{
	char buffer[100];

	csprintf(buffer, "Firmware: %S, %S, %S\r\n", Firmware_Version, Firmware_Time, Firmware_Date );
	U1_TxPuts(buffer);
}

// *****************************************************************************
int main( void )
{
	//-----------------------------------------------
	// initialise port pins.
	IO_Init();
	asm("wdr");

	// start watchdog

	//-----------------------------------------------
	// Initialise hardware.

	Timer_Init();	// Heart Beat Timer.

	// ADC
	ADC_Init();

	U1_Init ( 115200 );	// USB Coms.
	U3_Init ( 115200 );	// Expansion port.

	// I2C - To read time data from clock. - To do

	PWM_Initialise();

	//-----------------------------------------------
	// Initialise Sub-modules.
	PWM_Cmds_Init();

	// RTC Clock  - To do

	//-----------------------------------------------
	// Run main loop.
	Print_Version();
	PORTC ^= 0x1;
	
	for ( ; ; )
	{
		asm(" sei");
		asm("wdr");
		if ( Timer_Is10ms() )
		{
			// USB command handler
			Run_USB_Serial();

			// Expansion command handler
//			Run_External_Serial();
		}

		if ( Timer_Is100ms() )
		{
			PWM_Cmds_Run();
		}

		if ( Timer_Is1s() )
		{
		  PORTG ^= 0x1;
		}
	}
}

// *****************************************************************************
// *****************************************************************************



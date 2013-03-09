



// *****************************************************************************
#include <iom2560v.h>
#include "pwm.h"

// *****************************************************************************
/**
	OCR5C,	 1
	OCR5B,	 2
	OCR5A,	 3
	OCR1B,	 4
	OCR1A,	 5
	OCR3C,	 6
	OCR3B,	 7
	OCR3A	 8
	Last change: TB 2/02/2013 10:48:27 AM
*/
/***

	8 Bit Fast PWM. Mode 5
	Period = ( Hz )

	TCCRnB = WGMn3:2, CSn2:0
	TCCRnB = WGMn3:2, CSn2:0

*/
/******************************************/
static void PWM_UpdateRegister(int ch, int value)
{
	switch ( ch )
	{
		case 0:
			OCR5C = value;
			break;

		case 1:
			OCR5B = value;
			break;

		case 2:
			OCR5A = value;
			break;

		case 3:
			OCR1B = value;
			break;

		case 4:
			OCR1A = value;
			break;

		case 5:
			OCR3C = value;
			break;

		case 6:
			OCR3B = value;
			break;

		case 7:
			OCR3A = value;
			break;
	}
}

/******************************************/
static void PWM_Clear(void)
{
	int i;

	for ( i = 0; i < PWM_NUM_CHANELS; i++ )
	{// turn off all outputs.
		PWM_SetDutyCycle(i, 0);
	}
}

/******************************************/
static int PWM_RangeCheck(int duty)
{
	if ( duty > 100 )
		return 100;
	if ( duty < 0 )
		return 0;
	return duty;
}

/******************************************/
static void PWM_Enable_Port(int ch)
{
	switch ( ch )
	{
		case 0:
			TCCR5A |= (1<<COM5C1);
			break;

		case 1:
			TCCR5A |= (1<<COM5B1);
			break;

		case 2:
			TCCR5A |= (1<<COM5A1);
			break;

		case 3:
			TCCR1A |= (1<<COM1B1);
			break;

		case 4:
			TCCR1A |= (1<<COM1A1);
			break;

		case 5:
			TCCR3A |= (1<<COM3C1);
			break;

		case 6:
			TCCR3A |= (1<<COM3B1);
			break;

		case 7:
			TCCR3A |= (1<<COM3A1);
			break;
	}
}

/******************************************/
static void PWM_Disable_Port(int ch)
{
	switch ( ch )
	{
		case 0:
			TCCR5A &= ~(1<<COM5C1);
			PORTL &= ~(1<<5);
			break;

		case 1:
			TCCR5A &= ~(1<<COM5B1);
			PORTL &= ~(1<<4);
			break;

		case 2:
			TCCR5A &= ~(1<<COM5A1);
			PORTL &= ~(1<<3);
			break;

		case 3:
			TCCR1A &= ~(1<<COM1B1);
			PORTB &= ~(1<<6);
			break;

		case 4:
			TCCR1A &= ~(1<<COM1A1);
			PORTB &= ~(1<<5);
			break;

		case 5:
			TCCR3A &= ~(1<<COM3C1);
			PORTE &= ~(1<<5);
			break;

		case 6:
			TCCR3A &= ~(1<<COM3B1);
			PORTE &= ~(1<<4);
			break;

		case 7:
			TCCR3A &= ~(1<<COM3A1);
			PORTE &= ~(1<<3);
			break;
	}
}

/******************************************/
void PWM_SetDutyCycle(int ch, int duty)
{
	int value;	
	// need to multiply by 2.55 to get full range.
	// 327 / 128 = 2.55
	if ( duty == 0 )
	   PWM_Disable_Port(ch);
	else
	   PWM_Enable_Port(ch);

	// convert from % to raw value.
	value = ( PWM_RangeCheck(duty) * 327 ) >> 7;
	PWM_UpdateRegister(ch, value);
}

/******************************************/
void PWM_Initialise(void)
{
	// set up the 3 timers that run the PWM module.
	TCCR1A = (1<<COM1A1) | (1<<COM1B1) | (1<<WGM10);
	TCCR1B = (1<<WGM12) |  (1<<CS11);

	TCCR3A = (1<<COM3A1) | (1<<COM3B1) | (1<<COM3C1) | (1<<WGM30);
	TCCR3B = (1<<WGM32) |  (1<<CS31);

	TCCR5A = (1<<COM5A1) | (1<<COM5B1) | (1<<COM5C1)| (1<<WGM50);
	TCCR5B = (1<<WGM52) |  (1<<CS51);

	// make sure all chanels are turned off.
	PWM_Clear();
}

/******************************************/
/******************************************/






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

		default :
			break;
	}
}

/******************************************/
static void PWM_Clear(void)
{
	int i;

	for ( i = 0; i < PWM_NUM_CHANELS; i++ )
	{// turn off all outputs.
		PWM_UpdateRegister(i, 0);
	}
}

/******************************************/
static int PWM_RangeCheck(int duty)
{
	if ( duty > 100 )
		return 100;
	if ( duty < 100 )
		return 0;
	return duty;
}

/******************************************/
void PWM_SetDutyCycle(int ch, int duty)
{
	int value;

	if ( PWM_isAlarm() == 0 )
	{
		// convert from % to raw value.
		value = ( PWM_RangeCheck(value) * PWM_TIMER_RES ) / PWM_TIMER_RES;
		PWM_UpdateRegister(ch, value);
	}
}

/******************************************/
int PWM_Alarms;	// used to indicate a problem, and disable all channels.

/******************************************/
int PWM_isAlarm(void)
{
	return PWM_Alarms;
}

/******************************************/
void PWM_SetAlarm(int alm)
{
	PWM_Alarms |= alm;
	PWM_Clear();
}

/******************************************/
void PWM_ClearAlarm(int alm)
{
	PWM_Alarms &= ~alm;
	PWM_Clear();
}

/******************************************/
void PWM_Initialise(void)
{
	// set up the 3 timers that run the PWM module.
	TCCR1A = (1<<COM1A1) | (1<<COM1A0) | (1<<COM1B1) | (1<<COM1B0) | (1<<WGM10);
	TCCR1A = (1<<WGM12);

	TCCR3A = (1<<COM3A1) | (1<<COM3A0) | (1<<COM3B1) | (1<<COM3B0) | (1<<COM3C1) | (1<<COM3C0) | (1<<WGM30);
	TCCR3A = (1<<WGM32);

	TCCR5A = (1<<COM5A1) | (1<<COM5A0) | (1<<COM5B1) | (1<<COM5B0) | (1<<COM5C1) | (1<<COM5C0) | (1<<WGM50);
	TCCR5A = (1<<WGM52);

	// make sure all chanels are turned off.
	PWM_Clear();
}

/******************************************/
/******************************************/


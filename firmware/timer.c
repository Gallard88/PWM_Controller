

// *****************************************************************************
#include "timer.h"

#include <iom2560v.h>

volatile int Count_1m;

volatile int Count_10m;
const int Timer_10ms_Count = 10;
volatile int Count_100m;
const int Timer_100ms_Count = 100;
volatile int Count_1s;
const int Timer_1s_Count = 1000;

//	***************************************************************************
/*
	Crystal frequency of 14.7456MHz
	Want heart Beat = 100 Hz ( 10 ms )

	Prescale, 64
	Count to 230.

	14745600 / 8
	57600 / 230
	1002 Hz

*/
#define CTC_TIME 230	// +ve for CTC mode

//	***************************************************************************
void Timer_Init(void)
{

	OCR0A = CTC_TIME;
	TIFR0   = (1<<OCF0A);		// Clear any pending overflow interrupt
	TIMSK0 |= (1<<OCIE0A);		// enable timer overflow interrupt
	TCCR0A = (1<<WGM01);		// CTC mode
	TCCR0B = (1<<CS01) | (1<<CS00);			// prescaler = Ck/256, starts timer
}

//	***************************************************************************
// ImageCraft
#pragma interrupt_handler T0CompAInt:iv_TIMER0_COMPA
void T0CompAInt (void)
{
	// T0 Overflow Interrupt (basic timing)
	Count_1m++;
	Count_10m ++;
	Count_100m ++;
	Count_1s ++;
}

//	***************************************************************************
int Timer_Is1ms (void)
{

	if ( Count_1m )
	{
		asm(" cli");
		Count_1m--;
		asm(" sei");
		return 1;
	}
	return 0;
}

//	***************************************************************************
int Timer_Is10ms (void) {

	if ( Count_10m > Timer_10ms_Count )
	{
		asm(" cli");
		Count_10m -= Timer_10ms_Count;
		asm(" sei");
		return 1;
	}
	return 0;
}

//	***************************************************************************
int Timer_Is100ms (void)
{
	if ( Count_100m > Timer_100ms_Count )
	{
		asm(" cli");
		Count_100m -= Timer_100ms_Count;
		asm(" sei");
		return 1;
	}
	return 0;
}

//	***************************************************************************
int Timer_Is1s (void)
{
	if ( Count_1s > Timer_1s_Count )
	{
		asm(" cli");
		Count_1s -= Timer_1s_Count;
		asm(" sei");
		return 1;
	}
	return 0;
}

//	***************************************************************************



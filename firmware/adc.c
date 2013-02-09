/** @package 

	adc.c
	
	Copyright(c) Toshiba 2000
	
	Author: TOM BURNS
	Created: TB  26/01/2013 12:42:08 PM
	Last change: TB 26/01/2013 12:46:37 PM
*/


//*****************************************************************************
#include <iom2560v.h>

#include "adc.h"

//*****************************************************************************
#define ADPS 7	// /128

//*****************************************************************************
void ADC_Init(void)
{
	ADCSRA = ADPS + (1<<ADEN);	// set prescaler and enable
	ADMUX = (1<<REFS0);		// use Avcc as ref with external C
}

//*****************************************************************************
int ADC_Read(int ch)
{
	ADMUX = (ch & 7) + (1<<REFS0);
	if ( ch > 7 )
	  ADCSRB |= (1<< MUX5);
	else
	  ADCSRB &= ~(1<< MUX5);
	ADCSRA = ADPS + (1<<ADEN) + (1<<ADSC);	// start the conversion
	while ( ADCSRA & (1<<ADSC) )
		;	// wait for conversion complete when ADSC will go low
	return ADC;
}

//*****************************************************************************
//*****************************************************************************
void ADC_LoadAvgFilter(ADC_Avg_Filter *data)
{
	int i;

	for ( i =0; i < data->buf_size; i++ )
	{
		ADC_RunAvgFilter(data);
	}
}

//*****************************************************************************
int ADC_RunAvgFilter(ADC_Avg_Filter *data)
{
	long value;
	int i, *ptr;

	value = ((long)( ADC_Read(data->ch) - data->pre_offset)* (long) data->scale_mult ) >> data->scale_div;

	data->buffer[data->buf_ofs] = value;
	data->buf_ofs++;
	if ( data->buf_ofs > data->buf_size )
		data->buf_ofs = 0;

	value = 0;
	i = data->buf_size;
	ptr = data->buffer;
	while ( i )
	{
		i--;
		value += *ptr++;
	}
	data->average = ( value / data->buf_size ) - data->post_offset;
	return data->average;
}

//*****************************************************************************

//*****************************************************************************
//*****************************************************************************





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
	ADCSRA = ADPS + (1<<ADEN) + (1<<ADSC);	// start the conversion
	while ( ADCSRA & (1<<ADSC) )
		;	// wait for conversion complete when ADSC will go low
	return ADC;
}

//*****************************************************************************
//*****************************************************************************
int ADC_RunAvgFilter(ADC_Avg_Filter *data)
{
	int value, *ptr;
	int i;

	value = ( ADC_Read(data->ch) * data->scale_mult ) >> data->scale_div;

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
	data->average = ( value / data->buf_size ) - data->offset;
	return data->average;
}

//*****************************************************************************

//*****************************************************************************
//*****************************************************************************


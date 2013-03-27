/** @package

	adc.h

	Copyright(c) Toshiba 2000

	Author: TOM BURNS
	Created: TB  26/01/2013 12:43:36 PM
	Last change: TB 26/01/2013 12:46:49 PM
*/
//*****************************************************************************
#ifndef __ADC_H__
#define __ADC_H__
//*****************************************************************************

void ADC_Init(void);

int ADC_Read(int ch);

typedef struct
{ // struct contains data on reading a ch
//	and running a simple averaging filter.
  char ch;
  char scale_div;	// number of bits to right shift by.
  int scale_mult;
  int *buffer;	//
  int buf_ofs;
  int buf_size;	//
  int pre_offset;
  int post_offset;
  int average;
} ADC_Avg_Filter;

//*****************************************************************************
void ADC_LoadAvgFilter(ADC_Avg_Filter *data);

int ADC_RunAvgFilter(ADC_Avg_Filter *data);

//*****************************************************************************
//*****************************************************************************
#endif


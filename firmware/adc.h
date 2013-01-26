//*****************************************************************************
#ifndef __ADC_H__
#define __ADC_H__
//*****************************************************************************

void ADC_Init(void);

int ADC_Read(int ch);

typedef struct
{// struct contains data on reading a ch
//	and running a simple averaging filter.
	char ch;
	char scale_div;	// number of bits to right shift by.
	int scale_mult;
	int *buffer;	//
	int buf_ofs;
	int buf_size;	//
	int offset;
	int average;
} ADC_Avg_Filter;

//*****************************************************************************
int ADC_RunAvgFilter(ADC_Avg_Filter *data);

//*****************************************************************************
//*****************************************************************************
#endif


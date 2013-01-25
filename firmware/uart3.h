/*	uart1.h
	
	Last change: LG 23/11/2010 3:08:57 PM
*/
//*****************************************************************************
#ifndef __UART3__
#define __UART3__
//*****************************************************************************
//*****************************************************************************
void U3_Init (unsigned long baud );

// Rx Functions.
char U3_RxEmpty(void);
char U3_RxGetChar(void);
int U3_RxGetLine(char *line, int length);

// Tx Functions.
char U3_TxEmpty (void);
char U3_TxFull (void);
void U3_TxPutChar (char c);
void U3_TxPuts (char *s);			// null terminated string from RAM
void U3_TxPutsf (const char *s);	// null terminated string from Flash

//*****************************************************************************
//*****************************************************************************
#endif


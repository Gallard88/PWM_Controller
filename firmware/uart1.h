/*	uart1.h
	
	Last change: LG 23/11/2010 3:08:57 PM
*/
//*****************************************************************************
#ifndef __UART1__
#define __UART1__
//*****************************************************************************
//*****************************************************************************
void U1_Init (unsigned long baud );

// Rx Functions.
char U1_RxEmpty(void);
char U1_RxGetChar(void);
int U1_RxGetLine(char *line, int length);

// Tx Functions.
char U1_TxEmpty (void);
char U1_TxFull (void);
void U1_TxPutChar (char c);
void U1_TxPuts (char *s);			// null terminated string from RAM
void U1_TxPutsf (const char *s);	// null terminated string from Flash

//*****************************************************************************
//*****************************************************************************
#endif


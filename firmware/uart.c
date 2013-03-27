/*

	uart.c

*/
//*****************************************************************************
//*****************************************************************************

#if UART_BUF_LEN > 256
unsigned int RxOut;			// FIFO output offset
unsigned int RxIn;			// FIFO input offset
unsigned int TxOut;			// FIFO output offset
unsigned int TxIn;			// FIFO input offset
#else
unsigned char RxOut;		// FIFO output offset
unsigned char RxIn;			// FIFO input offset
unsigned char TxOut;		// FIFO output offset
unsigned char TxIn;			// FIFO input offset
#endif

char TxBuf[UART_BUF_LEN];		// transmit buffer
char RxBuf[UART_BUF_LEN];		// receive buffer

//	***************************************************************************
void UartInit (unsigned long baud )
{
  unsigned long raw_baud;

  // buffer offsets
  RxIn = RxOut = TxIn = TxOut = 0;
  UCSRC = (1<<UCSZ11)+(1<<UCSZ10);	// 8-bit data (default)
  UCSRB = (1<<TXEN1)+(1<<RXEN1)+(1<<RXCIE1);		// TXEN will force TxD pin high

//	raw_baud = ( 8 ) - 1;
  raw_baud = ( XTAL / ( 16 * baud )) - 1;
  UBRRH = (raw_baud) >> 8;
  UBRRL = (unsigned char)(raw_baud);		// triggers reset of prescaler
}


//*****************************************************************************
// Rx Fifo
void UartRxInterrupt(void)
{ // a byte is received
  // no checks for fifo overflow
#if (UART_BUF_LEN == 256)
  RxBuf[RxIn++] = UDR;		// read character once
#else
  RxBuf[RxIn] = UDR;			// read character once
  RxIn = (RxIn+1) & (UART_BUF_LEN-1);	// update buffer offset
#endif
}

// *****************************************************************************
// Tx Fifo
void UartTxInterrupt(void)
{ // UART data register empty, write another?

  if ( TxOut == TxIn )
  { // buffer empty or flow stopped by host PC
    UCSRB &= ~(1<<UDRIE1);		// disable interrupt
  }
  else
  { // something to send
#if (UART_BUF_LEN == 256)
    UDR = TxBuf[TxOut++];	// send it
#else
    UDR = TxBuf[TxOut];	// send it
    TxOut = (TxOut+1) & (UART_BUF_LEN-1);
#endif
  }
}

// *****************************************************************************
char UartRxEmpty(void)
{ // is the rx fifo empty?
  if (RxIn == RxOut)
    return 1;
  return 0;
}

// *****************************************************************************
char UartRxGetChar(void)
{ // Assumes RxBufReady() == 1
  char c;

  if ( UartRxEmpty() )
    return 0;
  c = RxBuf[RxOut];
#if (UART_BUF_LEN == 256)
  RxOut++;
#else
  RxOut = (RxOut+1) & (UART_BUF_LEN-1);
#endif
  return c;
}

// *****************************************************************************
int UartRxGetLine(char *line, int length)
{
  char c;
  while (( length-- ) && ( !UartRxEmpty() ))
  {
    c = UartRxGetChar();
    if (( c == '\r' ) || ( c == '\n' ))
      return 1;
    else
    {
      *line++ = c;
      *line = 0;
    }
  }
  if ( length == 0 )
    return -1;
  return 0;
}

// *****************************************************************************
// *****************************************************************************
char UartTxEmpty (void)
{
  return (TxIn == TxOut);
}

// *****************************************************************************
char UartTxFull (void)
{ // return 1 if buffer full or flow off
#if (UART_BUF_LEN == 256)
  return (TxOut == (TxIn+1));
#else
  return ((TxOut == ((TxIn+1) & (UART_BUF_LEN-1))));
#endif
}


// *****************************************************************************
void UartTxPutChar (char c)
{ // checks for Tx not full - not multitasking friendly
  while ( UartTxFull() )
    ;	// wait for space and flow	- call schedule here?
#if (UART_BUF_LEN == 256)
  TxBuf[TxIn] = c;
  TxIn++;
#else
  TxBuf[TxIn] = c;
  TxIn = (TxIn+1) & (UART_BUF_LEN-1);
#endif

  UCSRB |= (1<<UDRIE1);	// ensure interrupt is enabled, will start sending
}

// *****************************************************************************
// Higher-level functions
// *****************************************************************************
void UartTxPuts (char *s)		// null terminated string from RAM
{
  while ( *s )
  {
    UartTxPutChar(*s++);
  }
}


// *****************************************************************************
void UartTxPutsf (const char *s)	// null terminated string from Flash
{
  while ( *s )
  {
    UartTxPutChar(*s++);
  }
}

// *****************************************************************************


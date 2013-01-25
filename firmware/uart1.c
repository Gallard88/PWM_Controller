/*
	uart1.c

	basic Uart driver.

	Processor:		ATMega2560
	Compiler:       ICC AVR
*/
//*****************************************************************************
//*****************************************************************************
#include <iom2560v.h>
#include <stdio.h>		// for putchar

#include "uart1.h"

//*****************************************************************************
#define UART_NO				1
//*****************************************************************************
#define UartInit			U1_Init

#define UartRxEmpty			U1_RxEmpty
#define UartRxGetChar		U1_RxGetChar
#define UartRxGetLine		U1_RxGetLine

#define UartTxEmpty			U1_TxEmpty
#define UartTxFull			U1_TxFull
#define UartTxPutChar		U1_TxPutChar
#define UartTxPuts			U1_TxPuts
#define UartTxPutsf			U1_TxPutsf

//*****************************************************************************
// private functions/data
#define	UartRxInterrupt		U1_RxInterrupt
#define	UartTxInterrupt		U1_TxInterrupt

#pragma interrupt_handler 	U1_RxInterrupt:iv_USART1_RXC
#pragma interrupt_handler 	U1_TxInterrupt:iv_USART1_UDRE

#define UBRRL				UBRR1L
#define UBRRH				UBRR1H
#define UCSRC				UCSR1C
#define UCSRB				UCSR1B
#define TCCRB				TCCR1B
#define TIFR				TIFR1
#define TIMSK				TIMSK1
#define UDR					UDR1

//*****************************************************************************
#define	UART_BUF_LEN		UART_1_BUF_LEN
#define UART_1_BUF_LEN		64

#define XTAL				U1_XTAL
#define U1_XTAL				14745600

#define	TxOut				U1_TxOut
#define	TxIn				U1_TxIn
#define TxBuf				U1_TxBuf

#define	RxOut				U1_RxOut
#define	RxIn				U1_RxIn
#define RxBuf				U1_RxBuf

//*****************************************************************************
#include "uart.c"

//*****************************************************************************
//*****************************************************************************


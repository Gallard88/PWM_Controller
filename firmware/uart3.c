/*
	uart3.c

	basic Uart driver.

	Processor:		ATMega2560
	Compiler:       ICC AVR
*/
//*****************************************************************************
//*****************************************************************************
#include <iom2560v.h>
#include <stdio.h>		// for putchar

#include "uart3.h"

//*****************************************************************************
#define UART_NO				3
//*****************************************************************************
#define UartInit			U3_Init

#define UartRxEmpty			U3_RxEmpty
#define UartRxGetChar		U3_RxGetChar
#define UartRxGetLine		U3_RxGetLine

#define UartTxEmpty			U3_TxEmpty
#define UartTxFull			U3_TxFull
#define UartTxPutChar		U3_TxPutChar
#define UartTxPuts			U3_TxPuts
#define UartTxPutsf			U3_TxPutsf

//*****************************************************************************
// private functions/data
#define	UartRxInterrupt		U3_RxInterrupt
#define	UartTxInterrupt		U3_TxInterrupt

#pragma interrupt_handler 	U3_RxInterrupt:iv_USART3_RXC
#pragma interrupt_handler 	U3_TxInterrupt:iv_USART3_UDRE

#define UBRRL				UBRR3L
#define UBRRH				UBRR3H
#define UCSRC				UCSR3C
#define UCSRB				UCSR3B
#define TCCRB				TCCR3B
#define TIFR				TIFR3
#define TIMSK				TIMSK3
#define UDR					UDR3

//*****************************************************************************
#define	UART_BUF_LEN		UART_3_BUF_LEN
#define UART_3_BUF_LEN		64

#define XTAL				U3_XTAL
#define U3_XTAL				14745600

#define	TxOut				U3_TxOut
#define	TxIn				U3_TxIn
#define TxBuf				U3_TxBuf

#define	RxOut				U3_RxOut
#define	RxIn				U3_RxIn
#define RxBuf				U3_RxBuf

//*****************************************************************************
#include "uart.c"

//*****************************************************************************
//*****************************************************************************


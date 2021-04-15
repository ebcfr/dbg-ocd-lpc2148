#include <stdio.h>
#include "board.h"
#include "irq.h"
#include "uart.h"
#include "util.h"

#define MAIN1

#ifdef MAIN1
/******************************************************************************
 * uart : polling transmit test
 *
 ******************************************************************************/
int main()
{
	uart_init(_UART0,115200,UART_8N1,NULL);

	uart_puts(_UART0,"This is a message from LPC2148 :-)\r\n");

	while (1) ;

	return 1;
}
#endif

#ifdef MAIN2
/******************************************************************************
 * uart : polling tx and irq rcv
 *
 ******************************************************************************/
static void on_rx_cb(char c)
{
	uart_putc(_UART0, c);
}

int main()
{
	uart_init(_UART0,115200,UART_8N1,on_rx_cb);

	uart_puts(_UART0,"Enter some text here : ");

	irq_enable();

	while (1) ;

	return 1;
}
#endif

#ifdef MAIN3
/******************************************************************************
 * uart : hexa codes
 *
 ******************************************************************************/
static void on_rx_cb(char c)
{
	char  s[34];
	num2str(s,c,16);
	uart_puts(_UART0, " 0x");
	uart_puts(_UART0, s);
}

int main()
{
	uart_init(_UART0,115200,UART_8N1,on_rx_cb);

	uart_puts(_UART0,"\x1B[2J\x1B[H");

	uart_puts(_UART0,"We display a message here");

	// positionnement du curseur ligne 20, col 5
	uart_puts(_UART0,"\x1B[20;5H");

	// on écrit en couleur
	uart_puts(_UART0,"\x1B[31mFrom now, you can enter some text:\x1B[0m");

	irq_enable();

	while (1) ;

	return 1;
}
#endif

#ifdef MAIN4
/******************************************************************************
 * printf : test
 *
 ******************************************************************************/
static void on_rx_cb(char c)
{
	char  s[34];
	num2str(s,c,16);
	uart_puts(_UART0, " 0x");
	uart_puts(_UART0, s);
}

int main()
{
	int a=5, b=8;

	uart_init(_UART0,115200,UART_8N1,on_rx_cb);

	uart_puts(_UART0,"\x1B[2J\x1B[H");

	irq_enable();

	uart_printf(_UART0, "The sum of %d and %d is %d\r\n", a, b, a+b);
	uart_printf(_UART0, "\x1B[%u;%uHThe pointer _UART0 = 0x%x\r\n", 20,5,_UART0);

	while(1) {
	}

	return 0;
}
#endif


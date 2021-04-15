#include "uart.h"
#include "io.h"
#include "irq.h"
#include <stddef.h>
#include <string.h>
#include <stdarg.h>

// U0IER bits (Interrupt Enable Register)
#define UART_IER_RBR_BIT		(0x00000001)	// Receive Data Available bit
#define UART_IER_THRE_BIT		(0x00000002)	// THR empty bit
#define UART_IER_RXSTAT_BIT		(0x00000004)	// Line Status bit
#define UART_IER_ABEND_BIT		(0x00000100)	// End of Auto Baud bit
#define UART_IER_ABTO_BIT		(0x00000200)	// Auto Baud Time Out

// Definitions for typical UART 'fmode' settings
#define UART_FIFO_OFF     (0x06)
#define UART_FIFO_TRIG1   (0x07)
#define UART_FIFO_TRIG4   (0x47)
#define UART_FIFO_TRIG8   (0x87)
#define UART_FIFO_TRIG14  (0xC7)

static OnUartRx callback0=0;
static OnUartRx callback1=0;

#ifdef __IRQ_HANDLER__
static void uart0_isr()
{
	if ((_UART0->IIR_FCR & 0x04) && callback0) callback0(_UART0->RBR_THR_DLL);
}

static void uart1_isr()
{
	if ((_UART1->IIR_FCR & 0x04) && callback1) callback1(_UART1->RBR_THR_DLL);
}
#else
static __attribute__((interrupt("IRQ"))) void uart0_isr()
{
	if ((_UART0->IIR_FCR & 0x04) && callback0) callback0(_UART0->RBR_THR_DLL);
  
	_VIC->VectAddr=0;
}

static __attribute__((interrupt("IRQ"))) void uart1_isr()
{
	if ((_UART1->IIR_FCR & 0x04) && callback1) callback1(_UART1->RBR_THR_DLL);
  
	_VIC->VectAddr=0;
}
#endif

/*
 * uart_polling_init : polling IO
 */
/*
int uart_polling_init(UART *u, uint32_t baud, uint8_t mode)
{
    if (u==_UART0) {					// TxD -> P0.0 and RxD -> P0.1
    	io_configure(IO_PORT0, 0x00000003, IO_PIN_FUNC1);
    } else if (u==_UART1) {				// TxD -> P0.8 and RxD -> P0.9
    	io_configure(IO_PORT0, 0x00000300, IO_PIN_FUNC1);
    } else return 0;
    
	uint16_t tmp = (Fpclk)/16/baud;
  
	u->IER_DLM = 0x00;					// disable all interrupts
	u->IIR_FCR;							// clear interrupt ID
	u->RBR_THR_DLL;						// clear receive register
	u->LSR;								// clear line status register

	u->FDR = 0x10;						// Don't use Fractionnal Divider
										// UM10139.pdf p. 150
	// baud generator setup :
	u->LCR = 0x80;						// DLAB bit=U0LCR[7]=1
	u->RBR_THR_DLL = (uint8_t)tmp;		// DLL - lower byte
	u->IER_DLM = (uint8_t)(tmp>>8);		// DLM - upper byte
	// access Rx and Dx Registers
	u->LCR = mode & 0x7F;				// setup line mode
	u->IIR_FCR = UART_FIFO_TRIG1;			// setup fifo mode
  
    return 1;
}
*/
/*
 * uart_init : polling Tx and IRQ Rx if cb defined else polling Rx
 */
int uart_init(UART *u, uint32_t baud, uint8_t mode, OnUartRx cb)
{
    if (u==_UART0) {					// TxD -> P0.0 and RxD -> P0.1
    	io_configure(_IO0, 0x00000003, IO_PIN_FUNC1);
    } else if (u==_UART1) {				// TxD -> P0.8 and RxD -> P0.9
    	io_configure(_IO0, 0x00000300, IO_PIN_FUNC1);
    } else return 0;
    
	uint16_t tmp = (Fpclk)/16/baud;
  
	u->IER_DLM = 0x00;					// disable all interrupts
	u->IIR_FCR;							// clear interrupt ID
	u->RBR_THR_DLL;						// clear receive register
	u->LSR;								// clear line status register

	u->FDR = 0x10;						// Don't use Fractionnal Divider
										// UM10139.pdf p. 150
	// baud generator setup :
	u->LCR = 0x80;						// DLAB bit=U0LCR[7]=1
	u->RBR_THR_DLL = (uint8_t)tmp;		// DLL - lower byte
	u->IER_DLM = (uint8_t)(tmp>>8);		// DLM - upper byte
	// access Rx and Dx Registers
	u->LCR = mode & 0x7F;				// setup line mode
	u->IIR_FCR = UART_FIFO_TRIG1;		// setup fifo mode
  
	u->IER_DLM = UART_IER_RBR_BIT;		// setup interrupt enable bits
	
	_VIC->VectAddr=0;

	if (cb) {							// cb defined --> use irq
	    if (u==_UART0) {
	        if (cb) callback0=cb;
	        irq_register_slot(IRQ_CHANNEL_UART0,IRQ_UART0_SLOT,(Handler_t)uart0_isr);
	        return 1;
	    } else if (u==_UART1) {
	        if (cb) callback1=cb;
	        irq_register_slot(IRQ_CHANNEL_UART1,IRQ_UART1_SLOT,(Handler_t)uart1_isr);
	        return 1;
	    }
	}
    return 1;
}

/*
 * uart_getc : get a char from the serial link (blocking)
 */
char uart_getc(UART *u)
{
  while (!(u->LSR & 0x01));				// wait for RBR full
  return (char)(u->RBR_THR_DLL);		// get the received char from RBR
}

/*
 * uart_getchar : check if a char has been received from the serial link
 * (non-blocking)
 */
int uart_getchar(UART *u, char *pChar)
{
	if((u->LSR & 0x01) != 0x00) {
		*pChar = (char)(u->RBR_THR_DLL);
		return 1;
	}
	return 0;
}
/*
 * uart_putc : send a char over the serial link (polling)
 */
void uart_putc(UART *u, char c)
{
  while (!(u->LSR & 0x20));				// wait for THR empty
  u->RBR_THR_DLL=c;						// send char to THR
}

/*
 * uart_puts : send a string over the serial link (polling)
 */
void uart_puts(UART *u, char *s)
{
  if (s)
    while (*s) uart_putc(u, *s++);
}

#ifndef __SIMPLE_STDIO__
#include "util.h"

/*
 * uart_printf : print formatted text to serial link
 */
void uart_printf(UART* u, const char* fmt, ...)
{
	va_list        ap;
	char          *p;
	char           ch;
	unsigned long  ul;
	char           s[34];
	
	va_start(ap, fmt);
	while (*fmt != '\0') {
		if (*fmt =='%') {
			switch (*++fmt) {
				case '%':
					uart_putc(u,'%');
					break;
				case 'c':
					ch = va_arg(ap, int);
					uart_putc(u, ch);
					break;
				case 's':
					p = va_arg(ap, char *);
					uart_puts(u, p);
					break;
				case 'd':
					ul = va_arg(ap, long);
					if ((long)ul < 0) {
						uart_putc(u, '-');
						ul = -(long)ul;
					}
					num2str(s, ul, 10);
					uart_puts(u, s);
					break;
				case 'u':
					ul = va_arg(ap, unsigned int);
					num2str(s, ul, 10);
					uart_puts(u, s);
					break;
				case 'x':
					ul = va_arg(ap, unsigned int);
					num2str(s, ul, 16);
					uart_puts(u, s);
					break;
				default:
				    uart_putc(u, *fmt);
			}
		} else uart_putc(u, *fmt);
		fmt++;
	}
	va_end(ap);
}

#else
#include <stdarg.h>

#define __isdigit(c) ((c>='0') && (c<='9'))
#define __isalpha(c) (c >'9')
#define __isupper(c) !(c & 0x20)
#define __ishex(c) (((c >= '0')&&(c <= '9'))||((c >= 'A')&&(c <= 'F')))
#define __ascii2hex(c) ((c <= '9')? c-'0': c-'A'+10)

#define DEFAULT_UART _UART0

void _putc(char c)
{
	uart_putc(DEFAULT_UART, c);
}

/*****************************************************************************
 *
 * Description:
 *    Print a number. 
 *
 * Params:
 *    [in] outputFnk - Output function where characters to be printed are 
 *                      sent 
 * 
 *                Params (callback):
 *                   ch - Character to be printed 
 *    [in] number    - Number to be printed 
 *    [in] base      - Base when printing number (2-16) 
 *
 ****************************************************************************/
static void printNum(void (*outputFnk) (char ch),
                     unsigned int number,
                     unsigned int base)
{
	char *p;
	char  buf[33];
	char  hexChars[] = "0123456789ABCDEF";
	
	p = buf;
	
	do {
		*p++ = hexChars[number % base];
	} while (number /= base);
	
	do {
		outputFnk(*--p);
	} while (p > buf);
}

/*****************************************************************************
 *
 * Description:
 *    A simple implementation of printf that uses a minimum of stack space.
 *
 * Params:
 *    [in] outputFnk - Output function where characters to be printed are 
 *                      sent 
 * 
 *                Params (callback):
 *                   ch - Character to be printed 
 *    [in] fmt       - Format string that specifies what to be printed 
 *    [in] ap        - Structure that hold information about the variable 
 *                     number of parameters 
 *
 ****************************************************************************/
static void simplePrint(void (*outputFnk) (char ch),
                        const char * fmt,
                        va_list      ap)
{
	char          *p;
	char           ch;
	unsigned long  ul;
	unsigned char  lflag;

	for (;;) {
		while ((ch = *fmt++) != '%') {
			if (ch == '\0') return;
			outputFnk(ch);
		}
		lflag = 0;

reswitch:

		switch (ch = *fmt++) {
		case '\0':
			return;

		case 'l':
			lflag = 1;
			goto reswitch;

		case 'c':
			ch = va_arg(ap, unsigned int);
			outputFnk(ch & 0x7f);
			break;

		case 's':
			p = va_arg(ap, char *);
			while ((ch = *p++) != 0) outputFnk(ch);
			break;

		case 'd':
			ul = lflag ? va_arg(ap, long) : va_arg(ap, unsigned int);
			if ((long)ul < 0) {
				outputFnk('-');
				ul = -(long)ul;
			}
			printNum(outputFnk, ul, 10);
			break;

		case 'o':
			ul = va_arg(ap, unsigned int);
			printNum(outputFnk, ul, 8);
			break;

		case 'u':
			ul = va_arg(ap, unsigned int);
			printNum(outputFnk, ul, 10);
			break;

		case 'p':
			outputFnk('0');
			outputFnk('x');
			lflag = 1;
			// fall through

		case 'x':
			ul = va_arg(ap, unsigned int);
			printNum(outputFnk, ul, 16);
			break;

		default:
//			outputFnk('%');
			if (lflag)
				outputFnk('l');
			outputFnk(ch);
			break;
		}
	}
	va_end(ap);
}

/*****************************************************************************
 *
 * Description:
 *    Simple implementation of printf 
 *
 * Params:
 *    [in] fmt - Format string that specifies what to be printed 
 *    [in] ... - Variable number of parameters to match format string 
 *
 ****************************************************************************/
int printf(const char * fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	simplePrint(_putc, fmt, ap);
	va_end(ap);
	return 1;
}

int puts(char *s) {uart_puts(_UART0,s); return 0;}
/*****************************************************************************
 *
 * Description:
 *    Receive (blocking call) a string (end with '\r'). 
 *
 * Params:
 *    [inout] pString - pointer to return the received string
 *
 ****************************************************************************/
static void uart_get_string(UART* u, char *pString)
{
	char *pString2;
	char rxChar;

	pString2 = pString;
	while((rxChar = uart_getc(u)) != '\r') {
		if (rxChar == '\b') {
			if ((int)pString2 < (int)pString) {
				uart_puts(u, "\b \b");
				pString--;
			}
		} else {
			*pString++ = rxChar;
			uart_putc(u, rxChar);
		}
	}
	*pString = '\0';
	uart_putc(u, '\n');
}

size_t strlen(const char *s)
{
	size_t l=0;
	while (*s++) l++;
	return l;
}
/*****************************************************************************
 *
 * Description:
 *    Receive (blocking call) a number (end with '\r').
 *    Accepts hexadecimal (radix 16) and integer (radix 10) numbers.
 *
 * Returns:
 *    int - the received number
 *
 ****************************************************************************/
static int uart_get_int(UART *u)
{
	char  abStr[30];
	char *pString = abStr;
	int   wBase=10;
	int   wMinus=0;
	int   wLastIndex;
	int   wResult=0;
	int   wI;
        
	uart_get_string(u, pString);
        
	if(pString[0] == '-') {
		wMinus = 1;
		pString++;
	}

	if(pString[0] == '0' && (pString[1] == 'x' || pString[1] == 'X')) {
		wBase = 16;
		pString += 2;
	}

	wLastIndex = strlen(pString) - 1;
	if(pString[wLastIndex] == 'h' || pString[wLastIndex] == 'H') {
		wBase = 16;
		pString[wLastIndex] = 0;
		wLastIndex--;
	}

	if(wBase == 10) {
		while (__isdigit(*pString)) {
			wResult = 10*wResult + (*pString - '0');
			pString++;
		}
		wResult = wMinus ? (-1*wResult):wResult;
	} else {
		for(wI=0; wI<=wLastIndex; wI++) {
			if(__isalpha(pString[wI])) {
				if(__isupper(pString[wI]))
					wResult = (wResult<<4) + pString[wI] - 'A' + 10;
				else
					wResult = (wResult<<4) + pString[wI] - 'a' + 10;
			} else {
				wResult = (wResult<<4) + pString[wI] - '0';
			}
		}
		wResult = wMinus ? (-1*wResult):wResult;
	}
	return wResult;
}

/*****************************************************************************
 *
 * Description:
 *    A simple implementation of the scanf()-function. 
 *
 * Params:
 *    [inout] pFmt - format string, accepts:
 *                   %s - string
 *                   %c - character
 *                   %i - integer
 *
 ****************************************************************************/
int scanf(const char *pFmt, ...)
{
	va_list pArg;
	char    character;
	int    *pInt;
	char   *pChar;
        
	va_start(pArg, pFmt);
	while((character = *pFmt++) != '\0') {
		if(character != '%') continue;
		switch(*pFmt) {
		case 's':
		case 'S':
			pChar = va_arg (pArg, char *);
			uart_get_string(DEFAULT_UART,pChar);
			break;
		case 'i':
		case 'I':
			pInt  = va_arg (pArg, int *);
			*pInt = uart_get_int(DEFAULT_UART);
			break;
		case 'c':
		case 'C':
			pChar  = va_arg (pArg, char *);
			*pChar = uart_getc(DEFAULT_UART);
			break;
		}
	}
	va_end(pArg);
	return 1;
}
#endif

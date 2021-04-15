#include <stdlib.h>
#include "board.h"
#include "irq.h"
#include "lcd.h"
#include "adc.h"
#include "io.h"
#include "uart.h"
#include "timer.h"

#define MAIN2

#ifdef MAIN1
/****************************************************************
 * ADC sampling on timer irq, 2 Hz sampling frequency
 *  open a serial terminal, /dev/ttyUSB0,
 *  115200 bauds, 8 bits, no parity, 1 stop-bit
 ****************************************************************/
volatile unsigned int value, adc_updated=0;

void on_adc_cb(uint32_t ch, uint32_t val)
{
	if (adc_updated==1) uart_puts(_UART0, "Overrun !\r\n");
	value=val;
	adc_updated=1;
}

void on_timer_cb()
{
	adc_channel_sample(_ADC0, 1);
}

void on_uart_cb(char c)
{
	switch(c) {
	case 'r':
		timer_start(_TIMER0);
		uart_puts(_UART0, "\r\ncmd run\r\n");
		break;
	case 's':
		timer_stop(_TIMER0);
		uart_puts(_UART0, "\r\ncmd stop\r\n");
		break;
	default:
		break;
	}
}

int main()
{
	lcd_init();
	lcd_display(LCD_DISPLAY_DISPLAY_ON);
	uart_init(_UART0, 115200, UART_8N1, on_uart_cb);
	timer_tick_init(_TIMER0, 500, on_timer_cb);
	adc_init(_ADC0, on_adc_cb);
	adc_channel_enable(_ADC0,1);
	
    irq_enable();

   	lcd_clear();
   	lcd_home();
    while (1) {
 		if (adc_updated) {
 			lcd_goto_xy(1,1);
			lcd_printf("val=%4d",value);
			uart_printf(_UART0,"%d\r\n",value);
			adc_updated=0;
		}
   }

    return 0;
}
#endif

#ifdef MAIN2
/****************************************************************
 * sampling frequency 10 Hz
 * run the tcl UI:
 *
 *   tclsh docs/plot.tcl
 *
 * run the microcontroller app, start sampling from the UI
 ****************************************************************/
volatile unsigned int value, adc_updated=0, overrun=0;

void on_adc_cb(uint32_t ch, uint32_t val)
{
	if (adc_updated==1) overrun=1;
	value=val;
	adc_updated=1;
}

void on_timer_cb()
{
	adc_channel_sample(_ADC0, 1);
}

void on_uart_cb(char c)
{
	switch(c) {
	case 'r':
		timer_start(_TIMER0);
		lcd_goto_xy(1,1);
		lcd_printf("sampling");
		break;
	case 's':
		timer_stop(_TIMER0);
		lcd_goto_xy(1,1);
		lcd_printf("stopped ");
		break;
	default:
		break;
	}
}

int main()
{
	lcd_init();
	lcd_display(LCD_DISPLAY_DISPLAY_ON);
	uart_init(_UART0, 115200, UART_8N1, on_uart_cb);
	timer_tick_init(_TIMER0, 100, on_timer_cb);
	adc_init(_ADC0, on_adc_cb);
	adc_channel_enable(_ADC0,1);
	
    irq_enable();

   	lcd_clear();
   	lcd_home();
    while (1) {
 		if (adc_updated) {
 			lcd_goto_xy(9,1);
 			if (overrun) lcd_printf("-overrun");
 			else lcd_printf("        ");
 			lcd_goto_xy(1,2);
			lcd_printf("val=%4d",value);
			uart_printf(_UART0,"%d\r\n",value);
			adc_updated=0;
			overrun=0;
		}
   }

    return 0;
}

#endif

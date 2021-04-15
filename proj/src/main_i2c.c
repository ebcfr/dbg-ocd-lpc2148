#include <stdlib.h>
#include "board.h"
#include "irq.h"
#include "uart.h"
#include "util.h"
#include "i2c.h"
#include "lm75.h"
#include "cat1025.h"

volatile char cmd;
volatile uint32_t data_available=0;
char buf[10];
static volatile char *p=buf;

static void on_rx_cb(char c)
{
	if (!data_available) {	// prevent updating buf while it is converted
		if ((c>='0') && (c<='9')) {
			*p++=c;
			uart_putc(_UART0,c);
		} else if ((c==0x0D) || (c==0x0A)) {
			*p='\0';
			p=buf;
			data_available=1;
		} else if ((c>='a') && (c<='z')) {
			cmd = c;
		}
	}
}

int main()
{
	int temp;

	uart_init(_UART0, 115200, UART_8N1, on_rx_cb);
	i2c_master_init();
	
	irq_enable();
	
	uart_puts(_UART0,"\x1B[2J\x1B[H");	// clear terminal
	
	while(1) {
		uart_printf(_UART0,"\r\nEntrez une commande : ");
		while (!cmd) ;
		switch (cmd) {
		case 't': // get current temperature
			lm75_read_temp(&temp);
			uart_printf(_UART0,"La température est %d.%d°C",temp>>1,temp&1 ? 5 : 0);
			cmd=0;
			break;
		case 'h': // get current hysteresis for overtemperature shutdown
			lm75_read_thyst(&temp);
			uart_printf(_UART0,"L'hystérésis est de %d.%d°C",temp>>1,temp&1 ? 5 : 0);
			cmd=0;
			break;
		case 'i': // set hysteresis temperature
			uart_printf(_UART0,"\r\nEntrez le nouvel hystérésis : ");
			while (!data_available) ;
			temp = str2num(buf,10) << 1;
			data_available=0;
			lm75_write_thyst(temp);
			cmd=0;
			break;
		case 'o': // get current overtemperature shutdown
			lm75_read_tos(&temp);
			uart_printf(_UART0,"La température d'alarme est %d.%d°C",temp>>1,temp&1 ? 5 : 0);
			cmd=0;
			break;
		case 's': // set overtemperature shutdown
			uart_printf(_UART0,"\r\nEntrez la nouvelle température d'alarme : ");
			while (!data_available) ;
			temp = str2num(buf,10) << 1;
			data_available=0;
			lm75_write_tos(temp);
			cmd=0;
			break;
		case 'm': // read CAT1025 EEPROM
			{
				int i;
				uint32_t addr;
				uint8_t data[16];

				uart_printf(_UART0,"\r\nEntrez l'adresse de départ : ");
				while (!data_available) ;
				addr = str2num(buf,10);
				data_available=0;

				cat1025_read(addr, data, 16);
				uart_printf(_UART0,"\r\n0x%x |", addr);
				for (i=0;i<16;i++) {
					uart_printf(_UART0," %x",data[i]);
				}
				cmd=0;
			}
			break;
		case 'w': // write CAT1025 EEPROM
			{
				int i;
				uint32_t addr;
				uint8_t data[16];

				uart_printf(_UART0,"\r\nEntrez l'adresse de départ : ");
				while (!data_available) ;
				addr = str2num(buf,10);
				data_available=0;

				// generate 16 bytes randomly
				uart_printf(_UART0,"\r\nDonnées à écrire\r\n0x%x |", addr);
				srand(* ((unsigned int*)data));
				for (i=0;i<16;i++) {
					data[i]= rand() % 256;
					uart_printf(_UART0," %x",data[i]);
				}
				cat1025_write(addr, data, 16);
				cmd=0;
			}
			break;
		default:
			cmd=0;
			break;
		}
	}
	return 0;
}



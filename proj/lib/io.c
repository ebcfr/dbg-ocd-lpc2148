/* io.c
 *
 * IO Configure and GPIO read/write ops
 *
 * UM10139.pdf p. 50-63
 */

#include "io.h"

/* io_configure
 * configure pins referenced in pinmask of specified port according to 'func'
 *
 * returns 1 if success and 0 else
 */
int io_configure(IO* p, uint32_t pin_mask, uint32_t func)
{
	int pinsel_mask=3, port=0;
	uint32_t pins=pin_mask;
	int fio_dir=0; // default to INPUT
	
	if (p==_IO0) {
		port=0;
	} else if (p==_IO1) {
		port=1;
	} else {
		return 0;
	}
	if (func==IO_PIN_OUTPUT) {
		fio_dir=1;
		func=0;
	}
	
	/* port 0 */
	if (port==0) {
		int i;
		// P0.0:15 --> config in PINSEL0
		for (i=0;i<16;i++) {
			if (pins & 1) { // apply 'func' selection to this pin
				_PCB->PINSEL0 = (_PCB->PINSEL0 & (~pinsel_mask)) | func<<(2*i);
			}
			pins = pins>>1;
			pinsel_mask=pinsel_mask<<2;
		}
		
		// P0.16:31 --> config in PINSEL1
		pinsel_mask=3;
		for (i=0;i<16;i++) {
			if (pins & 1) { // apply 'func' selection to this pin
				_PCB->PINSEL1 = (_PCB->PINSEL1 & (~pinsel_mask)) | func<<(2*i);
			}
			pins = pins>>1;
			pinsel_mask=pinsel_mask<<2;
		}
		
		if (func==0) {
			if (fio_dir==0) {							// PIN_INPUT
				_IO0->DIR = _IO0->DIR & (~pin_mask);
			} else {									// PIN_OUTPUT
				_IO0->DIR = _IO0->DIR | pin_mask;
			}
		}
		return 1;
	}
	
	/* port=1 : gpio only,
	   - pins 0:15 does not exist,
	   - pins 16:25 ==> GPIO only [hardware config]
	   - pins 26:31 ==> JTAG [hardware config]
	 */
	if (pin_mask & 0xFC00FFFF) return 0;			// if unallowed pins used
	
	if (func==0) {
		if (fio_dir==0) {							// PIN_INPUT
			_IO1->DIR = _IO1->DIR & (~pin_mask);
		} else {									// PIN_OUTPUT
			_IO1->DIR = _IO1->DIR | pin_mask;
		}
		return 1;
	}
	return 0;
}

/* io_read
 *
 * read 32 bit data from port 'p', filter the result with mask
 */
uint32_t io_read(IO *p, uint32_t mask)
{
	return (p->PIN & mask);
}

/* io_write
 *
 * write 32 bit data filtered by mask to port 'p'
 * '1' in val are written as HIGH level on port pins
 */
void io_write(IO *p, uint32_t val, uint32_t mask)
{
	p->CLR = mask;
	p->SET = val & mask;
}

/* io_write_n
 *
 * write 32 bit data filtered by mask to port 'p'
 * '1' in val are written as LOW level on port pins
 */
void io_write_n(IO *p, uint32_t val, uint32_t mask)
{
	p->SET = mask;
	p->CLR = val & mask;
}

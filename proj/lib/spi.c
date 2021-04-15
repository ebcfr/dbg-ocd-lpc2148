#include "spi.h"
#include "io.h"
#include "irq.h"

// SPI Control Register _S0SPCR
#define SPCR_BITENABLE		(1<<2)
#define SPCR_CPHA			(1<<3)
#define SPCR_CPOL			(1<<4)
#define SPCR_MASTER			(1<<5)
#define SPCR_LSBFIRST		(1<<6)
#define SPCR_IE				(1<<7)
#define SPCR_8BITS			(0x8<<8)
#define SPCR_9BITS			(0x9<<8)
#define SPCR_10BITS			(0xA<<8)
#define SPCR_11BITS			(0xB<<8)
#define SPCR_12BITS			(0xC<<8)
#define SPCR_13BITS			(0xD<<8)
#define SPCR_14BITS			(0xE<<8)
#define SPCR_15BITS			(0xF<<8)

// SPI Status Register _S0SPSR 
#define SPSR_ABRT			(1<<3)
#define SPSR_MODF			(1<<4)
#define SPSR_ROVR			(1<<5)
#define SPSR_WCOL			(1<<6)
#define SPSR_SPIF			(1<<7)
	
// SPI Interrupt register _S0SPINT
#define SPINT_IE_FLAG		(1<<0)

static uint8_t *	buffer;				// buffer to write or to store read data
//static uint32_t	n_to_read;			// how many bytes to be read?
static uint32_t	n_to_write;			// how many bytes to be written?
static uint32_t	n_wr;				// how many data actually written?
//static uint32_t   n_rd;				// how many data actually read?
static volatile uint8_t status;


#ifdef __IRQ_HANDLER__
static void spi0_isr(void)
{
	if (_SPI0->SR & SPSR_SPIF) {
		if (n_to_write) {
			_SPI0->DR = buffer[n_wr++];
			n_to_write--;
		} else {
			_IO0->SET=IO_PIN_15;		// deactivate SPI slave
			status=SPI_OK;
		}
	} else {
		_IO0->SET=IO_PIN_15;			// deactivate SPI slave
		status=SPI_ERROR;
	}

	_SPI0->INT = 0x01;        //reset IRQ flag in spi
}
#else
static __attribute__ ((interrupt("IRQ"))) void spi0_isr(void)
{
	if (_SPI0->SR & SPSR_SPIF) {
		if (n_to_write) {
			_SPI0->DR = buffer[n_wr++];
			n_to_write--;
		} else {
			_IO0->SET=IO_PIN_15;		// deactivate SPI slave
			status=SPI_OK;
		}
	} else {
		_IO0->SET=IO_PIN_15;			// deactivate SPI slave
		status=SPI_ERROR;
	}

	_SPI0->INT = 0x01;        //reset IRQ flag in spi
	irq_ack();
}
#endif

void spi_init(void)
{
 	// pin config : SCK -> P0.4, MISO -> P0.5, MOSI -> P0.6, SSEL0 -> P0.7
	// _PINSEL0 = (_PINSEL0 & (~0x0000FF00)) | 0x00005500;
 	io_configure(_IO0, IO_PIN_4|IO_PIN_5|IO_PIN_6|IO_PIN_7, IO_PIN_FUNC1);

	//initialize SPI interface
	_SPI0->CCR = 0x08;    
//	_SPI0->CR  = SPCR_MASTER | SPCR_LSBFIRST | SPCR_IE;
	_SPI0->CR  = SPCR_MASTER | SPCR_IE;

	// P0.15 is used for 74HC595 shift register chip selection as SPI slave
	io_configure(_IO0, IO_PIN_15, IO_PIN_OUTPUT);
	_IO0->SET=IO_PIN_15;
	
	_SPI0->INT = 0x01;        //reset IRQ flag in spi
	irq_ack();
	
	//initialize VIC for SPI interrupts
	irq_register_slot(IRQ_CHANNEL_SPI0, IRQ_SPI0_SLOT, (Handler_t)spi0_isr);
}

/* spi_write
 *   initialize SPI device
 */
void spi_write(uint8_t *data, uint32_t n)
{
	if (status!=SPI_BUSY) {
		status=SPI_BUSY;
		buffer=data;
		n_to_write=n-1;
		n_wr=1;
	
		_IO0->CLR=IO_PIN_15;
		
		//start transmission with first byte
    	_SPI0->DR = buffer[0];
    }
}


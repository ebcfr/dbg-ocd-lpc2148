#include "i2c.h"
#include "io.h"
#include "irq.h"


#define I2C_WRITE		0
#define I2C_READ		1

#define I2C_FREQ		400000       // 400 kHz

// Control register bits
#define I2C_BIT_AA		0x04
#define I2C_BIT_SI		0x08
#define I2C_BIT_STO 	0x10
#define I2C_BIT_STA		0x20
#define I2C_BIT_I2EN	0x40

#define I2C_START		0x60

/* I2C bus interface used */
#define i2c				(_I2C0)

/* context of the I2C transfert */
volatile static int		status = I2C_OK;	// driver status
static uint8_t	op=I2C_WRITE;		// read/write operation
static uint8_t	address;			// slave device address
static uint8_t *	buffer;				// buffer to write or to store read data
static uint32_t	n_to_read;			// how many bytes to be read?
static uint32_t	n_to_write;			// how many bytes to be written?
static uint32_t	n_wr;				// how many data actually written?
static uint32_t   n_rd;				// how many data actually read?
static uint8_t	state;				// internal I2C state


/* IRQ Handler for I2C Interface */
#ifndef __IRQ_HANDLER__
__attribute__((interrupt("IRQ"))) 
#endif
static void i2c_isr()
{
	state = i2c->STAT;
	switch (state) {
    case 0x08: // Start
    case 0x10: // Repeated start
    	i2c->DAT = (address<<1) | op;
    	i2c->CONCLR = I2C_BIT_STA;
    	break;

    case 0x18: // Write Mode : slave acknowledge
    case 0x28:
		if (n_to_write && (n_wr<n_to_write)) {	// Some data to write
			i2c->DAT = buffer[n_wr++];
			i2c->CONCLR = I2C_BIT_STA;			// STA=0
        } else {
			if (n_to_read) {					// Some data to read : repeated start
				op=I2C_READ;
				n_rd=0;
				i2c->CONSET=I2C_BIT_STA;
			} else {							// Finished : stop
				i2c->CONCLR=I2C_BIT_STA;
				i2c->CONSET=I2C_BIT_STO;
				status = I2C_OK;
			}
		}
		break;

	case 0x40: // Read mode : slave acknowledged address
		if (n_rd < n_to_read - 1) { // more than 1 byte to be received (nextstate -> 0x50)
			i2c->CONCLR = I2C_BIT_STA;
			i2c->CONSET = I2C_BIT_AA;
        } else {					// just 1 byte to be received (nextstate -> 0x58)
        	i2c->CONCLR = I2C_BIT_STA | I2C_BIT_AA;
        }
        break;
        
    case 0x50: // Read mode : slave sent data, master returned ACK
		buffer[n_rd++] = i2c->DAT;
		if (n_rd < n_to_read - 1) { // more than 1 byte still to be received (nextstate -> 0x50)
			i2c->CONCLR = I2C_BIT_STA;
			i2c->CONSET = I2C_BIT_AA;
        } else {					// just 1 byte still to be received (nextstate -> 0x58)
        	i2c->CONCLR = I2C_BIT_STA | I2C_BIT_AA;
        }
		break;
  
  	case 0x58: // Read mode  : slave sent data, master returned NACK
		buffer[n_rd++] = i2c->DAT;
		i2c->CONSET = I2C_BIT_STO;
		status=I2C_OK;
		break;

    case 0x20:	// Write mode : error, no slave answered
    case 0x48:	// Read mode : error, no slave answered
        i2c->CONCLR = I2C_BIT_STA;
    	i2c->CONSET = I2C_BIT_STO;
    	status = I2C_DEVICE_NOT_PRESENT;
		break;

    case 0x30:  // Write mode : slave returned NACK
        i2c->CONCLR = I2C_BIT_STA;
        i2c->CONSET = I2C_BIT_STO;
        status = I2C_SLAVE_ERROR;
		break;

    case 0x38:  // Write and Read mode : arbitration lost, release the bus
        i2c->CONSET = I2C_BIT_STA;
        status = I2C_ARBITRATION_LOST;
		break;

    default:
//        i2c->CONCLR = I2C_BIT_I2EN;
    	i2c->CONSET = I2C_BIT_STO;
        status = I2C_ERROR;
	}

	i2c->CONCLR = I2C_BIT_SI;
#ifndef __IRQ_HANDLER__
	_VIC->VectAddr=0;
#endif
}

/* i2c_master_init
 *
 * i2c interface initialized in master mode
 */
int i2c_master_init()
{
	uint32_t tmp = (Fpclk)/2/I2C_FREQ;
	
	// configuration des broches (Pin Connect Block)
//	if (i2c==_I2C0){
		io_configure(_IO0,0x0000000C,IO_PIN_FUNC1);
/*	} else if (i2c==_I2C1) {
		io_configure(IO_PORT0,(1<<14)|(1<<11),IO_PIN_FUNC3);
	} return I2C_INIT_ERROR;*/
	
	// initialisation de l'interface I2C
	i2c->CONCLR=0x6C;						// tous les drapeaux � O
	i2c->SCLL=tmp;							// setup clk
	i2c->SCLH=tmp;							// setup clk
	i2c->CONSET=I2C_BIT_I2EN;					// enable I2C interface

//	if (i2c==_I2C0){
		irq_register_slot(IRQ_CHANNEL_I2C0,IRQ_I2C0_SLOT,(Handler_t)i2c_isr);
/*	} else {
		irq_register_slot(IRQ_CHANNEL_I2C1,IRQ_I2C1_SLOT,(Handler_t)i2c_isr);
	}*/
	_VIC->VectAddr=0;

	op=I2C_WRITE;
	
	return I2C_OK;
}

/* i2c_write : write n bytes from buf to slave identified by addr
 *
 */
int i2c_write(uint8_t addr, uint8_t* buf, uint32_t n)
{
	status = I2C_BUSY;
	
	address    = addr;
	buffer     = buf;
	n_to_read  = 0;
	n_to_write = n;
	n_wr = n_rd = 0;
	op = I2C_WRITE;
	
	i2c->CONCLR=0x6C;
	i2c->CONSET=I2C_START;					// enable I2C interface
	
	while (status == I2C_BUSY) ;			// wait for the transaction to be done
	
	return status;
}

/* i2c_read : read n bytes from slave identified by addr to buf
 *
 */
int i2c_read(uint8_t addr, uint8_t* buf, uint32_t n)
{
	status = I2C_BUSY;
	
	address    = addr;
	buffer     = buf;
	n_to_read  = n;
	n_to_write = 0;
	n_wr = n_rd = 0;
	op = I2C_READ;

	i2c->CONCLR=0x6C;
	i2c->CONSET=I2C_START;					// enable I2C interface
	
	while (status == I2C_BUSY) ;			// wait for the transaction to be done
	
	return status;
}

/* i2c_write_read : write nwr bytes from buf to slave identified by addr,
 *                  then read nrd bytes to buf
 *
 */
int i2c_write_read(uint8_t addr, uint8_t* buf, uint32_t nwr, uint32_t nrd)
{
	status = I2C_BUSY;
	
	address    = addr;
	buffer     = buf;
	n_to_read  = nrd;
	n_to_write = nwr;
	n_wr = n_rd = 0;
	op = I2C_WRITE;
	
	i2c->CONCLR=0x6C;
	i2c->CONSET=I2C_START;					// enable I2C interface
	
	while (status == I2C_BUSY) ;			// wait for the transaction to be done
	
	return status;
}

/******************************************************************************
 * io_asm.s
 ******************************************************************************/
#include "lpc214x.h"

        .text
        .arm
        .global io_read, io_write, io_write_n


/******************************************************************************
 * uint32 io_read(IO *port, uint32 mask)
 * input parameters
 *   r0 : io port struct address
 *   r1 : mask
 * return parameter
 *   r0 : read and masked value on port 'port'
 ******************************************************************************/
io_read:
		ldr     r2, [r0, #IO_PIN]
		and     r0, r2, r1
        mov     pc, lr
        
/******************************************************************************
 * void io_write(IO *port, uint32 val, uint32 mask)
 * '1' in val are written to port as HIGH level
 * input parameters
 *   r0 : io port struct address
 *   r1 : value to be written after masking
 *   r2 : mask
 ******************************************************************************/
io_write:
        str     r2, [r0, #IO_CLR]
        and     r1, r1, r2
        str     r1, [r0, #IO_SET]
        mov     pc, lr
        
/******************************************************************************
 * void io_write_n(IO *port, uint32 val, uint32 mask)
 * '1' in val are written to port as LOW level
 * input parameters
 *   r0 : io port struct address
 *   r1 : value to be written after masking
 *   r2 : mask
 ******************************************************************************/
io_write_n:
        str     r2, [r0, #IO_SET]
        and     r1, r1, r2
        str     r1, [r0, #IO_CLR]
        mov     pc, lr
        
        .end
        
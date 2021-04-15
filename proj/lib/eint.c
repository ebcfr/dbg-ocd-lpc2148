#include "eint.h"
#include "io.h"
#include "irq.h"

// EXTINT 
#define EXT_EINT0_BIT			0
#define EXT_EINT1_BIT			1
#define EXT_EINT2_BIT			2
#define EXT_EINT3_BIT			3

static OnEint callback = 0;

/* eint1_isr
 *   eint1 ISR (Interrupt Service Routine)
 */
#ifdef __IRQ_HANDLER__
static void eint1_isr(void)
{
    _EXT->INT = 1<<EXT_EINT1_BIT;
    if (callback) callback();
}
#else
static __attribute__((interrupt("IRQ"))) void eint1_isr(void)
{
    _EXT->INT = 1<<EXT_EINT1_BIT;
    if (callback) callback();
    irq_ack();
}
#endif

void eint1_init(uint32_t mode, uint32_t polar, OnEint cb)
{
	io_configure(_IO0, IO_PIN_14, IO_PIN_FUNC2);

    _EXT->MODE = (_EXT->MODE & (~(1<<EXT_EINT1_BIT))) | (mode<<EXT_EINT1_BIT);
    _EXT->POLAR = (_EXT->POLAR & (~(1<<EXT_EINT1_BIT))) | (polar<<EXT_EINT1_BIT);

    // acknowledge pending irq
    _EXT->INT = 1<<EXT_EINT1_BIT;
    irq_ack();
    
    if(cb) callback = cb;
    irq_register_slot(IRQ_CHANNEL_EINT1,IRQ_EINT1_SLOT, (Handler_t)eint1_isr);
}


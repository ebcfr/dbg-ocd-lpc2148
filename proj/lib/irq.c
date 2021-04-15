#include "irq.h"
#include "board.h"

/* disable all irq channels */
void irq_init()
{
	_VIC->IntEnClr=0xFFFFFFFF;
}

/* irq_register_slot : register an isr, enable channel interrupt
 *
 *   channel : irq vic channel
 *   slot    : slot to be allocated to channel
 *   isr     : callback isr
 */
int irq_register_slot(uint32_t channel, uint32_t slot, Handler_t isr)
{
	if ((channel < 32) && (slot < 16)) {
		_VIC->VectCntlSlot[slot]=0x20 | channel;
		if (isr) _VIC->VectAddrSlot[slot]=(uint32_t)isr;
		_VIC->IntSelect=_VIC->IntSelect & ~(1<<channel);
		_VIC->IntEnable=_VIC->IntEnable | (1<<channel);
		return 1;
	}
	return 0;
}

/* irq_unregister_slot : unregister slot, disable irq handling
 *
 *   channel : irq vic channel
 *   slot    : slot to be deallocated
 */
int irq_unregister_slot(uint32_t channel, uint32_t slot)
{
	if ((channel < 32) && (slot < 16)) {
		_VIC->IntEnClr=1<<channel;
		_VIC->VectCntlSlot[slot]=0;
		return 1;
	}
	return 0;
}

/* irq_acknowledge : disable irq
 *
 *   channel : irq vic channel
 */
void irq_ack()
{
	_VIC->VectAddr=0;
}

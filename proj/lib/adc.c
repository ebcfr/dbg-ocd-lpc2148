#include "adc.h"
#include "io.h" 
#include "irq.h"

// ADxCR (Control Register)
#define ADxCR_SEL_MASK	0xFF
#define ADxCR_BURST		(1<<16)
#define ADxCR_PDN		(1<<21)
#define ADxCR_START		(1<<24)

static OnSample callback0=0;
static OnSample callback1=0;

#ifdef __IRQ_HANDLER__
static void adc0_isr()
{
	unsigned int tmp=_ADC0->GDR;
	_ADC0->CR &= ~ADxCR_START;
	unsigned int ch=(tmp>>24) & 7;
	_ADC0->DR[ch];
	if (callback0) callback0(ch,(tmp>>6) & 0x3FF);
}

static void adc1_isr()
{
	unsigned int tmp=_ADC1->GDR;
	_ADC1->CR &= ~ADxCR_START;
	unsigned int ch=(tmp>>24) & 7;
	_ADC1->DR[ch];
	if (callback1) callback1((tmp>>24) &7,(tmp>>6) & 0x3FF);
}

#else
static __attribute__((interrupt("IRQ"))) void adc0_isr()
{
	unsigned int tmp=_ADC0->GDR;
	_ADC0->CR &= ~ADxCR_START;
	unsigned int ch=(tmp>>24) & 7;
	_ADC0->DR[ch];
	if (callback0) callback0(ch,(tmp>>6) & 0x3FF);
    _VIC->VectAddr=0;
}

static __attribute__((interrupt("IRQ"))) void adc1_isr()
{
	unsigned int tmp=_ADC1->GDR;
	_ADC1->CR &= ~ADxCR_START;
	unsigned int ch=(tmp>>24) & 7;
	_ADC1->DR[ch];
	if (callback1) callback1((tmp>>24) &7,(tmp>>6) & 0x3FF);
    _VIC->VectAddr=0;
}
#endif

/* adc_init
 *   intialize ADC
 */
int adc_init(ADC *adc, OnSample cb)
{
	int i;
	
	adc->CR= ((Fpclk/4500000+1)<<8) | (1<<21);	// Ctrl Register CLKDIV=14 for Fpclk=60MHz
	adc->INTEN=0;
	for (i=0;i<8;i++) adc->DR[i];
	adc->GDR;
	_VIC->VectAddr = 0;
	
	if (adc == _ADC0) {
		if (cb) callback0=cb;
		irq_register_slot(IRQ_CHANNEL_ADC0,IRQ_ADC0_SLOT,(Handler_t)adc0_isr);
		return 1;
	} else if (adc == _ADC1) {
		if (cb) callback1=cb;
		irq_register_slot(IRQ_CHANNEL_ADC1,IRQ_ADC1_SLOT,(Handler_t)adc1_isr);
		return 1;
	}
    return 0;
}

/* adc_channel_enable
 *   set up adc channel
 */
int adc_channel_enable(ADC *adc, uint32_t channel)
{
	if (channel>7) return 0;
	
	if (adc == _ADC0) {
		if (channel==1) {									// AD0.1 -> P0.28
			io_configure(_IO0, IO_PIN_28, IO_PIN_FUNC1);
		} else if (channel==2) {							// AD0.2 -> P0.29
			io_configure(_IO0, IO_PIN_29, IO_PIN_FUNC1);
		} else return 0;
	}
	else return 0;

	adc->INTEN = adc->INTEN | (1<<channel);
	
	return 0;
}

/* adc_channel_sample
 *   sample the specified channel and generate an interrupt when it is done
 */
int adc_channel_sample(ADC *adc, uint32_t channel)
{
	if (channel>7) return 0;
	
	adc->CR = (adc->CR & (~ADxCR_SEL_MASK)) | (1<<channel) | ADxCR_START;
	return 1;
}

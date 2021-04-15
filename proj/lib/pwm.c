#include "pwm.h"
#include "io.h" 
#include "irq.h"

// PWMIR (Interrupt Register)
#define PWM_IR_MR0_FLAG         (1<<0)
#define PWM_IR_MR1_FLAG         (1<<1)
#define PWM_IR_MR2_FLAG         (1<<2)
#define PWM_IR_MR3_FLAG         (1<<3)
#define PWM_IR_MR4_FLAG         (1<<8)
#define PWM_IR_MR5_FLAG         (1<<9)
#define PWM_IR_MR6_FLAG         (1<<10)
#define PWM_IR_ALL_FLAG         (PWM_IR_MR0_FLAG|PWM_IR_MR1_FLAG|PWM_IR_MR2_FLAG|PWM_IR_MR3_FLAG|PWM_IR_MR4_FLAG|PWM_IR_MR5_FLAG|PWM_IR_MR6_FLAG)

// PWMTCR (Timer Control Register)
#define PWM_TCR_COUNTER_START		(1<<0)
#define PWM_TCR_COUNTER_STOP		(0)
#define PWM_TCR_COUNTER_RESET		(1<<1)
#define PWM_TCR_PWM_EN				(1<<3)

// PWMMCR (Match Control Register)
#define PWM_MCR_INT_ON_MR0      (1<<0)    	// MR0
#define PWM_MCR_RESET_ON_MR0    (1<<1)
#define PWM_MCR_STOP_ON_MR0     (1<<2)
#define PWM_MCR_INT_ON_MR1      (1<<3)    	// MR1
#define PWM_MCR_RESET_ON_MR1    (1<<4)
#define PWM_MCR_STOP_ON_MR1     (1<<5)
#define PWM_MCR_INT_ON_MR2      (1<<6)    	// MR2
#define PWM_MCR_RESET_ON_MR2    (1<<7)
#define PWM_MCR_STOP_ON_MR2     (1<<8)
#define PWM_MCR_INT_ON_MR3      (1<<9)    	// MR3
#define PWM_MCR_RESET_ON_MR3    (1<<10)
#define PWM_MCR_STOP_ON_MR3     (1<<11)
#define PWM_MCR_INT_ON_MR4      (1<<12)   	// MR4
#define PWM_MCR_RESET_ON_MR4    (1<<13)
#define PWM_MCR_STOP_ON_MR4     (1<<14)
#define PWM_MCR_INT_ON_MR5      (1<<15)    	// MR5
#define PWM_MCR_RESET_ON_MR5    (1<<16)
#define PWM_MCR_STOP_ON_MR5     (1<<17)
#define PWM_MCR_INT_ON_MR6      (1<<18)    	// MR6
#define PWM_MCR_RESET_ON_MR6    (1<<19)
#define PWM_MCR_STOP_ON_MR6     (1<<20)

// PWMPCR (PWM Control Register)
#define PWM_PCR_DBL_EDGE_MR2	(1<<2)
#define PWM_PCR_DBL_EDGE_MR3	(1<<3)
#define PWM_PCR_DBL_EDGE_MR4	(1<<4)
#define PWM_PCR_DBL_EDGE_MR5	(1<<5)
#define PWM_PCR_DBL_EDGE_MR6	(1<<6)

#define PWM_PCR_OE_MR1			(1<<9)
#define PWM_PCR_OE_MR2			(1<<10)
#define PWM_PCR_OE_MR3			(1<<11)
#define PWM_PCR_OE_MR4			(1<<12)
#define PWM_PCR_OE_MR5			(1<<13)
#define PWM_PCR_OE_MR6			(1<<14)

static OnTick callback=0;

#ifdef __IRQ_HANDLER__
static void pwm_isr()
{
    _PWM->IR = PWM_IR_MR0_FLAG;   // Clear interrupt flag by writing 1 to Bit 0
	if (callback) callback();
}
#else
static __attribute__((interrupt("IRQ"))) void pwm_isr()
{
    _PWM->IR = PWM_IR_MR0_FLAG;   // Clear interrupt flag by writing 1 to Bit 0
	if (callback) callback();
    _VIC->VectAddr=0;
}
#endif

/* pwm_init
 *   setup pwm timer period, each tick_ms
 */
int pwm_init(PWM *pwm, uint32_t period_ms, OnTick cb)
{
    pwm->TCR = PWM_TCR_COUNTER_RESET;			// timer stop and reset
    pwm->MR0 = 100*period_ms-1;								// Compare-hit at mSec
    pwm->MCR = PWM_MCR_INT_ON_MR0 | PWM_MCR_RESET_ON_MR0; // Interrupt and Reset on MR0  
    pwm->PR = Fpclk/1000/100;
    pwm->IR = PWM_IR_ALL_FLAG;							// Reset IRQ flags
    _VIC->VectAddr=0;
  
  	pwm->PCR = 0;
  	
    if (cb) callback=cb;
    irq_register_slot(IRQ_CHANNEL_PWM,IRQ_PWM_SLOT,(Handler_t)pwm_isr);
    
    return 1;
}

/* pwm_channel_enable
 *   set up pwm channel
 */
int pwm_channel_enable(PWM *pwm, uint32_t channel, uint32_t dutycycle, uint32_t oe, uint32_t dbledge)
{
	if ((channel<1) || (channel>6) || dutycycle>100) return 0;
	
	unsigned int T=pwm->MR0;
	
	switch (channel) {
		case 1:
			io_configure(_IO0, IO_PIN_0, IO_PIN_FUNC2); // PWM1 -> P0.0
			pwm->MR1 = dutycycle*T/100;
			if (oe) pwm->PCR = pwm->PCR | PWM_PCR_OE_MR1;
			break;
		case 2:
			io_configure(_IO0, IO_PIN_7, IO_PIN_FUNC2);	// PWM2 -> P0.7
			pwm->MR2 = dutycycle*T/100;
			if (oe) pwm->PCR = pwm->PCR | PWM_PCR_OE_MR2;
			if (dbledge) pwm->PCR = pwm->PCR | (1<<channel);
			break;
		case 3:
			io_configure(_IO0, IO_PIN_1, IO_PIN_FUNC2);	// PWM3 -> P0.1
			pwm->MR3 = dutycycle*T/100;
			if (oe) pwm->PCR = pwm->PCR | PWM_PCR_OE_MR3;
			if (dbledge) pwm->PCR = pwm->PCR | (1<<channel);
			break;
		case 4:
			io_configure(_IO0, IO_PIN_8, IO_PIN_FUNC2);	// PWM4 -> P0.8
			pwm->MR4 = dutycycle*T/100;
			if (oe) pwm->PCR = pwm->PCR | PWM_PCR_OE_MR4;
			if (dbledge) pwm->PCR = pwm->PCR | (1<<channel);
			break;
		case 5:
			io_configure(_IO0, IO_PIN_21, IO_PIN_FUNC1);	// PWM5 -> P0.21
			pwm->MR5 = dutycycle*T/100;
			if (oe) pwm->PCR = pwm->PCR | PWM_PCR_OE_MR5;
			if (dbledge) pwm->PCR = pwm->PCR | (1<<channel);
			break;
		case 6:
			io_configure(_IO0, IO_PIN_9, IO_PIN_FUNC2);	// PWM6 -> P0.9
			pwm->MR6 = dutycycle*T/100;
			if (oe) pwm->PCR = pwm->PCR | PWM_PCR_OE_MR6;
			if (dbledge) pwm->PCR = pwm->PCR | (1<<channel);
			break;
	}
	pwm->LER |= 1<<channel;
	
	return 1;
}

/* pwm_channel_set
 *   set up dutycycle for pwm channel
 */
int pwm_channel_set(PWM *pwm, uint32_t channel, uint32_t dutycycle)
{
	if ((channel<1) || (channel>6) || dutycycle>100) return 0;
	
	unsigned int T=pwm->MR0;
	
	switch (channel) {
		case 1:
			pwm->MR1 = dutycycle*T/100;
			break;
		case 2:
			pwm->MR2 = dutycycle*T/100;
			break;
		case 3:
			pwm->MR3 = dutycycle*T/100;
			break;
		case 4:
			pwm->MR4 = dutycycle*T/100;
			break;
		case 5:
			pwm->MR5 = dutycycle*T/100;
			break;
		case 6:
			pwm->MR6 = dutycycle*T/100;
			break;
	}
	pwm->LER |= 1<<channel;
	
	return 1;
}

/* pwm_channel_disable
 *   set up pwm channel
 */
int pwm_channel_disable(PWM *pwm, uint32_t channel)
{
	if ((channel<1) || (channel>6)) return 0;
	
	switch (channel) {
		case 1:
			io_configure(_IO0, IO_PIN_0, IO_PIN_INPUT); // PWM1 -> P0.0
			pwm->MR1 = 0;
			pwm->PCR = pwm->PCR & (~PWM_PCR_OE_MR1);
			break;
		case 2:
			io_configure(_IO0, IO_PIN_7, IO_PIN_INPUT);	// PWM2 -> P0.7
			pwm->MR2 = 0;
			pwm->PCR = pwm->PCR & (~PWM_PCR_OE_MR2);
			break;
		case 3:
			io_configure(_IO0, IO_PIN_1, IO_PIN_INPUT);	// PWM3 -> P0.1
			pwm->MR3 = 0;
			pwm->PCR = pwm->PCR & (~PWM_PCR_OE_MR3);
			break;
		case 4:
			io_configure(_IO0, IO_PIN_8, IO_PIN_INPUT);	// PWM4 -> P0.8
			pwm->MR4 = 0;
			pwm->PCR = pwm->PCR & (~PWM_PCR_OE_MR4);
			break;
		case 5:
			io_configure(_IO0, IO_PIN_21, IO_PIN_INPUT);// PWM5 -> P0.21
			pwm->MR5 = 0;
			pwm->PCR = pwm->PCR & (~PWM_PCR_OE_MR5);
			break;
		case 6:
			io_configure(_IO0, IO_PIN_9, IO_PIN_INPUT);	// PWM6 -> P0.9
			pwm->MR6 = 0;
			pwm->PCR = pwm->PCR & (~PWM_PCR_OE_MR6);
			break;
	}
	pwm->LER |= 1<<channel;
	
	return 1;
}

/* pwm_start
 *   start counting
 */
void pwm_start(PWM *pwm)
{
	pwm->TCR=PWM_TCR_COUNTER_START | PWM_TCR_PWM_EN;
}

/* pwm_stop
 *   stop and reset counting
 */
void pwm_stop(PWM *pwm)
{
	pwm->TCR=PWM_TCR_COUNTER_STOP;
}

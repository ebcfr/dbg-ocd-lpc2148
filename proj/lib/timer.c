#include "timer.h"
#include "irq.h"

// TxCTCR (Timer Counter Control Register)
#define	TxCTCR_CNT_RISEDGE_PCLK	(0<<0)
#define	TxCTCR_CNT_RISEDGE_CAPX	(1<<0)
#define	TxCTCR_CNT_FALEDGE_PCLK	(2<<0)
#define	TxCTCR_CNT_FALEDGE_CAPX	(3<<0)
#define	TxCTCR_CNT_INPUT_CAPX_0	(0<<2)
#define	TxCTCR_CNT_INPUT_CAPX_1	(1<<2)
#define	TxCTCR_CNT_INPUT_CAPX_2	(2<<2)
#define	TxCTCR_CNT_INPUT_CAPX_3	(3<<2)

// TxTCR (Timer Control Register)
#define TxTCR_COUNTER_ENABLE  (1<<0)
#define TxTCR_COUNTER_RESET   (1<<1)

// TxMCR (Match Control Register)
#define TxMCR_INT_ON_MR0      (1<<0)    // MR0
#define TxMCR_RESET_ON_MR0    (1<<1)
#define TxMCR_STOP_ON_MR0     (1<<2)
#define TxMCR_INT_ON_MR1      (1<<3)    // MR1
#define TxMCR_RESET_ON_MR1    (1<<4)
#define TxMCR_STOP_ON_MR1     (1<<5)
#define TxMCR_INT_ON_MR2      (1<<6)    // MR2
#define TxMCR_RESET_ON_MR2    (1<<7)
#define TxMCR_STOP_ON_MR2     (1<<8)
#define TxMCR_INT_ON_MR3      (1<<9)    // MR3
#define TxMCR_RESET_ON_MR3    (1<<10)
#define TxMCR_STOP_ON_MR3     (1<<11)

// TxIR (Interrupt Register)
#define TxIR_MR0_FLAG         (1<<0)
#define TxIR_MR1_FLAG         (1<<1)
#define TxIR_MR2_FLAG         (1<<2)
#define TxIR_MR3_FLAG         (1<<3)
#define TxIR_CR0_FLAG         (1<<4)
#define TxIR_CR1_FLAG         (1<<5)
#define TxIR_CR2_FLAG         (1<<6)
#define TxIR_CR3_FLAG         (1<<7)


/* timer_wait_ms
 *   wait for ms millisecoonds function
 */
void timer_wait_ms(TIMER *tmr, uint32_t ms)
{
    uint32_t tmp;
    tmr->CTCR = TxCTCR_CNT_RISEDGE_PCLK;	// count on rising edgle of pclk
    tmr->TCR = TxTCR_COUNTER_RESET;			// timer stop and reset
    tmp = tmr->MR[0] = ms;					// Compare-hit with ms
    tmr->MCR = TxMCR_STOP_ON_MR0;			// Stop on MR0  
    tmr->PR = Fpclk/1000;
    tmr->IR = 0xFF;							// Reset IRQ flags
    tmr->TCR=TxTCR_COUNTER_ENABLE;			// count
    
    while (tmr->TC!=tmp);					// wait until count is done
}

/* timer_wait_us
 *   wait for us microsecoonds function
 */
void timer_wait_us(TIMER *tmr, uint32_t us)
{
    uint32_t tmp;
    tmr->CTCR = TxCTCR_CNT_RISEDGE_PCLK;	// count on rising edgle of pclk
    tmr->TCR = TxTCR_COUNTER_RESET;			// timer stop and reset
    tmp = tmr->MR[0] = us;					// Compare-hit with us
    tmr->MCR = TxMCR_STOP_ON_MR0;			// Stop on MR0  
    tmr->PR = Fpclk/1000000;
    tmr->IR = 0xFF;							// Reset IRQ flags
    tmr->TCR=TxTCR_COUNTER_ENABLE;			// count
    
    while (tmr->TC!=tmp);					// wait until count is done
}

/* timerX_isr
 *   timerX ISR (Interrupt Service Routine)
 */
static OnTick callback0=0;
static OnTick callback1=0;

#ifdef __IRQ_HANDLER__
void timer0_isr()
{
    if (callback0) callback0();
    _TIMER0->IR = TxIR_MR0_FLAG;   // Clear interrupt flag by writing 1 to Bit 0
}

void timer1_isr()
{
    if (callback1) callback1();
    _TIMER1->IR = TxIR_MR0_FLAG;   // Clear interrupt flag by writing 1 to Bit 0
}
#else
__attribute__((interrupt("IRQ"))) void timer0_isr()
{
    if (callback0) callback0();
    _TIMER0->IR = TxIR_MR0_FLAG;   // Clear interrupt flag by writing 1 to Bit 0
    _VIC->VectAddr=0;
}

__attribute__((interrupt("IRQ"))) void timer1_isr()
{
    if (callback1) callback1();
    _TIMER1->IR = TxIR_MR0_FLAG;   // Clear interrupt flag by writing 1 to Bit 0
    _VIC->VectAddr=0;
}
#endif


/* timer0_timeout_init
 *   timer0 configured to generate periodic interrupts
 */
int timer_tick_init(TIMER *tmr, uint32_t ms, OnTick cb)
{
    tmr->CTCR = TxCTCR_CNT_RISEDGE_PCLK;	// count on rising edgle of pclk
    tmr->TCR = TxTCR_COUNTER_RESET;			// timer stop and reset
    tmr->MR[0] = ms;								// Compare-hit at mSec
    tmr->MCR = TxMCR_INT_ON_MR0 | TxMCR_RESET_ON_MR0; // Interrupt and Reset on MR0  
    tmr->PR = Fpclk/1000;
    tmr->IR = 0xFF;							// Reset IRQ flags
    _VIC->VectAddr=0;
  
    if (tmr==_TIMER0) {
        if (cb) callback0=cb;
        irq_register_slot(IRQ_CHANNEL_TIMER0,IRQ_TIMER0_SLOT,(Handler_t)timer0_isr);
        return 1;
    } else if (tmr==_TIMER1) {
        if (cb) callback1=cb;
        irq_register_slot(IRQ_CHANNEL_TIMER1,IRQ_TIMER1_SLOT,(Handler_t)timer1_isr);
        return 1;
    }
    return 0;
}


/* timer_oneshot_init
 *   timer configured to generate one shot wait time
 */
int timer_oneshot_init(TIMER *tmr, uint32_t ms, OnTick cb)
{
    tmr->CTCR = TxCTCR_CNT_RISEDGE_PCLK;	// count on rising edgle of pclk
    tmr->TCR = TxTCR_COUNTER_RESET;			// timer stop and reset
    tmr->MR[0] = ms;								// Compare-hit at mSec
    tmr->MCR = TxMCR_INT_ON_MR0 | TxMCR_RESET_ON_MR0 | TxMCR_STOP_ON_MR0; // Interrupt and Reset and stop on MR0  
    tmr->PR = Fpclk/1000;
    tmr->IR = 0xFF;							// Reset IRQ flags
    _VIC->VectAddr=0;
  
    if (tmr==_TIMER0) {
        if (cb) callback0=cb;
        irq_register_slot(IRQ_CHANNEL_TIMER0,IRQ_TIMER0_SLOT,(Handler_t)timer0_isr);
        return 1;
    } else if (tmr==_TIMER1) {
        if (cb) callback1=cb;
        irq_register_slot(IRQ_CHANNEL_TIMER1,IRQ_TIMER1_SLOT,(Handler_t)timer1_isr);
        return 1;
    }
    return 0;
}


/* timer_tick_init
 *   setup timer to call cb function periodically, each tick_ms
 */
void timer_start(TIMER *tmr)
{
    tmr->TCR=TxTCR_COUNTER_ENABLE;
}

/* timer_stop
 *   stop and reset counting
 */
void timer_stop(TIMER *tmr)
{
    tmr->TCR=TxTCR_COUNTER_RESET;
}


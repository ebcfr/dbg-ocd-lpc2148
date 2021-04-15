#include "board.h"

/*****************************************************************************
 *  timer 0
 *****************************************************************************/

// TxTCR (Timer Control Register)
#define TxTCR_COUNTER_ENABLE  (1<<0)
#define TxTCR_COUNTER_RESET   (1<<1)

// TxMCR (Match Control Register)
#define TxMCR_INT_ON_MR0      (1<<0)              // MR0
#define TxMCR_RESET_ON_MR0    (1<<1)
#define TxMCR_STOP_ON_MR0     (1<<2)
#define TxMCR_INT_ON_MR1      (1<<3)              // MR1
#define TxMCR_RESET_ON_MR1    (1<<4)
#define TxMCR_STOP_ON_MR1     (1<<5)
#define TxMCR_INT_ON_MR2      (1<<6)              // MR2
#define TxMCR_RESET_ON_MR2    (1<<7)
#define TxMCR_STOP_ON_MR2     (1<<8)
#define TxMCR_INT_ON_MR3      (1<<9)              // MR3
#define TxMCR_RESET_ON_MR3    (1<<10)
#define TxMCR_STOP_ON_MR3     (1<<11)


void wait(uint32_t ms)
{
    uint32_t tmp;
    _TIMER0->CTCR = 0;
    _TIMER0->TCR = TxTCR_COUNTER_RESET;                  // timer stop and reset
    tmp = _TIMER0->MR[0] = ms;                             // Compare-hit at mSec
    _TIMER0->MCR = TxMCR_STOP_ON_MR0;                    // Stop on MR0  
    _TIMER0->PR = Fpclk/1000;
    _TIMER0->IR = 0xFF;                                  // Reset flags
    _TIMER0->TCR=TxTCR_COUNTER_ENABLE;                   // count
    
    while (_TIMER0->TC!=tmp);
}

/*****************************************************************************
 *  Base de Temps
 *****************************************************************************/
static uint32_t delay=200;
static uint32_t count_time=0;
static uint32_t count=0;

void BdT_start()
{
    count_time=0;
    count=1;
}

void BdT_stop()
{
    count=0;
}

void BdT_decrease_tick(uint32_t yes)
{
    if (count && yes) {
        delay-=50;
        if (delay<50) delay=50;
    } else {
        delay+=50;
        if (delay>2000) delay=2000;
    }
}

uint32_t BdT_tick_done()
{
    if (count_time>=delay) {
        count_time=0;
        return 1;
    }
    return 0;
}

void BdT_increment()
{
    if (count) {
        count_time++;
    }
    wait(1);
}

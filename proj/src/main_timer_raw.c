#include "board.h"
#include "io.h"
#include "lcd.h"

#define MAIN3

int cpt=0;
volatile int pushed=0;

//****************************************************************************
// functions prototypes
//****************************************************************************
extern void irq_enable();
extern void irq_disable();


//****************************************************************************
//  Test EINT1 button
//****************************************************************************
#ifdef MAIN1
#define EXT_EINT0_BIT			0
#define EXT_EINT1_BIT			1
#define EXT_EINT2_BIT			2
#define EXT_EINT3_BIT			3

#define EXT_LEVEL_SENSITIVE		0
#define EXT_EDGE_SENSITIVE		1
#define EXT_LOW					0
#define EXT_HIGH				1
#define EXT_FALLING				0
#define EXT_RISING				1

// eint1_isr
//   eint1 ISR (Interrupt Service Routine)
#ifdef __IRQ_HANDLER__
static void eint1_isr()
{
	pushed = 1;
	
    _EXTINT = 1<<EXT_EINT1_BIT;
}
#else
static __attribute__((interrupt("IRQ"))) void eint1_isr()
{
	pushed = 1;
	
    _EXTINT = 1<<EXT_EINT1_BIT;
    _VICVectAddr=0;
}
#endif
int main()
{
	lcd_init();
	lcd_display(LCD_DISPLAY_DISPLAY_ON);

	//==============================================================
    // config external irq on P0.14
	//==============================================================
	io_configure(_IO0, IO_PIN_14, IO_PIN_FUNC2);
    //_PINSEL0 = (_PINSEL0 & 0xCFFFFFFF) | 0x20000000;  // EINT1 -> P0.14
    
    // edge sensitive
    _EXTMODE = (_EXTMODE & (~(1<<EXT_EINT1_BIT))) | (EXT_EDGE_SENSITIVE<<EXT_EINT1_BIT);
    // rising edge
    _EXTPOLAR = (_EXTMODE & (~(1<<EXT_EINT1_BIT))) | (EXT_RISING<<EXT_EINT1_BIT);
    // acknowledge pending irq
    _EXTINT = 1<<EXT_EINT1_BIT;
    _VICVectAddr=0;
    
    // interrupt config
    _VICVectCntl4 = 0x2F;
    _VICVectAddr4 = (uint32_t)eint1_isr;
    _VICIntSelect &= 0xFFFF7FFF;
    _VICIntEnable |= 0x8000;

	//==============================================================
    
    irq_enable();
    
    while (1) {
    	if (pushed) {
    		lcd_clear();
    		lcd_home();
    		lcd_printf("%d",++cpt);
    		pushed=0;
    	}
    }

    return 0;
}
#endif
//****************************************************************************
//  Test Timer0
//****************************************************************************
#ifdef MAIN2
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
#define TxTCR_COUNTER_START		(1<<0)
#define TxTCR_COUNTER_STOP		(0)
#define TxTCR_COUNTER_RESET		(1<<1)

// TxMCR (Match Control Register)
#define TxMCR_INT_ON_MR0		(1<<0)    // MR0
#define TxMCR_RESET_ON_MR0		(1<<1)
#define TxMCR_STOP_ON_MR0		(1<<2)
#define TxMCR_INT_ON_MR1		(1<<3)    // MR1
#define TxMCR_RESET_ON_MR1		(1<<4)
#define TxMCR_STOP_ON_MR1		(1<<5)
#define TxMCR_INT_ON_MR2		(1<<6)    // MR2
#define TxMCR_RESET_ON_MR2		(1<<7)
#define TxMCR_STOP_ON_MR2		(1<<8)
#define TxMCR_INT_ON_MR3		(1<<9)    // MR3
#define TxMCR_RESET_ON_MR3		(1<<10)
#define TxMCR_STOP_ON_MR3		(1<<11)

// TxIR (Interrupt Register)
#define TxIR_MR0_FLAG			(1<<0)
#define TxIR_MR1_FLAG			(1<<1)
#define TxIR_MR2_FLAG			(1<<2)
#define TxIR_MR3_FLAG 			(1<<3)
#define TxIR_CR0_FLAG 			(1<<4)
#define TxIR_CR1_FLAG			(1<<5)
#define TxIR_CR2_FLAG 			(1<<6)
#define TxIR_CR3_FLAG			(1<<7)

volatile unsigned int time_ms=0;

#ifdef __IRQ_HANDLER__
static void timer0_isr()
{
	time_ms++;
    _T0IR = TxIR_MR0_FLAG;   // Clear interrupt flag by writing 1 to Bit 0
}
#else
static __attribute__((interrupt("IRQ"))) void timer0_isr()
{
	time_ms++;
    _T0IR = TxIR_MR0_FLAG;   // Clear interrupt flag by writing 1 to Bit 0
    _VICVectAddr=0;
}
#endif

int main()
{
	lcd_init();
	lcd_display(LCD_DISPLAY_DISPLAY_ON);
	//==============================================================
    // config Timer0
	//==============================================================
    _T0CTCR = TxCTCR_CNT_RISEDGE_PCLK;				// count on rising edge of pclk
    _T0TCR = TxTCR_COUNTER_RESET;					// timer stop and reset
    _T0MR0 = 99;										// Compare-hit at mSec
    _T0MCR = TxMCR_INT_ON_MR0 | TxMCR_RESET_ON_MR0;	// Interrupt and Reset on MR0
    _T0PR  = Fpclk/1000/100-1;
    
    _T0IR = 0xFF;									// Reset IRQ flags
    _VICVectAddr=0;
    
    // interrupt config
    _VICVectCntl2 = 0x24;
    _VICVectAddr2 = (uint32_t)timer0_isr;
    _VICIntSelect &= 0xFFFFFFEF;
    _VICIntEnable |= 0x00000010;
	//==============================================================
    
    irq_enable();
    
    _T0TCR = TxTCR_COUNTER_START;

    while (1) {
    	if ((time_ms%10)==0) {
    		lcd_goto_xy(1,1);
    		lcd_printf("%8d",time_ms);
    	}
    }

    return 0;
}
#endif
//****************************************************************************
//  Chronometer
//****************************************************************************
#ifdef MAIN3
volatile unsigned int time_ms=0;
unsigned int time_inter=0;
unsigned int state=0;

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
#define TxTCR_COUNTER_START		(1<<0)
#define TxTCR_COUNTER_STOP		(0)
#define TxTCR_COUNTER_RESET		(1<<1)

// TxMCR (Match Control Register)
#define TxMCR_INT_ON_MR0		(1<<0)    // MR0
#define TxMCR_RESET_ON_MR0		(1<<1)
#define TxMCR_STOP_ON_MR0		(1<<2)
#define TxMCR_INT_ON_MR1		(1<<3)    // MR1
#define TxMCR_RESET_ON_MR1		(1<<4)
#define TxMCR_STOP_ON_MR1		(1<<5)
#define TxMCR_INT_ON_MR2		(1<<6)    // MR2
#define TxMCR_RESET_ON_MR2		(1<<7)
#define TxMCR_STOP_ON_MR2		(1<<8)
#define TxMCR_INT_ON_MR3		(1<<9)    // MR3
#define TxMCR_RESET_ON_MR3		(1<<10)
#define TxMCR_STOP_ON_MR3		(1<<11)

// TxIR (Interrupt Register)
#define TxIR_MR0_FLAG			(1<<0)
#define TxIR_MR1_FLAG			(1<<1)
#define TxIR_MR2_FLAG			(1<<2)
#define TxIR_MR3_FLAG 			(1<<3)
#define TxIR_CR0_FLAG 			(1<<4)
#define TxIR_CR1_FLAG			(1<<5)
#define TxIR_CR2_FLAG 			(1<<6)
#define TxIR_CR3_FLAG			(1<<7)

#ifdef __IRQ_HANDLER__
static void timer0_isr()
{
	time_ms++;
    _T0IR = TxIR_MR0_FLAG;   // Clear interrupt flag by writing 1 to Bit 0
}
#else
static __attribute__((interrupt("IRQ"))) void timer0_isr()
{
	time_ms++;
    _T0IR = TxIR_MR0_FLAG;   // Clear interrupt flag by writing 1 to Bit 0
    _VICVectAddr=0;
}
#endif

#define EXT_EINT0_BIT			0
#define EXT_EINT1_BIT			1
#define EXT_EINT2_BIT			2
#define EXT_EINT3_BIT			3

#define EXT_LEVEL_SENSITIVE		0
#define EXT_EDGE_SENSITIVE		1
#define EXT_LOW					0
#define EXT_HIGH				1
#define EXT_FALLING				0
#define EXT_RISING				1

// eint1_isr
//   eint1 ISR (Interrupt Service Routine)
#ifdef __IRQ_HANDLER__
static void eint1_isr()
{
	pushed = 1;
	
    _EXTINT = 1<<EXT_EINT1_BIT;
}
#else
static __attribute__((interrupt("IRQ"))) void eint1_isr()
{
	pushed = 1;

    _EXTINT = 1<<EXT_EINT1_BIT;
    _VICVectAddr=0;
}
#endif

int main()
{
	lcd_init();
	lcd_display(LCD_DISPLAY_DISPLAY_ON);

	//==============================================================
    // config external irq on P0.14
	//==============================================================
	io_configure(_IO0, IO_PIN_14, IO_PIN_FUNC2);
    //_PINSEL0 = (_PINSEL0 & 0xCFFFFFFF) | 0x20000000;  // EINT1 -> P0.14

    // edge sensitive
    _EXTMODE = (_EXTMODE & (~(1<<EXT_EINT1_BIT))) | (EXT_EDGE_SENSITIVE<<EXT_EINT1_BIT);
    // rising edge
    _EXTPOLAR = (_EXTMODE & (~(1<<EXT_EINT1_BIT))) | (EXT_RISING<<EXT_EINT1_BIT);
    // acknoledge pending irq
    _EXTINT = 1<<EXT_EINT1_BIT;
    _VICVectAddr=0;

    // interrupt config
    _VICVectCntl4 = 0x2F;
    _VICVectAddr4 = (uint32_t)eint1_isr;
    _VICIntSelect &= 0xFFFF7FFF;
    _VICIntEnable |= 0x8000;

	//==============================================================
    // config Timer0
	//==============================================================
    _T0CTCR = TxCTCR_CNT_RISEDGE_PCLK;				// count on rising edgle of pclk
    _T0TCR = TxTCR_COUNTER_RESET;					// timer stop and reset
    _T0MR0 = 99;										// Compare-hit at mSec
    _T0MCR = TxMCR_INT_ON_MR0 | TxMCR_RESET_ON_MR0;	// Interrupt and Reset on MR0
    _T0PR  = Fpclk/1000/100-1;

    _T0IR = 0xFF;									// Reset IRQ flags
    _VICVectAddr=0;

    // interrupt config
    _VICVectCntl2 = 0x24;
    _VICVectAddr2 = (uint32_t)timer0_isr;
    _VICIntSelect &= 0xFFFFFFEF;
    _VICIntEnable |= 0x00000010;
	//==============================================================

    irq_enable();

	lcd_printf("%8d ms",time_ms);

    while (1) {
    	switch (state) {
    	case 0:									// initial state
    		if (pushed) {
    			_T0TCR = TxTCR_COUNTER_START;
    			pushed=0;
    			state=1;
    		}
    		break;
    	case 1:									// count
			lcd_goto_xy(1,1);
			lcd_printf("%8d ms",time_ms);
    		if (pushed) {
    			time_inter=time_ms;
    			lcd_goto_xy(1,2);
    			lcd_printf("%8d ms",time_inter);
     			pushed=0;
    			state=2;
    		}
    		break;
    	case 2:									// count, show intermediate time
			lcd_goto_xy(1,1);
			lcd_printf("%8d ms",time_ms);
    		if (pushed) {
    			_T0TCR = TxTCR_COUNTER_STOP;
    			lcd_goto_xy(1,2);
    			lcd_printf("%8d ms",time_ms-time_inter);
     			pushed=0;
    			state=3;
    		}
    		break;
    	case 3:
    		if (pushed) {
    			time_ms=0;
    			lcd_clear();
    			lcd_home();
    			lcd_printf("%8d ms",time_ms);
    			pushed=0;
    			state=0;
    		}
    		break;
    	}
    }

    return 0;
}
#endif
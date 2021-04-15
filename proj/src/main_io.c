#include "board.h"
#include "io.h"

#define MAIN5

extern void wait(int ms);
extern void BdT_start();
extern void BdT_stop();
extern void BdT_decrease_tick(uint32_t yes);
extern uint32_t BdT_tick_done();
extern void BdT_increment();

/*****************************************************************************
 *  leds
 *****************************************************************************/
void leds(uint8_t val)
{
	io_write_n(_IO0, val<<8, 0x0000FF00);
}

/*****************************************************************************
 *  buttons
 *****************************************************************************/
/* buttons
 *   return state of the 4 buttons
 */
uint32_t buttons()
{
    return (((~_IO0->PIN)>>16) & 0x0000001F);
}

int button0()
{
    static uint16_t btn0_state=0xFFFF;
    btn0_state = (btn0_state<<1) | ((_IO0->PIN>>16) & 1) | 0xE000;
    if (btn0_state==0xF000) return 1;
    return 0;
}

int button1()
{
    static uint16_t btn1_state=0xFFFF;
    btn1_state = (btn1_state<<1) | ((_IO0->PIN>>17) & 1) | 0xE000;
    if (btn1_state==0xF000) return 1;
    return 0;
}

int button2()
{
    static uint16_t btn2_state=0xFFFF;
    btn2_state = (btn2_state<<1) | ((_IO0->PIN>>18) & 1) | 0xE000;
    if (btn2_state==0xF000) return 1;
    return 0;
}

int button3()
{
    static uint16_t btn3_state=0xFFFF;
    btn3_state = (btn3_state<<1) | ((_IO0->PIN>>19) & 1) | 0xE000;
    if (btn3_state==0xF000) return 1;
    return 0;
}

int button4()
{
    static uint16_t btn4_state=0xFFFF;
    btn4_state = (btn4_state<<1) | ((_IO0->PIN>>20) & 1) | 0xE000;
    if (btn4_state==0xF000) return 1;
    return 0;
}

/*****************************************************************************
 *  main
 *****************************************************************************/
#ifdef MAIN1
int main()
{
	// P0.8:15 -> output (leds)
	io_configure(_IO0,0x0000FF00,IO_PIN_OUTPUT);
	// P0.16:20 -> input (push button)
	io_configure(_IO0,0x001F0000,IO_PIN_INPUT);

    leds(0);
    leds(1);
    leds(2);
    leds(0xD);
    leds(0xBC);
    leds(0xFF);

    while (1) {
    	leds(buttons());
    }

	return 0;
}
#endif

#ifdef MAIN2
int main()
{
	// P0.8:15 -> output (leds)
	io_configure(_IO0,0x0000FF00,IO_PIN_OUTPUT);
	// P0.16:20 -> input (push button)
	io_configure(_IO0,0x001F0000,IO_PIN_INPUT);

    leds(0);

    while(1) {
        leds(1);
        wait(500);
        leds(0);
        wait(500);
    }
    return 0;
}
#endif

#ifdef MAIN3
int main()
{
    uint32_t  cpt=1;

	// P0.8:15 -> output (leds)
	io_configure(_IO0,0x0000FF00,IO_PIN_OUTPUT);
	// P0.16:20 -> input (push button)
	io_configure(_IO0,0x001F0000,IO_PIN_INPUT);

    leds(0);

    while (1) {
        leds(cpt);
        wait(500);
        cpt=cpt<<1;
        if (cpt>0x80) cpt=1;
    }
    return 0;
}
#endif

#ifdef MAIN4
int main()
{
    uint32_t  cpt=0;

	// P0.8:15 -> output (leds)
	io_configure(_IO0,0x0000FF00,IO_PIN_OUTPUT);
	// P0.16:20 -> input (push button)
	io_configure(_IO0,0x001F0000,IO_PIN_INPUT);

    leds(0);

    while (1) {
        if (button3())
          leds(++cpt);
        wait(1);
    }
    return 0;
}
#endif

#ifdef MAIN5
/*****************************************************************************
 *  Chenillard
 *****************************************************************************/
uint8_t  leds_reg=1;
uint32_t chenillard_on=0;
uint32_t sens=0;

int main()
{
 	// P0.8:15 -> output (leds)
	io_configure(_IO0,0x0000FF00,IO_PIN_OUTPUT);
	// P0.16:20 -> input (push button)
	io_configure(_IO0,0x001F0000,IO_PIN_INPUT);

    leds(leds_reg);

    while (1) {
        uint32_t sw0=button0();
        uint32_t sw1=button1();
        uint32_t sw2=button2();
        uint32_t sw3=button3();

        if (sw0) {
            chenillard_on=!chenillard_on;
            if (chenillard_on) {
                // start BdT
                BdT_start();
            } else {
                // stop BdT
                BdT_stop();
            }
        } else if (chenillard_on && sw1) {
            sens=!sens;
            // inverser le sens
        } else if (chenillard_on && sw3) {
            // augmenter la vitesse
            BdT_decrease_tick(1);
        } else if (chenillard_on && sw2) {
            // diminuer la vitesse
            BdT_decrease_tick(0);
        }
        if (BdT_tick_done()) {
            if (sens) {
                leds_reg=leds_reg>>1;
                if (leds_reg==0x00) leds_reg=0x80;
                leds(leds_reg);
            } else {
                leds_reg=leds_reg<<1;
                if (leds_reg==0x00) leds_reg=1;
                leds(leds_reg);
            }
        }
        BdT_increment();
    }
    return 0;
}
#endif

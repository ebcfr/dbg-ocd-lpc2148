#include "board.h"
#include "irq.h"
#include "io.h"
#include "timer.h"
#include "term_io.h"
#include "lexer.h"


/****************************************************************
 * Stepper-motor control
 ****************************************************************/
typedef volatile struct {
	int		busy;
	int		clockwise;
	int 	steps_per_cycle;
	int		state;
	int		nsteps;
} stepper_t;

static stepper_t stepper = {
	.steps_per_cycle = 20,
	.state = 0,
	.busy = 0,
};

#define MAX_STATE	4

static const uint8_t seq[2][MAX_STATE] = {{1,1,0,0},{0,1,1,0}};


static void timer_cb()
{
	if (stepper.clockwise) {
		stepper.state = (stepper.state+1) % MAX_STATE;
	} else {
		stepper.state--;
		if (stepper.state<0) stepper.state=MAX_STATE-1;
	}
	io_write(_IO0, seq[0][stepper.state]<<21, IO_PIN_21);
	io_write(_IO0, seq[1][stepper.state]<<12, IO_PIN_12);
	stepper.nsteps--;
	if (!stepper.nsteps) {
		timer_stop(_TIMER1);
		stepper.busy = 0;
	}
}

/* rotate(uint32_t nsteps, int step_period_ms)
 *
 */
static void rotate(int nsteps, int step_period_ms)
{
	if (!nsteps || !step_period_ms) return;
	stepper.busy = 1;
	if (nsteps>0) {
		stepper.clockwise=1;
		stepper.nsteps=nsteps;
	} else {
		stepper.clockwise=0;
		stepper.nsteps=-nsteps;
	}
	timer_tick_init(_TIMER1,step_period_ms,timer_cb);
	timer_start(_TIMER1);
	while (stepper.busy) ;
}

/****************************************************************
 * main
 ****************************************************************/
int main()
{
	char text[80];
	
	/* configure pins */
	io_configure(_IO0, IO_PIN_12|IO_PIN_21, IO_PIN_OUTPUT);
	irq_enable();
	/* initialize motor */
	rotate(1,100);
	
	term_init(_UART0, 80, 24);
	term_clrscr();
	
	while(1) {
		readline(">", text, 60);
		lexer_reset(text);
		next_token();
		switch(token) {
		case T_STEP:
			next_token();
			if (token==T_NUM) rotate(token_val,100);
			term_printf("\r\n");
			break;
		default:
			term_printf("\r\nUnknown command\r\n");
			break;
		}
	}
	return 0;
}

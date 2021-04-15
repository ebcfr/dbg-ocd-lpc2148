#include <stdlib.h>
#include "dmd.h"
#include "spi.h"
#include "io.h"
#include "timer.h"
#include "eint.h"

static volatile int tc=0;			// time count variable
static uint8_t* frames=NULL;			// pointer to the data
static uint32_t init_col=0;			// first col to be drawn
static uint32_t frame_size;			// data size in bytes
static int32_t cur_col=0;				// current data column index to be displayed
static uint8_t col_mask=0x80;			// current mask for col selection
static uint32_t frame_mode;			// frame mode used
static uint32_t frame_period=1000;	// updating frame period

/* dot matrix display refresh callback
 *
 */
static void tick_cb()
{
	uint8_t data[2];
	data[0] = ~(frames[(init_col+cur_col) % (frame_size)]);
	data[1] = ~col_mask;

	cur_col = (cur_col+1) % 8; //(nframes*8);
	if (col_mask==0x01) col_mask=0x80;
	else col_mask=col_mask>>1;

	spi_write(data, 2);				// send data (non blocking)

	tc++;
	if (tc==frame_period) {
		switch (frame_mode) {
		case FRAME_MODE_REPLACE:
			init_col=(init_col+8) % frame_size;
			break;
		case FRAME_MODE_SCROLL:
			init_col=(init_col+1) % frame_size;
			break;
		default:
			break;
		}
		tc=0;
	}
}
/* dmd_init
 * 74HC595 shift registers are used for colum and row selection
 */
void dmd_init()
{
	timer_tick_init(_TIMER0, 1, tick_cb);
	spi_init();
}

/* dmd_scroll_period
 *
 * change the period used for updating scrolling
 */
void dmd_frame_period(uint32_t period_ms)
{
	frame_period=period_ms;
}

/* dmd_put_image
 *
 */
void dmd_put_image(uint8_t* data)
{
	/* timer_stop(_TIMER0);
	
	frames=data;
	frame_size=8;
	frame_mode=FRAME_MODE_NONE;
	init_col=0;
	cur_col=0;
	col_mask=0x80;
	tc=0;
	timer_start(_TIMER0); */
	dmd_put_images(data,8,FRAME_MODE_NONE);
}

/* dmd_put_images
 *
 */
void dmd_put_images(uint8_t* data, uint32_t n, uint32_t mode)
{
	timer_stop(_TIMER0);
	
	frames=data;
	frame_size=n;
	frame_mode=mode;
	init_col=0;
	cur_col=0;
	col_mask=0x80;
	tc=0;
	timer_start(_TIMER0);
}

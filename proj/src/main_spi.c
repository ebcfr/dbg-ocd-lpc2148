#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "irq.h"
#include "uart.h"
#include "spi.h"
#include "dmd.h"
#include "timer.h"
#include "converter.h"
#include "term_io.h"
#include "i2c.h"
#include "lm75.h"

#define MAIN3

#ifdef MAIN1
//---------------------------------------------------------------------------
//  spi test
//---------------------------------------------------------------------------
int main()
{
	uint8_t data[2];

	spi_init();

	irq_enable();

	data[0]=(uint8_t)~0x03;
	data[1]=(uint8_t)~(1<<6);

	spi_write(data,2);

	while (1) {

	}
	return 0;
}
#endif

#ifdef MAIN2
//---------------------------------------------------------------------------
//  led dot matrix display : put a picture (a sandglass)
//---------------------------------------------------------------------------
uint8_t buf[]={0xC3,0xE5,0xF9,0xF1,0xF1,0xF9,0xE5,0xC3};

int main()
{
	dmd_init();

	irq_enable();

	dmd_put_image(buf);

	while (1) {

	}
	return 0;
}
#endif

#ifdef MAIN3
//---------------------------------------------------------------------------
//  led dot matrix display : animation (an animated sandglass)
//---------------------------------------------------------------------------
Symbol s0={
"........",
"........",
" ...... ",
"  ....  ",
"  .  .  ",
" .    . ",
".      .",
"........"
};

Symbol s1={
"........",
"........",
" ...... ",
"  . ..  ",
"  . ..  ",
" .  . . ",
".   .  .",
"........"
};

Symbol s2={
"........",
"........",
" .  ... ",
"  . ..  ",
"  . ..  ",
" .  . . ",
"........",
"........"
};

Symbol s3={
"........",
".   ....",
" .  ... ",
"  . ..  ",
"  . ..  ",
" ...... ",
"........",
"........"
};

Symbol s4={
"........",
".      .",
" .    . ",
"  .  .  ",
"  ....  ",
" ...... ",
"........",
"........"
};

Symbol *data[]={&s0,&s1,&s2,&s3,&s4,NULL};

uint8_t buf[8*5];

int main()
{
	uint32_t nb;

	convert_init();
	nb=convert_stream(buf, data, 8);

	dmd_init();
	dmd_frame_period(800);

	irq_enable();

	dmd_put_images(buf, nb, FRAME_MODE_REPLACE);

	while (1) {

	}
	return 0;
}
#endif

#ifdef MAIN4
//---------------------------------------------------------------------------
//  led dot matrix display : text scrolling
//---------------------------------------------------------------------------
uint8_t  buf[512];

int main()
{
	convert_init();
	dmd_init();
	dmd_frame_period(200);

	irq_enable();

	uint32_t nb=convert_text(buf, "* HELLO ");
	dmd_put_images(buf,nb,FRAME_MODE_SCROLL);

	while(1) {

	}
	return 0;
}
#endif

#ifdef MAIN5
//---------------------------------------------------------------------------
//  led dot matrix display : text scrolling
//---------------------------------------------------------------------------
uint8_t  buf[512];
char text[80];

int main()
{
	int temp;

	convert_init();
	term_init(_UART0, 24, 80);
	i2c_master_init();
	dmd_init();
	dmd_frame_period(200);

	irq_enable();

	lm75_read_temp(&temp);
	uint32_t nb=convert_ftext(buf,"* %d.%d DEGRES ",temp>>1,temp&1 ? 5 : 0);
	dmd_put_images(buf,nb,FRAME_MODE_SCROLL);

	while(1) {
		term_clrscr();
		readline("message>", text, 60);
		if (!strcmp("temp",text)) {
			lm75_read_temp(&temp);
			uint32_t nb=convert_ftext(buf,"* %d.%d DEGRES ",temp>>1,temp&1 ? 5 : 0);
			dmd_put_images(buf,nb,FRAME_MODE_SCROLL);
		} else {
			nb=convert_text(buf, text);
			dmd_put_images(buf,nb,FRAME_MODE_SCROLL);
		}
	}
	return 0;
}
#endif

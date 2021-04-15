/***************************************************************************
 *
 *  LCD Module for the LPC2148
 *
 ***************************************************************************/
#include <stdarg.h>

#include "lcd.h"
#include "io.h"

/*
 * LCD pin assignment (Low level access and utility functions)
 *
 */

/* _IO1 pins */
#define LCD_D0        IO_PIN_16
#define LCD_D1        IO_PIN_17
#define LCD_D2        IO_PIN_18
#define LCD_D3        IO_PIN_19
#define LCD_D4        IO_PIN_20
#define LCD_D5        IO_PIN_21
#define LCD_D6        IO_PIN_22
#define LCD_D7        IO_PIN_23
#define LCD_DATA      (LCD_D7 | LCD_D6 | LCD_D5 | LCD_D4 | LCD_D3 | LCD_D2 | LCD_D1 | LCD_D0)
#define LCD_SHIFT     ((unsigned int) 16)
#define LCD_RS        IO_PIN_24
#define LCD_E         IO_PIN_25

/* _IO0 pins */
#define LCD_RW        IO_PIN_22
#define LCD_BL        IO_PIN_30

#define LCD_PWM_FREQ 20000

static inline void delay(int r)
{
	while (r--);
}

#define LCD_EN(x) 			if (x) _IO1->SET=LCD_E; else _IO1->CLR=LCD_E
#define LCD_RNW(x)			if (x) _IO0->SET=LCD_RW; else _IO0->CLR=LCD_RW
#define LCD_RS_DATA()		(_IO1->SET=LCD_RS)
#define LCD_RS_CMD()		(_IO1->CLR=LCD_RS)

#define LCD_DELAY_140NS()	(delay(10))
#define LCD_DELAY_1US()		(delay(80))

#define LCD_DATA_IN()		((_IO1->PIN & LCD_DATA) >> LCD_SHIFT)
#define LCD_DATA_OUT(c)		_IO1->CLR=(~c & 0xFF) << LCD_SHIFT; _IO1->SET=(c & 0xFF) << LCD_SHIFT

#define LCD_DATA_DIR(out)	if (out) _IO1->DIR|=LCD_DATA; else _IO1->DIR&=~LCD_DATA
#define OUTPUT				1
#define INPUT				0

static void lcd_send_data(unsigned char c)
{
	LCD_RS_DATA();
	LCD_RNW(0);

	LCD_EN(1);
	LCD_DATA_OUT(c);
	LCD_DELAY_140NS();
	LCD_EN(0);
	LCD_DELAY_1US();
}

static void lcd_send_cmd(unsigned char c)
{
	LCD_RS_CMD();
	LCD_RNW(0);

	LCD_EN(1);
	LCD_DATA_OUT(c);
	LCD_DELAY_140NS();
	LCD_EN(0);
	LCD_DELAY_1US();
}

static int lcd_get_status(void)
{
	unsigned char c;

	LCD_RS_CMD();
	LCD_RNW(1);

	LCD_EN(1);
	LCD_DATA_DIR(INPUT);
	LCD_DELAY_140NS();
	c = LCD_DATA_IN();
	LCD_EN(0);
	LCD_RNW(0);
	LCD_DATA_DIR(OUTPUT);
	LCD_DELAY_1US();
  
	return c;
}

static void lcd_is_busy(void)
{
	while (lcd_get_status() & 0x80);
}

/*
 *  Public interface
 */
/* HD44780 Commands */
#define LCD_COMMAND_CLEAR		0x01
#define LCD_COMMAND_HOME        0x02
#define LCD_COMMAND_MODE        0x04
#define LCD_COMMAND_DISPLAY     0x08
#define LCD_COMMAND_SHIFT       0x10
#define LCD_COMMAND_FUNCTION    0x20
#define LCD_COMMAND_CGRAM       0x40
#define LCD_COMMAND_DDRAM       0x80

/*
 *	LCD_FUNCTION MACROS
 */
#define LCD_FUNCTION_FONT_5X8   0x00
#define LCD_FUNCTION_FONT_5X11  0x04
#define LCD_FUNCTION_LINES_1    0x00
#define LCD_FUNCTION_LINES_2    0x08
#define LCD_FUNCTION_BUS_4      0x00
#define LCD_FUNCTION_BUS_8      0x10
#define LCD_FUNCTION_MASK       0x1c

void lcd_init(void)
{
	// io port setup
	io_configure(_IO0, LCD_RW | LCD_BL, IO_PIN_OUTPUT);
	io_configure(_IO1, LCD_DATA | LCD_E | LCD_RS, IO_PIN_OUTPUT);

	// back_light on
	_IO0->SET=LCD_BL;
	
	// signals setup
	LCD_EN(0);
	LCD_RNW(0);
	LCD_RS_CMD();
	LCD_DATA_OUT(0xff);

	// Initialize controller
	lcd_send_cmd(LCD_COMMAND_FUNCTION | LCD_FUNCTION_BUS_8 | LCD_FUNCTION_LINES_2 | LCD_FUNCTION_FONT_5X8);  delay(2500);
	lcd_send_cmd(LCD_COMMAND_FUNCTION | LCD_FUNCTION_BUS_8 | LCD_FUNCTION_LINES_2 | LCD_FUNCTION_FONT_5X8);  delay(2500);
	lcd_send_cmd(LCD_COMMAND_FUNCTION | LCD_FUNCTION_BUS_8 | LCD_FUNCTION_LINES_2 | LCD_FUNCTION_FONT_5X8);  delay(2500);
	lcd_send_cmd(LCD_COMMAND_FUNCTION | LCD_FUNCTION_BUS_8 | LCD_FUNCTION_LINES_2 | LCD_FUNCTION_FONT_5X8);  delay(2500);

	lcd_display(LCD_DISPLAY_DISPLAY_ON | LCD_DISPLAY_CURSOR_OFF | LCD_DISPLAY_BLINK_OFF);
	lcd_clear();
	lcd_mode(LCD_MODE_INCR | LCD_MODE_NOSHIFT);
	lcd_shift(LCD_SHIFT_CURSOR|LCD_SHIFT_LEFT);
	lcd_goto_xy(1, 1);
}

int lcd_backlight(int on)
{
	unsigned int state=(_IO0->PIN & LCD_BL)? 1 : 0;
	if (on) _IO0->SET=LCD_BL;
	else _IO0->CLR=LCD_BL;
	return state;
}

void lcd_display(unsigned char display)
{
	lcd_is_busy();
	lcd_send_cmd(LCD_COMMAND_DISPLAY | (display & LCD_DISPLAY_MASK));
}

void lcd_mode(unsigned char mode)
{
	lcd_is_busy ();
	lcd_send_cmd(LCD_COMMAND_MODE | (mode & LCD_MODE_MASK));
}

void lcd_shift(unsigned char shift)
{
	lcd_is_busy();
	lcd_send_cmd(LCD_COMMAND_SHIFT | (shift & LCD_SHIFT_MASK));
}

void lcd_clear(void)
{
	lcd_is_busy();
	lcd_send_cmd(LCD_COMMAND_CLEAR);
}

void lcd_home(void)
{
	lcd_is_busy();
	lcd_send_cmd(LCD_COMMAND_HOME);
}

void lcd_goto_xy(unsigned int x, unsigned int y)
{
	lcd_is_busy();
	lcd_send_cmd(LCD_COMMAND_DDRAM | ((y-1) ? 0x40 : 0x00) | ((x-1) & 0x0f));
	lcd_is_busy();
}

void lcd_put_char(char c)
{
	lcd_is_busy();
	if ((c>=0x10) && (c<0x18)) c-=0x10;
	lcd_send_data(c);
}

void lcd_put_string(const char *s)
{
	int i=0;
	
	while (s[i]) lcd_put_char(s[i++]);
}

/****************************************************************************
 * lcd_printf
 *   simple printf for lcd display
 *
 ****************************************************************************/
static char  hexChars[] = "0123456789ABCDEF";
// recursive version :)
/*
static void num2str(char **s, unsigned int number, unsigned int base)
{
	if (number/base) num2str(s, number/base, base);
	*(*s)++ = hexChars[number % base];
	**s = '\0';
}
*/

static void num2str(char *s, unsigned int number, unsigned int base, unsigned int size, unsigned int sp)
{
	char *p=s;
	unsigned int cnt;
	int i;
	char tmp;

	// get digits
	do {
		*s++=hexChars[number % base];
	} while (number /= base);
	*s='\0';

	// reverse string
	cnt=s-p;
	for (i=0;i<cnt/2;i++) {
		tmp=p[i]; p[i] = p[cnt-i-1]; p[cnt-i-1]=tmp;
	}

	// add extra space
	if (cnt<size) {
		for (i=cnt;i>=0;i--) p[i+size-cnt]=p[i];
		if (sp) tmp=' '; else tmp='0';
		for (i=0;i<size-cnt;i++) p[i]=tmp;
	}
}

static unsigned int str2num(char *s, unsigned base)
{
	unsigned int u=0, d;
	char ch=*s++;
	while (ch) {
		if ((ch>='0') && (ch<='9')) d=ch-'0';
		else if ((ch>='A') && (ch<='F')) d=ch-'A'+10;
		else if ((ch>='a') && (ch<='f')) d=ch-'a'+10;
		else break;
		u=d+base*u;
		ch=*s++;
	}
	return u;
}

void lcd_printf(const char* fmt, ...)
{
	va_list        ap;
	char          *p;
	char           ch;
	unsigned long  ul;
	unsigned long  size;
	unsigned int   sp;
	char           s[34];
	
	va_start(ap, fmt);
	while (*fmt != '\0') {
		if (*fmt =='%') {
			size=0; sp=1;
			if (*++fmt=='0') {fmt++; sp=0;}	// parse %04d --> sp=0
			ch=*fmt;
			if ((ch>'0') && (ch<='9')) {	// parse %4d --> size=4
				char tmp[10];
				int i=0;
				while ((ch>='0') && (ch<='9')) {
					tmp[i++]=ch;
					ch=*++fmt;
				}
				tmp[i]='\0';
				size=str2num(tmp,10);
			}
			switch (ch) {
				case '%':
					lcd_put_char('%');
					break;
				case 'c':
					ch = va_arg(ap, unsigned int);
					lcd_put_char(ch & 0x7f);
					break;
				case 's':
					p = va_arg(ap, char *);
					lcd_put_string(p);
					break;
				case 'd':
					ul = va_arg(ap, long);
					if ((long)ul < 0) {
						lcd_put_char('-');
						ul = -(long)ul;
						size--;
					}
					num2str(s, ul, 10, size, sp);
					lcd_put_string(s);
					break;
				case 'u':
					ul = va_arg(ap, unsigned int);
					num2str(s, ul, 10, size, sp);
					lcd_put_string(s);
					break;
				case 'o':
					ul = va_arg(ap, unsigned int);
					num2str(s, ul, 8, size, sp);
					lcd_put_string(s);
					break;
				case 'p':
					lcd_put_char('0');
					lcd_put_char('x');
					ul = va_arg(ap, unsigned int);
					num2str(s, ul, 16, size, sp);
					lcd_put_string(s);
					break;
				case 'x':
					ul = va_arg(ap, unsigned int);
					num2str(s, ul, 16, size, sp);
					lcd_put_string(s);
					break;
				default:
				    lcd_put_char(*fmt);
			}
		} else lcd_put_char(*fmt);
		fmt++;
	}
	va_end(ap);
}

/*
 * lcd_new_char 
 *
 * it allows to make up to 8 user defined characters in CGRAM
 *
 * n    : user defined char id (0 <= n <= 7)
 * data : must be a pointer to an 8 char array filled with the char mask (5*8)
 *  -----
 * |   # | 0x02
 * |  #  | 0x04
 * | ### | 0x0E
 * |#   #| 0x11
 * |#####| 0x1F
 * |#    | 0x10
 * | ### | 0x0E
 * |     | 0x00
 *  -----
 * lcd_goto_xy() must be called after using lcd_new_char to switch back to
 * DDRAM output
 *
 * Warning ! n=0 is assigned to ascii 0x00 so that it can't be used in C string
 * As a work around, special characters are used with ascii codes 0x10 to 0x17
 */
void lcd_new_char(unsigned char n, unsigned char *data)
{
	int i;
	
	lcd_is_busy();
	lcd_send_cmd(LCD_COMMAND_CGRAM | ((n<<3) & 0x38));
	for (i=0;i<8;i++) {
		lcd_is_busy();
		lcd_send_data(data[i]);
	}
}

#include "converter.h"
#include "util.h"

//char *none[]={
Symbol none={
".... ",
".... ",
".... ",
".... ",
".... ",
".... ",
".... ",
"     "
};

//char *A[]={
Symbol A={
" ..  ",
".  . ",
".  . ",
".... ",
".  . ",
".  . ",
".  . ",
"     "
};

//char *B[]={
Symbol B={
"...  ",
".  . ",
".  . ",
"...  ",
".  . ",
".  . ",
"...  ",
"     "
};

//char *C[]={
Symbol C={
" ..  ",
".  . ",
".    ",
".    ",
".    ",
".  . ",
" ..  ",
"     "
};

//char *D[]={
Symbol D={
"...  ",
".  . ",
".  . ",
".  . ",
".  . ",
".  . ",
"...  ",
"     "
};

//char *E[]={
Symbol E={
".... ",
".    ",
".    ",
"...  ",
".    ",
".    ",
".... ",
"     "
};

//char *F[]={
Symbol F={
".... ",
".    ",
".    ",
"...  ",
".    ",
".    ",
".    ",
"     "
};

//char *G[]={
Symbol G={
" ..  ",
".  . ",
".    ",
".    ",
". .. ",
".  . ",
" ... ",
"     "
};

//char *H[]={
Symbol H={
".  . ",
".  . ",
".  . ",
".... ",
".  . ",
".  . ",
".  . ",
"     "
};

//char *I[]={
Symbol I={
"... ",
" .  ",
" .  ",
" .  ",
" .  ",
" .  ",
"... ",
"    "
};

//char *J[]={
Symbol J={
"   . ",
"   . ",
"   . ",
"   . ",
"   . ",
".  . ",
" ..  ",
"     "
};

//char *K[]={
Symbol K={
".  . ",
". .  ",
"..   ",
".    ",
"..   ",
". .  ",
".  . ",
"     "
};

//char *L[]={
Symbol L={
".    ",
".    ",
".    ",
".    ",
".    ",
".    ",
".... ",
"     "
};

//char *M[]={
Symbol M={
".     .",
"..   .. ",
". . . . ",
".  .  . ",
".     . ",
".     . ",
".     . ",
"        "
};

//char *N[]={
Symbol N={
".   . ",
"..  . ",
"..  . ",
". . . ",
".  .. ",
".  .. ",
".   . ",
"      "
};

//char *O[]={
Symbol O={
" ..  ",
".  . ",
".  . ",
".  . ",
".  . ",
".  . ",
" ..  ",
"     "
};

//char *P[]={
Symbol P={
"...  ",
".  . ",
".  . ",
"...  ",
".    ",
".    ",
".    ",
"     "
};

//char *Q[]={
Symbol Q={
" ..   ",
".  .  ",
".  .  ",
".  .  ",
". ..  ",
".  .. ",
" .. . ",
"      "
};

//char *R[]={
Symbol R={
"...  ",
".  . ",
".  . ",
"...  ",
"..   ",
". .  ",
".  . ",
"     "
};

//char *S[]={
Symbol S={
" ..  ",
".  . ",
".    ",
" ..  ",
"   . ",
".  . ",
" ..  ",
"     "
};

//char *T[]={
Symbol T={
"..... ",
"  .   ",
"  .   ",
"  .   ",
"  .   ",
"  .   ",
"  .   ",
"      "
};

//char *U[]={
Symbol U={
".  . ",
".  . ",
".  . ",
".  . ",
".  . ",
".  . ",
" ..  ",
"     "
};

//char *V[]={
Symbol V={
".   . ",
".   . ",
".   . ",
" . .  ",
" . .  ",
" . .  ",
"  .   ",
"      "
};

//char *W[]={
Symbol W={
".     . ",
".  .  . ",
".  .  . ",
" .. ..  ",
" .. ..  ",
" .. ..  ",
"  . .   ",
"        "
};

//char *X[]={
Symbol X={
".     . ",
" .   .  ",
"  . .   ",
"   .    ",
"  . .   ",
" .   .  ",
".     . ",
"        "
};

//char *Y[]={
Symbol Y={
".     . ",
" .   .  ",
"  . .   ",
"   .    ",
"   .    ",
"   .    ",
"   .    ",
"        "
};

//char *Z[]={
Symbol Z={
"..... ",
"    . ",
"   .  ",
"  .   ",
" .    ",
".     ",
"..... ",
"      "
};


//char *zero[]={
Symbol zero={
" ...  ",
".   . ",
".  .. ",
". . . ",
"..  . ",
".   . ",
" ...  ",
"      "
};

//char *one[]={
Symbol one={
"  . ",
" .. ",
". . ",
"  . ",
"  . ",
"  . ",
"  . ",
"    "
};

//char *two[]={
Symbol two={
" ..  ",
".  . ",
"   . ",
"  .  ",
" .   ",
".    ",
".... ",
"     "
};

//char *three[]={
Symbol three={
" ..  ",
".  . ",
"   . ",
"  .  ",
"   . ",
".  . ",
" ..  ",
"     "
};

//char *four[]={
Symbol four={
"   .  ",
"  ..  ",
" . .  ",
".  .  ",
"..... ",
"   .  ",
"   .  ",
"      "
};

//char *five[]={
Symbol five={
".... ",
".    ",
".    ",
"...  ",
"   . ",
".  . ",
" ..  ",
"     "
};

//char *six[]={
Symbol six={
" ..  ",
".  . ",
".    ",
"...  ",
".  . ",
".  . ",
" ..  ",
"     "
};

//char *seven[]={
Symbol seven={
".... ",
"   . ",
"  .  ",
" .   ",
".    ",
".    ",
".    ",
"     "
};

//char *eight[]={
Symbol eight={
" ..  ",
".  . ",
".  . ",
" ..  ",
".  . ",
".  . ",
" ..  ",
"     "
};

//char *nine[]={
Symbol nine={
" ..  ",
".  . ",
".  . ",
" ... ",
"   . ",
"   . ",
" ..  ",
"     "
};

//char *minus[]={
Symbol minus={
"      ",
"      ",
"      ",
"..... ",
"      ",
"      ",
"      ",
"      "
};

//char *plus[]={
Symbol plus={
"      ",
"  .   ",
"  .   ",
"..... ",
"  .   ",
"  .   ",
"      ",
"      "
};

//char *mul[]={
Symbol mul={
"      ",
"  .   ",
". . . ",
" ...  ",
". . . ",
"  .   ",
"      ",
"      "
};

//char *div[]={
Symbol divide={
"      ",
"  .   ",
"      ",
"..... ",
"      ",
"  .   ",
"      ",
"      "
};

//char *equal[]={
Symbol equal={
"      ",
"      ",
"..... ",
"      ",
"..... ",
"      ",
"      ",
"      "
};

//char *tilde[]={
Symbol tilde={
"      ",
"      ",
" .    ",
". . . ",
"   .  ",
"      ",
"      ",
"      "
};

//char *hyphen[]={
Symbol hyphen={
"    ",
"    ",
"    ",
"... ",
"    ",
"    ",
"    ",
"    "
};

//char *excl_mark[]={
Symbol excl_mark={
". ",
". ",
". ",
". ",
". ",
"  ",
". ",
"  "
};

//char *question_mark[]={
Symbol question_mark={
" .  ",
". . ",
"  . ",
" .  ",
" .  ",
"    ",
" .  ",
"    "
};

//char *ampersand[]={
Symbol ampersand={
" .    ",
". .   ",
" .    ",
". .   ",
".  .. ",
".  .  ",
" .. . ",
"      "
};

//char *at[]={
Symbol at={
" ...  ",
".   . ",
"    . ",
" .. . ",
". . . ",
". . . ",
" ...  ",
"      "
};

//char *space[]={
Symbol space={
" ",
" ",
" ",
" ",
" ",
" ",
" ",
" "
};

//char *dot[]={
Symbol dot={
"  ",
"  ",
"  ",
"  ",
"  ",
"  ",
". ",
"  "
};


Symbol degree={
" .  ",
". . ",
" .  ",
"    ",
"    ",
"    ",
"    ",
"    "
};

//char *smiley[]={
Symbol smiley={
" ......  ",
". .  . . ",
".   .  . ",
".  ..  . ",
". .  . . ",
".  ..  . ",
" ......  ",
"         "
};

static Symbol * symtable[256];

void convert_init()
{
	int i;
	for (i=0;i<256;i++) symtable[i]=&none;

	symtable['A']=&A;
	symtable['B']=&B;
	symtable['C']=&C;
	symtable['D']=&D;
	symtable['E']=&E;
	symtable['F']=&F;
	symtable['G']=&G;
	symtable['H']=&H;
	symtable['I']=&I;
	symtable['J']=&J;
	symtable['K']=&K;
	symtable['L']=&L;
	symtable['M']=&M;
	symtable['N']=&N;
	symtable['O']=&O;
	symtable['P']=&P;
	symtable['Q']=&Q;
	symtable['R']=&R;
	symtable['S']=&S;
	symtable['T']=&T;
	symtable['U']=&U;
	symtable['V']=&V;
	symtable['W']=&W;
	symtable['X']=&X;
	symtable['Y']=&Y;
	symtable['Z']=&Z;
	
	symtable['0']=&zero;
	symtable['1']=&one;
	symtable['2']=&two;
	symtable['3']=&three;
	symtable['4']=&four;
	symtable['5']=&five;
	symtable['6']=&six;
	symtable['7']=&seven;
	symtable['8']=&eight;
	symtable['9']=&nine;
	
	symtable['+']=&plus;
	symtable['-']=&minus;
	symtable['*']=&mul;
	symtable['/']=&divide;
	
	symtable['~']=&tilde;
	symtable['!']=&excl_mark;
	symtable['?']=&question_mark;
	symtable['&']=&ampersand;
	symtable['@']=&at;
	symtable[' ']=&space;
	symtable['.']=&dot;
}

void convert_symbol(uint8_t *buf, Symbol s)
{
	int r,c;
	for (c=0;c<8;c++) {
		uint8_t val=0;
		uint8_t mask=0x80;
		for (r=0;r<8;r++) {
			if (s[r][c]=='.') {
				val |= mask;
			}
			mask=mask>>1;
		}
		*buf++=val;
	}
}

// convert a stream of symbol, last must be a NULL pointer
uint32_t convert_stream(uint8_t *buf, Symbol *s[], int32_t sym_width)
{
	uint32_t cnt=0;
	while (*s) {
		int r,c=0;
		while (1) {
			uint8_t val=0;
			uint8_t mask=0x80;
			for (r=0;r<8;r++) {
				if ((**s)[r][c]=='.') {
					val |= mask;
				}
				mask=mask>>1;
			}
			*buf++=val;
			cnt++;
			c++;
			if (!sym_width) {
				if (!val) break;
			} else if (c==sym_width) break;
		}
		s++;
	}
	return cnt;
}

uint32_t convert_text(uint8_t *buf, char* s)
{
	uint32_t cnt=0;
	while (*s) {
		Symbol *sym=symtable[(unsigned int)*s];
		int r,c=0;
		while(1) {
			uint8_t val=0;
			uint8_t mask=0x80;
			for (r=0;r<8;r++) {
				if ((*sym)[r][c]=='.') {
					val |= mask;
				}
				mask=mask>>1;
			}
			*buf++=val;
			cnt++;
			c++;
			if (!val) break;
		}
		s++;
	}
	return cnt;
}

#include <stdarg.h>

static uint32_t convert_char(char ch, uint8_t *buf)
{
	uint32_t cnt=0;
	Symbol *sym=symtable[(unsigned int)ch];
	int r,c=0;
	while(1) {
		uint8_t val=0;
		uint8_t mask=0x80;
		for (r=0;r<8;r++) {
			if ((*sym)[r][c]=='.') {
				val |= mask;
			}
			mask=mask>>1;
		}
		*buf++=val;
		cnt++;
		c++;
		if (!val) break;
	}
	return cnt;
}

static uint32_t convert_str(char* s, uint8_t *buf)
{
	uint32_t cnt=0;
	while (*s) {
		Symbol *sym=symtable[(unsigned int)*s];
		int r,c=0;
		while(1) {
			uint8_t val=0;
			uint8_t mask=0x80;
			for (r=0;r<8;r++) {
				if ((*sym)[r][c]=='.') {
					val |= mask;
				}
				mask=mask>>1;
			}
			*buf++=val;
			cnt++;
			c++;
			if (!val) break;
		}
		s++;
	}
	return cnt;
}

uint32_t convert_ftext(uint8_t* buf, const char* fmt, ...)
{
	va_list        ap;
	char          *p;
	char           ch;
	unsigned long  ul;
	char           s[34];
	uint8_t         *pbuf=buf;

	va_start(ap, fmt);
	while (*fmt != '\0') {
		if (*fmt =='%') {
			switch (*++fmt) {
				case '%':
					buf=buf+convert_char('%',buf);
					break;
				case 'c':
					ch = va_arg(ap, int);
					buf=buf+convert_char(ch,buf);
					break;
				case 's':
					p = va_arg(ap, char *);
					buf=buf+convert_str(p,buf);
					break;
				case 'd':
					ul = va_arg(ap, long);
					if ((long)ul < 0) {
						buf=buf+convert_char('-',buf);
						ul = -(long)ul;
					}
					num2str(s, ul, 10);
					buf=buf+convert_str(s,buf);
					break;
				case 'u':
					ul = va_arg(ap, unsigned int);
					num2str(s, ul, 10);
					buf=buf+convert_str(s, buf);
					break;
				case 'x':
					ul = va_arg(ap, unsigned int);
					num2str(s, ul, 16);
					buf=buf+convert_str(s, buf);
					break;
				default:
					buf=buf+convert_char(*fmt,buf);
			}
		} else buf=buf+convert_char(*fmt,buf);
		fmt++;
	}
	va_end(ap);
	return buf-pbuf;
}

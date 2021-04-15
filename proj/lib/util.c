#include "util.h"

/* num2str
 *   convert a number 'number' in base 'base' to the string s (the string
 *   variable must be large enough)
 */
void num2str(char *s, unsigned int number, unsigned int base)
{
	static char  hexChars[] = "0123456789ABCDEF";

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
}

unsigned int str2num(char *s, unsigned base)
{
	unsigned int u=0, d;
	char ch=*s++;
	while (ch) {
		if ((ch>='0') && (ch<='9')) d=ch-'0';
		else if ((base==16) && (ch>='A') && (ch<='F')) d=ch-'A'+10;
		else if ((base==16) && (ch>='a') && (ch<='f')) d=ch-'a'+10;
		else break;
		u=d+base*u;
		ch=*s++;
	}
	return u;
}


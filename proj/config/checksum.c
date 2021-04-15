/*
 *  gcc -o checksum checksum.c
 *  ./checksum
 *
 *  When starting from flash, the bootloader checks the sum of the 
 *  8 first vectors is 0 to enforce valid code. A magic number is 
 *  pushed for vector 5 at adress 0x14 (unused vector) so that the
 *  sum is 0.
 *
 *  This utility calculates the value of the magic number when the
 *  __IRQ_HANDLER__ flag is defined in the UADEFS symbol of the
 *  makefile, when it is not.
 *
 *  In fact the loader calculates the value as well, and inserts
 *  automatically the number in the vector table ...
 */
#include <stdio.h>
#include <stdint.h>

int main(int argc, char* argv[])
{
	int i;
	uint32_t v1=0xe59ff018;
	uint32_t v2=0xe51ffff0;	/* code for: ldr pc, [pc, #-0xFF0] */
	uint32_t v=0, va=0;
	
	for (i=0; i<7 ; i++) {
		v=v+v1;
	}
	
	for (i=0; i<6 ; i++) {
		va=va+v1;
	}
	va=va+v2;
	
	printf("if __IRQ_HANDLER__ defined\n  magic number = 0x%x\n",0-v);
	printf("else\n  magic number = 0x%x\n", 0-va);
	
	return 0;
}
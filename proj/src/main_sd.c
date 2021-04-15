#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "term_io.h"
#include "diskio.h"
#include "lexer.h"

#define MIN(a,b) ((a)<(b)? (a) : (b))
#define MAX(a,b) ((a)>(b)? (a) : (b))

/****************************************************************
 * explore SD card sectors
 *
 * mini UART shell
 * > di        : initialize disk
 * > ds        : get status information
 * > dd <addr> : dump a sector (try "dd 0" --> Master Boot Record
 *                              contains the partition table from
 *                              offset 0x1BE)
 ****************************************************************/
uint8_t  buf[512];
char text[80];

int main()
{
	uint8_t res=0, b;
	int32_t p2, sector=0;
	int k, ofs;
	
	term_init(_UART0, 80, 24);
	term_clrscr();
	
	while(1) {
		readline(">", text, 60);
		lexer_reset(text);
		next_token();
		switch(token) {
		case T_DISK_INIT:	/* di - Initialize disk */
			term_printf("\r\nDisk Initialize : rc=%d\r\n", (int32_t)disk_initialize());
			break;
		case T_DISK_STATUS:	/* ds - Show disk status */
			if (disk_ioctl(GET_SECTOR_COUNT, &p2) == RES_OK) {
				term_printf("\r\nDrive size : %u sectors\r\n", p2);
			}
			if (disk_ioctl(GET_BLOCK_SIZE, &p2) == RES_OK) {
				term_printf("Block size : %u sectors\r\n", p2);
			}
			if (disk_ioctl(MMC_GET_TYPE, &b) == RES_OK) {
				term_printf("Media type : %u ", b);
				if (b==1) term_printf("(MMC)\r\n");
				else if (b==2) term_printf("(SDCARD v1)\r\n");
				else if (b==4) term_printf("(SDCARD v2)\r\n");
				else term_printf("(Unknown)\r\n");
			}
			if (disk_ioctl(MMC_GET_CSD, buf) == RES_OK) {
				term_printf("CSD        : "); 
				for (k=0;k<16;k++) term_printf("%02x", buf[k]);
				term_printf("\r\n");
			}
			if (disk_ioctl(MMC_GET_CID, buf) == RES_OK) {
				term_printf("CID        : ");
				for (k=0;k<16;k++) term_printf("%02x", buf[k]);
				term_printf("\r\n");
			}
			if (disk_ioctl(MMC_GET_OCR, buf) == RES_OK) {
				term_printf("OCR        : ");
				for (k=0;k<4;k++) term_printf("%02x", buf[k]);
				term_printf("\r\n");
			}
			break;
		case T_DISK_DUMP:	/* dd [<lba>] - Dump sector */
			next_token();
			if (token==T_NUM) sector=token_val;
			res = disk_read(buf, sector, 1);
			if (res) { term_printf("\r\nrc=%d\r\n", (uint32_t)res); break; }
			term_printf("\nDumping sector %u ...\r\n", sector);
			for (ofs = 0; ofs < 512; ofs+=16) {
				term_printf("0x%03x : ", ofs);
				for (k=0;k<16;k++) term_printf("%02x ", buf[ofs+k]);
				term_printf("  ");
				for (k=0;k<16;k++) {
					char c=buf[ofs+k];
					if (c<0x20 || c>0x7F) c='.';
					term_printf("%c", c);
				}
				term_printf("\r\n");
			}
			for (k=0;k<80;k++) term_printf("-");
			term_printf("\r\n");
			sector ++;
			break;
		case T_HELP:
			term_printf(
				"\r\n[Disk controls]\r\n"
				" di             - initialize disk\r\n"
				" dd [<sect>]    - dump a sector\r\n"
				" ds             - show disk status\r\n"
				"\r\n"
			);
			break;
		default:
			term_printf("\r\nUnknown command\r\n");
			break;
		}
	}
	return 0;
}

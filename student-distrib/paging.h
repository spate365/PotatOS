#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"
#include "lib.h"
#include "sched.h"
#define ONEKB   	1024
#define FOURKB  	4096
#define SHIFT_12 	12

/*
the format of the last 9 bits of a page directory entry is
12-13 - page attribute table index
 8 - ignored (G)
 7 - page size (S)
 6 - dirty 
 5 - accessed (A)
 4 - cache disable bit (D)
 3 - write-through (W)
 2 - user/supervisor bit (U)
 1 - read/write flag (R)
 0 - present (P)  
 */

/*Global Variables */
#define PRESENT_WRITE 		0x00000003
#define WRITE_ONLY 			0x00000002
#define PRESENT_WRITE_SIZE 	0x00000087
#define KERNEL				0x00000083
#define VID_MEM_PRESENT		0x00000007

#define FOURKB_HEX     			0x1000
#define VIDMEM_OFFSET 			0xB8000
#define VIDMEM_OFFSET_1 		VIDMEM_OFFSET + FOURKB_HEX
#define VIDMEM_OFFSET_2 		VIDMEM_OFFSET_1 + FOURKB_HEX
#define VIDMEM_OFFSET_3 		VIDMEM_OFFSET_2 + FOURKB_HEX	

#define KERNELMEM			0x400000
#define FOUR_MB				0x400000
#define EIGHT_MB			0x800000
#define VIRTUAL 			0x8000000

/* Function Definitions */
void paging_init();
extern void enable_paging();
void create_process_page(int process_num);
void create_video_page(int term_num);
void remap_vidmem(int term_num);

/* aligning the page table and the page directory */
int page_table[ONEKB] __attribute__((aligned (FOURKB)));
int page_directory[ONEKB] __attribute__((aligned (FOURKB)));
int fish_table[ONEKB] __attribute__((aligned (FOURKB)));


#endif

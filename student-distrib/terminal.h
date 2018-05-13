#ifndef _TERMINAL_H
#define _TERMINAL_H

/*Files Imported*/
#include "lib.h"
#include "i8259.h"
#include "sched.h"
#include "keyboard.h"
/* Global Variables */
#define TOT_TERMS		3

#define TOT_ROWS	25
#define TOT_COLS	80
#define FAILURE		-1
#define BUFF_SIZE 	128
#define	FOUR_KB		4096

#define VIDEO		0xB8000

#define VIDEO_1		VIDEO + FOUR_KB
#define VIDEO_2		VIDEO_1 + FOUR_KB
#define VIDEO_3		VIDEO_2 + FOUR_KB

#define HIST_NUM	10


typedef struct 
{
	int esp;
	int ebp;

	int x_position;
	int y_position;
	int read_index;
	int read_flag;
	int b_index;	// buffer index or total printed
	int is_active;

	int process_flag;
	int curr_process;
	volatile int rtc_flag;
	
	char kb_buffer[BUFF_SIZE];
	int32_t vidmem_cpy;

	uint8_t change_attrib;
} terminal_t;

terminal_t t_terms[TOT_TERMS];		// max of 3 terminals
volatile int curr_running;

/* Used in syscalls and pcb */
int term_active;

/* Function Definitions */
int32_t term_open(const uint8_t* filename);
int32_t term_read(int32_t fd, void* buff, int32_t nbytes);
int32_t term_write(int32_t fd, const void* buff, int32_t nbytes);
int32_t term_close(int32_t fd);
int find_term(int process_num);

extern void change_screen(int32_t new_terminal);
void update_term(int i);

#endif

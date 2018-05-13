#ifndef _SYSTEM_CALLS_H
#define _SYSTEM_CALLS_H

/* Files Imported */
#include "types.h"
#include "lib.h"
#include "x86_desc.h"
#include "filesys.h"
#include "paging.h"
#include "pcb.h"
#include "terminal.h"

/* Global Variables */
#define DEL					0x7F
#define E					0x45
#define L					0x4C
#define F					0x46
#define CHECK_ELF			4
#define LOAD_OFFSET			0x08048000
#define ONE_TWO_EIGHT_MB	0x80000000
#define EIGHT_KB			0x2000

#define HIGH_BOUND			0x8400000
#define LOW_BOUND			0x8000000

#define RTC_TYPE 			0
#define DIR_TYPE 			1
#define FILE_TYPE 			2

#define MAX_PROC			5
#define MIN_VAL				0
#define MAX_VAL				7

/* Function Definition */
extern int32_t halt(uint8_t status);
extern int32_t execute(const uint8_t* command);
extern int32_t open (const uint8_t* filename);
extern int32_t close(int32_t fd);
extern int32_t read (int32_t fd, void* buf, int32_t nbytes);
extern int32_t write (int32_t fd, const void* buf, int32_t nbytes);
extern int32_t getargs(uint8_t* buf, int32_t nbytes);
extern int32_t vidmap(uint8_t** screen_start);

extern int32_t find_empty_fd(pcb_struct* curr_pcb);
void switch_to_user_mode(uint32_t n, pcb_struct* pcb);
int next_process_num();

#endif

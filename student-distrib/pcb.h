#ifndef _PCB_H
#define _PCB_H

/*Files Imported*/
#include "lib.h"

#include "types.h"
#include "filesys.h"
#include "terminal.h"
#include "rtc.h"

/* Global Variables */
#define FD_ARRAY_SIZE   8
#define MAX_PID         6
#define NUM_ARGUMENTS   128
#define STDIN_NUM       0       // stdin = fd 0 ->read-only keyboard input
#define STDOUT_NUM      1       // stdout = fd 1 ->write-only terminal output

#define IN_USE          1
#define NOT_IN_USE      0
#define EIGHT_MB        0x800000
#define EIGHT_KB        0x2000
#define BIT_MASK        0xFFF000
/* File Operations Jump Table */

/*jump table structure inspired by page 42 of student notes*/
typedef struct
{
    int32_t (*open) (const uint8_t* filename);
    int32_t (*read) (int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write) (int32_t fd, const void* buf, int32_t nbytes);
    int32_t (*close) (int32_t fd);


} fileop_jumptable;

/*table 8.2, file systems abstraction */
typedef struct
{
    fileop_jumptable* fileop_ptr;   // pointer to file operations jump table
    uint32_t inode;                 // only valid for data files, should be 0 for directories and RTC device file
    uint32_t file_position;         // keeps track of where the user is currently reading from in the file; should be updated by read syscalls
    uint32_t flags;                 // should mark this file descriptor as "in-use"

} fd_table;

typedef struct
{
	uint32_t curr_esp;
    uint32_t curr_ebp;

    fd_table fd_array[FD_ARRAY_SIZE];
    uint32_t process_id;
    uint8_t arguments[NUM_ARGUMENTS];
    int32_t exec_return;
    //struct pcb_struct* parent_pcb;

} pcb_struct;

extern fileop_jumptable rtc_jumptable;
extern fileop_jumptable term_jumptable;
extern fileop_jumptable dir_jumptable;
extern fileop_jumptable file_jumptable;

/* Function Definition */
int32_t create_pcb(pcb_struct *pcb, int process_count, uint8_t* file);
pcb_struct* get_curr_pcb();

#endif

#ifndef _FILESYS_H
#define _FILESYS_H

/* Files Imported */
#include "types.h"
#include "lib.h"
#include "multiboot.h"
#include "rtc.h"
#include "terminal.h"
#include "pcb.h"

/* Global Variables */
#define END_OF_DIR      64
#define FILE_NAME_B     32
#define BLOCK           4
#define BOOT_PADDING    52
#define FILE_DATA       4096
#define BOOT_BLOCK_LEN  128
#define SET             1
#define RESET           0

/* Structs Definition */
typedef struct
{
    uint32_t length;
    uint32_t data_block[1023];
}inode_t;

typedef struct 
{
    uint8_t filename[FILE_NAME_B];
    uint32_t type;
    uint32_t node;
    uint8_t padding[24];
}dentry_t;

typedef struct
{
    uint32_t num_dir;
    uint32_t num_inodes;
    uint32_t num_dblocks;
    uint8_t padding[BOOT_PADDING];
    dentry_t *boot_dentry;
}boot_t;

/* Global Varibles */
uint32_t global_index;
uint8_t* start_loc;


/* Global variables initialization to remove warnings */
boot_t* boot_block;
inode_t* block_chain;
dentry_t* file_locator;
dentry_t name;

/* Function Definitions */
extern int32_t get_inode(int index);
extern void init_filesys(uint32_t mod);

extern int32_t file_open(const uint8_t* filename);
extern int32_t file_close(int32_t fd);
extern int32_t file_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t file_write(int32_t fd, const void* buf, int32_t nbytes);

extern int32_t dir_open(const uint8_t* filename);
extern int32_t dir_close(int32_t fd);
extern int32_t dir_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes);

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index(const uint32_t index, dentry_t* dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

#endif

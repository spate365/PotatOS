#ifndef _RTC_H
#define _RTC_H

/* Files Imported */
#include "types.h"
#include "lib.h"
#include "i8259.h"
#include "sched.h"
#include "pcb.h"
#include "terminal.h"
#include "keyboard.h"
/* Global Variables */
#define	REG_A		0x0A
#define	REG_B		0x0B
#define	REG_C		0x0C
#define DISABLE_NMI 0x80
#define INDEX		0x70
#define	RW_CMOS		0x71
#define SIX_MASK	0x40
#define RTC_IRQ		0x08

#define max_freq			1024
#define min_freq			2
#define freq_array_size		10
#define four_byte			4


/* Function Definitions */
extern void rtc_init();
extern void rtc_handler();
extern int32_t rtc_open(const uint8_t* filename);
extern int32_t rtc_close(int32_t fd);
extern int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);

#endif

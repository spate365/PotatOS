#ifndef _KEYBOARD_H
#define _KEYBOARD_H

/* Files Imported */
#include "types.h"
#include "lib.h"
#include "i8259.h"
#include "terminal.h"
#include "sched.h"

/* Global Variables */
#define	KB_ADDR			0x60
#define KB_IRQ			0x01

#define RSHIFT_DOWN 	54
#define LSHIFT_DOWN 	42
#define RSHIFT_RELEASE 	182
#define LSHIFT_RELEASE 	170

#define CAPS_KEYCODE 	58

#define RCTRL_DOWN 		0xE0
#define LCTRL_DOWN 		29
// #define RCTRL_RELEASE 	157
#define CTRL_RELEASE 	157

#define ALT_DOWN		0x38
#define ALT_RELEASE		0xB8

#define UP				72
#define DOWN			80
#define LEFT			75
#define RIGHT			77
#define TAB				0x0F
		

/* Keys */
#define UNKNOWN 		0
#define BUFF_SIZE 		128
#define ENTER 			28
#define BACKSPACE		14


#define MAX_SIZE		128

#define Fn1				59
#define Fn2				60
#define Fn3				61


/* Function Definitions */
extern void kb_init();
extern void kb_handler();

extern char fill_keyboard_buffer(int keycode);

void scroll_up2(int term_num);
void putc2(uint8_t c, int term_num);


#endif

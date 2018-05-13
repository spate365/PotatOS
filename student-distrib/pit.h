#ifndef _PIT_H
#define _PIT_H

/* Files Imported */
#include "types.h"
#include "lib.h"
#include "i8259.h"
#include "terminal.h"
#include "keyboard.h"
#define PIT_IRQ 	0

#define MODE_2		0x34
#define CHANNEL_0 	0x40
#define COMMAND		0x43

// (frequency = 1193182 / reload_value Hz) 
#define HIGH_BITS 	(1193182 / 100) >> 8
#define LOW_BITS	(1193182 / 100) && 0xFF

extern void pit_init();
extern void pit_handler();

#endif

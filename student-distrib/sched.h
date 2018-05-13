#ifndef _SCHED_H
#define _SCHED_H

/* Files Imported */
#include "types.h"
#include "lib.h"
#include "i8259.h"
#include "terminal.h"
#include "paging.h"
#include "keyboard.h"
void sched_task();
void sched_init();

volatile int scheduled_term;
volatile int total_active;

#endif


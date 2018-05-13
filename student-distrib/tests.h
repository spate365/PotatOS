#ifndef TESTS_H
#define TESTS_H

/* Files Imported */
#include "rtc.h"
#include "keyboard.h"
#include "idt.h"
#include "paging.h"
#include "filesys.h"
#include "terminal.h"
#include "x86_desc.h"
#include "lib.h"
#include "system_calls.h"

/* Global Variable */
#define PASS 		1
#define FAIL 		0

// test launcher
void launch_tests();

#endif /* TESTS_H */

#ifndef _IDT_H
#define _IDT_H

/* Files Imported */
#include "x86_desc.h"
#include "lib.h"
#include "types.h"
#include "rtc.h"
#include "keyboard.h"
#include "system_calls.h"

/* Global Variables */
#define SIZE_IDT	20
#define SKIP		15

#define SYSINDEX	0x80
#define PITINDEX	0x20
#define KBINDEX		0x21
#define RTCINDEX	0x28

#define DISABLE			0x0
#define	ENABLE			0x1
#define ENABLE_SYS		0x3

#define TRUE		1
#define HALT_EXCEPTION	256

/* Function Definitions */
extern void DE();
extern void DB();
extern void NMI();
extern void BP();
extern void OF();
extern void BR();
extern void UD();
extern void NM();
extern void DF();
extern void CSO();
extern void TS();
extern void NP();
extern void SS();
extern void GP();
extern void PF();
extern void MF();
extern void AC();
extern void MC();
extern void XF();
extern void SYS(); 
extern void RES();
extern void unknown();
extern int init_func();

extern void handle_kb();
extern void handle_rtc();
extern void handle_pit();
extern void call_system();

#endif

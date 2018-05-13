#include "pit.h"


/* Inspired by: OS_Dev/Programmable_Interval_Timer#Channel_0
 * Inspired by: WikiBooks/wiki/X86_Assembly/Programmable_Interval_Timer
 * void pit_init()
 *
 * Inputs: none
 * Outputs: none
 * Side Effects: Initalizes a PIT so it can be used
 */
void pit_init()
{
	sched_init();

	enable_irq(PIT_IRQ);

	//This enables mode 2 on the PIT (rate generator)
	outb(MODE_2, COMMAND);

	//Send high and low bits
	outb(LOW_BITS, CHANNEL_0);
	outb(HIGH_BITS, CHANNEL_0);
}

/* void pit_handler()
 *
 * Inputs: none
 * Outputs: none
 * Side Effects: Handles a pit interrupt and schedules a task
 */
void pit_handler()
{	
	disable_irq(KB_IRQ);
	send_eoi(PIT_IRQ);
	sched_task();
}


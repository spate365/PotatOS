/* Files Imported */
#include "sched.h"
#include "system_calls.h"

/* void sched_init()
 *
 * Inputs: none
 * Outputs: none
 * Side Effects: Initializes a scheduler
 */
void sched_init()
{
	total_active = 0;
	scheduled_term = 2;
}

/* void sched_task()
 *
 * Inputs: none
 * Outputs: none
 * Side Effects: Schedules a task to be run for 10 ms
 */
void sched_task()
{
	cli();

	if(total_active < TOT_TERMS)
	{
		if(total_active != 0)
		{
			asm volatile(
					
				"movl %%esp, %0;"
				"movl %%ebp, %1;"
				:
				: "m" (t_terms[curr_running].esp), "m" (t_terms[curr_running].ebp)
			);
		}

		change_screen(total_active);
		curr_running = total_active;
		total_active++;

		sti();
		execute((uint8_t*)"shell");
	}
	else if(total_active == TOT_TERMS)
	{	
		asm volatile(
					
			"movl %%esp, %0;"
			"movl %%ebp, %1;"
			:
			: "m" (t_terms[TOT_TERMS - 1].esp), "m" (t_terms[TOT_TERMS - 1].ebp)
		);

		change_screen(0);
		curr_running = 0;
		total_active++;

	}
	else
	{

		// Save EBP and ESP
		asm volatile(
					
					"movl %%esp, %0;"
					"movl %%ebp, %1;"
					:
					: "m" (t_terms[scheduled_term].esp), "m" (t_terms[scheduled_term].ebp)
		);

		scheduled_term++;
		scheduled_term %= TOT_TERMS;

		if(scheduled_term == curr_running)
		{
			remap_vidmem(-1);
			enable_irq(KB_IRQ);
		}
		else
		{
			disable_irq(KB_IRQ);
			remap_vidmem(scheduled_term);
		}

		create_process_page(t_terms[scheduled_term].curr_process);
		tss.esp0 = (EIGHT_MB - ((t_terms[scheduled_term].curr_process) * EIGHT_KB));

		//Restore EBP and ESP
		asm volatile(
					
					"movl %0, %%esp;"
					"movl %1, %%ebp;"
					:
					: "r" (t_terms[scheduled_term].esp), "r" (t_terms[scheduled_term].ebp)
		);
	}

	sti();
}


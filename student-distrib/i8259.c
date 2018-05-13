/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

/* Files Imported */
#include "i8259.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = LOW_ACTIVE; /* IRQs 0-7  */
uint8_t slave_mask = LOW_ACTIVE;  /* IRQs 8-15 */


/* void i8259_init()
 *
 * initializes the 8259 PIC to work according to the Intel documentation provided
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void i8259_init(void) 
{
	int i;
	for(i = 0; i <= HALF_SIZE; ++i)
		disable_irq(i);
	
	
	outb(LOW_ACTIVE, MASTER_PORT2);
	outb(LOW_ACTIVE, SLAVE_PORT2);
	
	//master
	outb(ICW1, MASTER_8259_PORT);
	outb(ICW1, SLAVE_8259_PORT);
	
	outb(ICW2_MASTER, MASTER_PORT2);
	outb(ICW2_SLAVE, SLAVE_PORT2);
	
	outb(ICW3_MASTER, MASTER_PORT2);
	outb(ICW3_SLAVE, SLAVE_PORT2);
	
	outb(ICW4, MASTER_PORT2);
	outb(ICW4, SLAVE_PORT2);

	master_mask = LOW_ACTIVE; 
	slave_mask = LOW_ACTIVE;

	enable_irq(SLAVE_LINE); //slave line sits at 2	
}

/* void enable_irq(uint32_t irq_num)
 *
 * Enable (unmask) the specified IRQ
 * Inputs: irq_num, the specified IRQ
 * Outputs: None
 * Side Effects: None
 */
void enable_irq(uint32_t irq_num) 
{
	uint32_t mask = LAST_BIT;

	//master
	if ( (irq_num >= 0) && (irq_num < FULL_SIZE) )
	{
		if(irq_num < HALF_SIZE)
		{
			mask <<= irq_num;
			master_mask &= ~mask;
			
			outb(master_mask, MASTER_PORT2);
		}
	//slave
		else 
		{
			mask <<= (irq_num - HALF_SIZE);
			slave_mask &= ~mask;
			
			outb(slave_mask, SLAVE_PORT2);
		}
	}
}

/* void disable_irq(uint32_t irq_num)
 *
 * disable (mask) the specified IRQ
 * Inputs: irq_num, the specified IRQ
 * Outputs: None
 * Side Effects: None
 */
void disable_irq(uint32_t irq_num) 
{
	uint32_t mask = LAST_BIT;
	
	//master
	if ( (irq_num >= 0) && (irq_num < FULL_SIZE) )
	{
		if(irq_num < HALF_SIZE)
		{
			mask <<= irq_num;
			master_mask |= mask;
			
			outb(master_mask, MASTER_PORT2);
		}
		//slave
		else
		{
			mask <<= (irq_num - HALF_SIZE);
			slave_mask |= mask;
			
			outb(slave_mask, SLAVE_PORT2);
		}
	}
}


/* void send_eoi(uint32_t irq_num)
 *
 * Inspired by: OSDev .../PIC
 * Send end-of-interrupt signal for the specified IRQ
 * Inputs: irq_num, the specified IRQ
 * Outputs: None
 * Side Effects: None
 */
void send_eoi(uint32_t irq_num) 
{

	if ( (irq_num >= 0) && (irq_num < FULL_SIZE) )
	{
		if(irq_num >= HALF_SIZE)
		{
			outb((irq_num - HALF_SIZE) | EOI, SLAVE_8259_PORT);
			outb(SLAVE_LINE | EOI, MASTER_8259_PORT);
		}
		else
			outb(irq_num | EOI, MASTER_8259_PORT);
	}
	
}

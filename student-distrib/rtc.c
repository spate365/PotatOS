/* RTC Handler */

/* Files Imported */
#include "rtc.h"

/* Global variables initialization to remove warnings */

/* void rtc_init()
 *
 * enables the rtc to work with the pic
 *
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void rtc_init()
{
	char queue;
	
	cli();						//prevent interrupts
	
	//enables periodic interrupts
	/* Inspired by OSDev .../RTC */
	outb(REG_B + DISABLE_NMI, INDEX);
	queue = inb(RW_CMOS);		
	outb(REG_B + DISABLE_NMI, INDEX);
	
	queue |= SIX_MASK;
	outb(queue, RW_CMOS);		//Turns on 6th bit of reg (periodic interrupts)
		
	enable_irq(RTC_IRQ);		//enables interrupts for RTC on PIC

	rtc_open(NULL);

	sti();						//enables interrupts
}

/* void rtc_handler()
 *
 * handler calls test interrupts when an rtc interrupt is sent by the pic
 *
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void rtc_handler()
{
	send_eoi(KB_IRQ);

	cli();						//prevent interrupts

	t_terms[0].rtc_flag = 0;
	t_terms[1].rtc_flag = 0;
	t_terms[2].rtc_flag = 0;
	
	/* Inspired by OSDev .../RTC */
	outb(REG_C, INDEX);			
	inb(RW_CMOS);				
	send_eoi(RTC_IRQ);

	sti();						//enables interrupts
}

/* int32_t rtc_open(const uint8_t* filename)
 *
 * enables the rtc and sets default frequency to 2
 *
 * Inputs: filename = nothing
 * Outputs: returns 0 for success
 * Side Effects: sets default frequency of rtc to 2
 */
int32_t rtc_open(const uint8_t* filename)
{
	int frequency = min_freq;

	rtc_write(0, &frequency, four_byte);
	return 0;
}

/* int32_t rtc_close(int32_t fd)
 *
 * do nothing
 *
 * Inputs: fd = nothing
 * Outputs: returns 0 for success
 * Side Effects: None
 */
int32_t rtc_close(int32_t fd)
{
	return 0;
}

/* int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes)
 *
 * spins until a rtc interrupt is recieved
 *
 * Inputs: fd = nothing
 *		 : buf = nothing
 *		 : nbytes = nothing
 * Outputs: returns 0 when rtc interrupt is recieved 
 * Side Effects: None
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes)
{
	t_terms[scheduled_term].rtc_flag = 1;
	send_eoi(KB_IRQ);
	while(1)
	{
		//stop if rtc interrupt is recieved
		if(!t_terms[scheduled_term].rtc_flag)
			return 0;
	}
	return 0;
}

/* int32_t rtc_write(int32_t fd, void* buf, int32_t nbytes)
 *
 * changes the frequency of the rtc
 *
 * Inputs: fd = nothing
 *		   buf = the frequency to change to
 *		   nbytes = bytes to write
 * Outputs: returns number of bytes written or -1 for failure
 * Side Effects: changes the frequency of the RTC
 */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes)
{
	//error cases
	if(nbytes != four_byte || buf == NULL)	//4 bytes input
		return -1;

	int* f = (int*)buf;

	if(*f < min_freq || *f > max_freq)
	 	return -1;

	int32_t freq[freq_array_size];
	int i, rate;
	char queue;

	//calculates the rate
	for(i = 0; i < freq_array_size; i++)
		freq[i] = max_freq >> i;

	for(i = 0; i < freq_array_size; i++)
	{
		if(*f == freq[i])
		{
			//offset by 6 b/c 1024 is rate 6
			rate = i + 6;
			break;
		}
		else 
		{
			//reached the end of the array, not a power of 2
			if(i == 9 && *f != freq[i]) 
				return -1;
		}
	}

	//cli();

	/* Inspired by OSDev .../RTC */
	outb(REG_A + DISABLE_NMI, INDEX);
	queue = inb(RW_CMOS);	
	outb(REG_A + DISABLE_NMI, INDEX);
	outb(rate | (queue & 0xF0), RW_CMOS);

	//sti();

	return nbytes;
}

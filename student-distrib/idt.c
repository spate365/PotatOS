/* Initialize the Interrupt Descriptor Table */

/* Files Imported */
#include "idt.h"

/* void DE()
 *
 * handles interrupt for divide error as shown in IDT documentation, prints error
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void DE()
{
	printf("Divide Error\n");
	halt((uint8_t)HALT_EXCEPTION);
}

/* void DB()
 *
 * handles interrupt which is Reserved as shown in IDT documentation, prints error
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void DB()
{
	printf("RESERVED DB\n");
	halt((uint8_t)HALT_EXCEPTION);
}
/* void NMI()
 *
 * handles NMI interrupt as shown in IDT documentation, prints error
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void NMI()
{
	printf("NMI Interrupt\n");
	halt((uint8_t)HALT_EXCEPTION);
}
/* void BP()
 *
 * handles BP interrupt as shown in IDT documentation, prints error
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void BP()
{
	printf("Breakpoint\n");
	halt((uint8_t)HALT_EXCEPTION);
}
/* void OF()
 *
 * handles OF interrupt as shown in IDT documentation, prints error
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void OF()
{
	printf("Overflow\n");
	halt((uint8_t)HALT_EXCEPTION);
}
/* void BR()
 *
 * handles BR interrupt as shown in IDT documentation, prints error
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void BR()
{
	printf("BOUND Range Exceeded\n");
	halt((uint8_t)HALT_EXCEPTION);
}
/* void UD()
 *
 * handles UD interrupt as shown in IDT documentation, prints error
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void UD() 
{
	printf("Invalid Opcode\n");
	halt((uint8_t)HALT_EXCEPTION);
}
/* void NM()
 *
 * handles NM interrupt as shown in IDT documentation, prints error
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void NM() 
{
	printf("Device Not Available\n");
	halt((uint8_t)HALT_EXCEPTION);
}
/* void DF()
 *
 * handles DF interrupt as shown in IDT documentation, prints error
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void DF() 
{
	printf("Double Fault\n");
	halt((uint8_t)HALT_EXCEPTION);
}
/* void CSO()
 *
 * handles CSO interrupt as shown in IDT documentation, prints error
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void CSO() 
{
	printf("Coprocessor Segment Overrun\n");
	halt((uint8_t)HALT_EXCEPTION);
}
/* void TS()
 *
 * handles TS interrupt as shown in IDT documentation, prints error
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void TS() 
{
	printf("Invalid TSS\n");
	halt((uint8_t)HALT_EXCEPTION);
}
/* void NP()
 *
 * handles NP interrupt as shown in IDT documentation, prints error
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void NP() 
{
	printf("Segment Not Present\n");
	halt((uint8_t)HALT_EXCEPTION);
}
/* void SS()
 *
 * handles SS interrupt as shown in IDT documentation, prints error
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void SS() 
{
	printf("Stack Segment Fault\n");
	halt((uint8_t)HALT_EXCEPTION);
}
/* void GP()
 *
 * handles GP interrupt as shown in IDT documentation, prints error
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void GP() 
{
	printf("General Protection\n");
	halt((uint8_t)HALT_EXCEPTION);
}
/* void PF()
 *
 * handles PF interrupt as shown in IDT documentation, prints error
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void PF() 
{
	printf("Page Fault\n");
	halt((uint8_t)HALT_EXCEPTION);
}
/* void MF()
 *
 * handles MF interrupt as shown in IDT documentation, prints error
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void MF() 
{
	printf("x87 FPU Floating Point Error\n");
	halt((uint8_t)HALT_EXCEPTION);
}
/* void AC()
 *
 * handles AC interrupt as shown in IDT documentation, prints error
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void AC() 
{
	printf("Alignment Check\n");
	halt((uint8_t)HALT_EXCEPTION);
}
/* void MC()
 *
 * handles MC interrupt as shown in IDT documentation, prints error
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void MC() 
{
	printf("Machine Check\n");
	halt((uint8_t)HALT_EXCEPTION);
}
/* void XF()
 *
 * handles XF interrupt as shown in IDT documentation, prints error
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void XF() 
{
	printf("SIMD Floating Point Exception\n");
	halt((uint8_t)HALT_EXCEPTION);
}
/* void SYS()
 *
 * handles SYS interrupt as shown in IDT documentation, prints error
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void SYS() 
{
	printf("System Call Error\n");
	halt((uint8_t)HALT_EXCEPTION);
}
/* void RES()
 *
 * handles RES interrupt as shown in IDT documentation, prints error
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void RES() 
{
	printf("RESERVED INTEL\n");
	halt((uint8_t)HALT_EXCEPTION);
}
/* void unknown()
 *
 * handles unknown errors, if not defined in the IDT
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void unknown() 
{
	printf("Unknown Error\n");
	halt((uint8_t)HALT_EXCEPTION);
}

/* int init_func()
 *
 * Sets the indexes for the various interrupts that are supposed to be handled by the idt
 * also sets the pertinent bits/flags for 0-256 (eg, size, present, dpl etc.)
 * Inputs: None
 * Outputs: returns 0 on success
 * Side Effects: None
 */
int init_func()
{
	asm("lidt idt_desc_ptr;");
	int i;
	
	//Initializes each bit of idt
	for( i = 0; i < NUM_VEC; ++i)
	{
		idt[i].seg_selector = KERNEL_CS;
		idt[i].reserved4 = DISABLE;
		idt[i].reserved3 = ENABLE;
		idt[i].reserved2 = ENABLE;
		idt[i].reserved1 = ENABLE;
		idt[i].size = ENABLE;
		idt[i].reserved0 = DISABLE;
		idt[i].dpl = DISABLE;
		idt[i].present = DISABLE;
		
		if ((i < SIZE_IDT) || (i == KBINDEX) || (i == RTCINDEX) || (i==PITINDEX) || (i == SYSINDEX))
			idt[i].present = ENABLE;
		
		if(i == SYSINDEX)
			idt[i].dpl = ENABLE_SYS;
		
		if ((i >= SIZE_IDT) && (i != KBINDEX) && (i != RTCINDEX) && (i != PITINDEX) && (i != SYSINDEX))
			SET_IDT_ENTRY(idt[i], unknown);

	}
	
	/* setting individual index with the exception */
	SET_IDT_ENTRY(idt[0], DE);
	SET_IDT_ENTRY(idt[1], DB);
	SET_IDT_ENTRY(idt[2], NMI);
	SET_IDT_ENTRY(idt[3], BP);
	SET_IDT_ENTRY(idt[4], OF);
	SET_IDT_ENTRY(idt[5], BR);
	SET_IDT_ENTRY(idt[6], UD);
	SET_IDT_ENTRY(idt[7], NM);
	SET_IDT_ENTRY(idt[8], DF);
	SET_IDT_ENTRY(idt[9], CSO);
	SET_IDT_ENTRY(idt[10], TS);
	SET_IDT_ENTRY(idt[11], NP);
	SET_IDT_ENTRY(idt[12], SS);
	SET_IDT_ENTRY(idt[13], GP);
	SET_IDT_ENTRY(idt[14], PF);
	SET_IDT_ENTRY(idt[15], RES);
	SET_IDT_ENTRY(idt[16], MF);
	SET_IDT_ENTRY(idt[17], AC);
	SET_IDT_ENTRY(idt[18], MC);
	SET_IDT_ENTRY(idt[19], XF);
	
	SET_IDT_ENTRY(idt[KBINDEX], handle_kb);
	SET_IDT_ENTRY(idt[RTCINDEX], handle_rtc);
	SET_IDT_ENTRY(idt[PITINDEX], handle_pit);
	SET_IDT_ENTRY(idt[SYSINDEX], call_system);

	
	return 0;
}

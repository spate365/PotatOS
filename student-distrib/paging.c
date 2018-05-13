#include "paging.h"

/* inspired by OSDev .../Setting_Up_Paging*/

/*
from my understanding of the doc:
0-4MB has to be full of 4kb pages
4-8MB has to have one large table
*/


/* paging_init()
 * 
 * initializes page tables and page directories, maps the video memory, maps the kernel
 *
 * Inputs: None
 * Outputs: None
 * Side Effects: calls enable paging which changes the values in the control registers cr0, cr3 and cr4
 * Files: paging.h
 */
void paging_init()
{
    int i;
    for (i = 0; i < ONEKB; i++)
    {
        /*initialize all the entries in the page directories to not present */
        page_directory[i] = WRITE_ONLY;
        page_table[i] = WRITE_ONLY;
        fish_table[i] = WRITE_ONLY;
    }

    /*next step is initializing the first 4MB at 0 */
	int pd0;
	pd0 = ((int)page_table | PRESENT_WRITE);
	page_directory[0] = pd0;

    /*set kernel memory to begin at 4MB */
    int pd_kern;
    pd_kern = KERNELMEM | KERNEL;
    page_directory[1] = pd_kern;
    
    /* map the video memory */
    page_table[VIDMEM_OFFSET >> SHIFT_12] =  VIDMEM_OFFSET | PRESENT_WRITE;
    page_table[(VIDMEM_OFFSET_1) >> SHIFT_12] =  (VIDMEM_OFFSET_1) | PRESENT_WRITE;
    page_table[(VIDMEM_OFFSET_2) >> SHIFT_12] =  (VIDMEM_OFFSET_2) | PRESENT_WRITE;
    page_table[(VIDMEM_OFFSET_3) >> SHIFT_12] =  (VIDMEM_OFFSET_3) | PRESENT_WRITE;

    enable_paging();
}

/* extern void enable_paging()
 * 
 * setting relevant flags in the control registers, ends with putting correct address in cr0 to enable paging
 *
 * Inputs: None
 * Outputs: None
 * Side Effects: changes control register values, cr0, cr3 and cr4. also uses/changes eax
 * Files: paging.h
 */
extern void enable_paging()
{

    /* this function is essentially assembly code that sets the 3 control registers
    that must be set for the paging function, cr0, cr3 and cr4 */
    
    /*
    eax is basically used as a register for mathematical and logical functions.
    
    first, the initial index of the page directory is put into cr3. In 
    this case, that value is 0

    then the value in eax is cleared and the current value in cr4 is 
    loaded into eax. this is ORed with the mask 0000 0010 to enable the PSE
    bit 

    eax is once again cleared and loaded with cr0. we need to set the paging
    and protection bits of cr0, so it's ORed with 8000 0001
    */
    
    //0 has to be last
    asm volatile (
                "xorl %%eax, %%eax;"
                "movl %0, %%eax;"
                "movl %%eax, %%cr3;"
                "xorl %%eax, %%eax;"
                "movl %%cr4, %%eax;"
                "orl $0x00000010, %%eax;"
                "movl %%eax, %%cr4;"
                "xorl %%eax, %%eax;"
                "movl %%cr0, %%eax;"
                "orl $0x80000001, %%eax;"
                "movl %%eax, %%cr0;"
                :
                :"r"(page_directory)
                :"%eax"
                );


 
}

/* create_process_page(int process_num)
 * 
 * creates a page for the current process to load data into
 *
 * Inputs: process_num - the number of the current process
 * Outputs: None
 * Side Effects: creates a page in memory for the current process
 */
void create_process_page(int process_num)
{
    // physical memory location is 8MB + (process number * 4MB)
    int phys_mem_loc = EIGHT_MB + (process_num * FOUR_MB);

    // User level programs are loaded to 128MB in virtual
    page_directory[32] = phys_mem_loc | PRESENT_WRITE_SIZE;

    // Flush TLB  (thanks to OSDev)
    asm volatile (

                "movl %%cr3, %%eax;"
                "movl %%eax, %%cr3;"
                :
                :
                :"%eax"
                );
}

/* create_process_page(int term_num)
 * 
 * creates a page for the video memory in user space
 *
 * Inputs: term_num - the terminal number to create a process page for
 * Outputs: None
 * Side Effects: creates a page in user space memory for the video memory
 */
void create_video_page(int term_num)
{
    // make the entire vidmem write enabled
    page_directory[33 + (term_num + 1)] = (int) page_table | VID_MEM_PRESENT;
    page_table[0] =  (VIDMEM_OFFSET + (FOURKB_HEX * (term_num + 1))) | VID_MEM_PRESENT;

    // Flush TLB  (thanks to OSDev)
    asm volatile (

                "movl %%cr3, %%eax;"
                "movl %%eax, %%cr3;"
                :
                :
                :"%eax"
                );
}

/* create_process_page(int term_num)
 * 
 * remaps a page for the video memory in user space
 *
 * Inputs: term_num - the terminal number to remap a process page for
 * Outputs: None
 * Side Effects: remaps a page in user space memory for the video memory
 */
void remap_vidmem(int term_num)
{
    // make the entire vidmem write enabled
    page_directory[33 + (term_num + 1)] =  (int) page_table | VID_MEM_PRESENT;
    page_table[0] = (VIDMEM_OFFSET + (FOURKB_HEX * (term_num + 1)))  | VID_MEM_PRESENT;

    // Flush TLB  (thanks to OSDev)
    asm volatile (

                "movl %%cr3, %%eax;"
                "movl %%eax, %%cr3;"
                :
                :
                :"%eax"
                );
}

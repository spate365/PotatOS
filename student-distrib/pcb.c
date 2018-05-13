/* PCB Function File */

/*Files Imported*/
#include "pcb.h" 

/*jump tables*/
fileop_jumptable rtc_jumptable = {rtc_open, rtc_read, rtc_write, rtc_close};
fileop_jumptable term_jumptable = {term_open,  term_read, term_write, term_close};
fileop_jumptable dir_jumptable = {dir_open, dir_read, dir_write, dir_close};
fileop_jumptable file_jumptable  = {file_open, file_read, file_write, file_close};

/* int32_t create_pcb(pcb_struct *pcb, int process_count, uint8_t* file)
 *
 * creates a pcb structure
 *
 * Inputs: pcb - the pointer to where to create a pcb struct
         : process_cout - the current process count
         : file - the arguments of the command
 * Outputs: -1 if failure
 *        : 0 if success
 * Side Effects: creates a pcb struct
 */
int32_t create_pcb(pcb_struct* pcb, int process_count, uint8_t* file)
{
    //first thing to do is check if pointer is null, return error
    if (pcb == NULL)
        return -1;
   
    int i;

    fd_table fd_initial;
    fd_initial.inode = 0;
    fd_initial.file_position = 0;
    fd_initial.flags = IN_USE;
    fd_initial.fileop_ptr = &term_jumptable;
    
    //clear the fd array (except 0 and 1 because they're stdin and stdout)
    for (i = 0; i < FD_ARRAY_SIZE; ++i)
    {
        //set the first two fd entries to stdin and stdout
        if ((i == STDIN_NUM) || (i == STDOUT_NUM))
            pcb->fd_array[i] = fd_initial;
        else
        {
            fd_table unused_fd_table;
            unused_fd_table.inode = 0;
            unused_fd_table.file_position = 0;
            unused_fd_table.fileop_ptr = NULL;
            unused_fd_table.flags = NOT_IN_USE;
            pcb->fd_array[i] = unused_fd_table;
        }

    }

    pcb->process_id = process_count;

    for(i = 0; file[i] != '\0'; ++i)
        pcb->arguments[i] = file[i];

    return 0;
}

/* pcb_struct* get_curr_pcb()
 *
 * gets the pointer of the pcb structure
 *
 * Inputs:
 * Outputs: returns the current pcb
 * Side Effects: none
 */
pcb_struct* get_curr_pcb()
{

    int32_t curr_pcb;

    asm volatile(
                
                "movl %%esp, %0;"
                :
                : "m" (curr_pcb)
    );

    curr_pcb = (curr_pcb & BIT_MASK) - FOURKB_HEX;
    
    return (pcb_struct*)curr_pcb;

}

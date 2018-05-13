/* System Calls */

/* Files Imported */
#include "system_calls.h"

uint8_t file[128];
int pcb_arr[6] = {-1,-1,-1,-1,-1,-1};

/* int32_t execute(const uint8_t* command)
 *
 * loads and executes a new program from an executable
 *
 * Inputs: command - a command (executable) and its arguments
 * Outputs: returns -1 if program cannot be run
 *		  : returns 0-255 if program executes a halt system call (given by halt)
 *		  : returns 256 if program dies by exception
 * Side Effects: runs an executable
 */
int32_t execute(const uint8_t* command)
{
	int i, j;
	int length;
	int32_t pcb_loc;
	int32_t check_validity;
	int32_t command_length = strlen((int8_t*)command);
	uint8_t cmd[command_length + 1];
	char check_buf[CHECK_ELF];
	dentry_t check_file;

	// forcing a NULL characters to be at the end of the array
	cmd[command_length] = '\0';
	file[command_length] = '\0';

	/* Parse through command */
	for(i = 0; i < command_length; ++i)
	{
		cmd[i] = 0;
		file[i] = 0;
	}

	for(i = 0; (command[i] != ' ') && (command[i] != '\0'); ++i)
		cmd[i] = command[i];

	if(command[i] == ' ')
	{	
		while(command[i] == ' ' && command[i] != '\0')
			i++;

		for(j = 0; command[i] != '\0'; ++i, ++j)
		{
			file[j] = command[i];
		}
	}
	
	/* Check file validity  */
	check_validity = read_dentry_by_name(&cmd[0], &check_file);

	if(check_validity == -1)
		return -1;

	check_validity = read_data(check_file.node, 0, (uint8_t*)check_buf, CHECK_ELF);

	if(check_validity != CHECK_ELF)
		return -1;
	
	if((check_buf[0] != DEL) && (check_buf[1] != E) && (check_buf[2] != L) && (check_buf[3] != F))
		return -1;

	/* check if max number of programs are being run */
	if(next_process_num() == -1)
	{
		printf("Max Process Reached... Request cannot be completed.\n");
		return 0;
	}

	/* execute the program */
	int curr_process = next_process_num();

	// link new process with its parent
	if(t_terms[curr_running].process_flag == 1)
		pcb_arr[curr_process] = t_terms[curr_running].curr_process;
	else	// first time running
	{
		pcb_arr[curr_process] = curr_process;
		t_terms[curr_running].process_flag = 1;
	}

	// update the current process
	t_terms[curr_running].curr_process = curr_process;

	//create a page for process in physical mem
	create_process_page(curr_process);

	// Load file into memory
	length = block_chain[check_file.node].length;
	read_data(check_file.node, 0, (uint8_t*)LOAD_OFFSET, length);

	// Start PCB
	pcb_loc = EIGHT_MB - ((curr_process + 1) * EIGHT_KB);
	pcb_struct* pcb = (pcb_struct*)pcb_loc;

	check_validity = create_pcb(pcb, curr_process, file); 
	
	if(check_validity == -1)
		return -1;

	// Context Switch
	tss.esp0 = (EIGHT_MB - ((curr_process) * EIGHT_KB));
	uint32_t* n = (uint32_t*)(LOAD_OFFSET + 24);
	
	//store the address of the pcb into some register, maybe ecx
	switch_to_user_mode(*n, pcb);

	return pcb->exec_return;
}

/* int32_t halt(const uint8_t status)
 *
 * halts an executable program and returns specified value to parent process
 *
 * Inputs: status - status to return
 * Outputs: returns 32bit bl
 * Side Effects: halts an executable
 */
int32_t halt(uint8_t status)
{
	int i;
	int curr_process;
	int term_with_process;

	//reset filesys info
    global_index = 0;

    /* restore parent data */
    //get the current PCB
   	pcb_struct * curr_pcb = get_curr_pcb();
   	curr_process = curr_pcb->process_id;
   	term_with_process = find_term(curr_process);

    //from the current pcb we need to get the esp and ebp
    uint32_t ret_esp = curr_pcb->curr_esp;
    uint32_t ret_ebp = curr_pcb->curr_ebp;
	curr_pcb->exec_return = (int32_t)status;

	// Update to parent process
    if(pcb_arr[curr_process] != curr_process)
    {
    	// update current process to the parent
    	t_terms[term_with_process].curr_process = pcb_arr[curr_process];

    	// mark pcb in array as empty
    	pcb_arr[curr_process] = -1;

    	// update current process to the parent (part 2)
    	curr_process = t_terms[term_with_process].curr_process;
    }
    else	// base shell case
    {
    	// mark pcb in array as empty
    	pcb_arr[curr_process] = -1;

    	t_terms[term_with_process].process_flag = 0;
		execute((uint8_t*)"shell");
    }
    
    tss.esp0 = (EIGHT_MB - ((curr_process) * EIGHT_KB));
	
    /* restore parent paging */
    create_process_page(curr_process);
    
    /* close relevant FDs */
    for (i=2; i<FD_ARRAY_SIZE; ++i)
    {
        if(curr_pcb->fd_array[i].flags == IN_USE)
        {
            close(i);
            curr_pcb->fd_array[i].flags = NOT_IN_USE;
        }
    }
 
    /* jump to execute return, using inline assembly to halt_jump */
	asm volatile(
				
				"movl %0, %%esp;"
				"movl %1, %%ebp;"

				"jmp halt_jump;"
				:
				: "m" (ret_esp), "m" (ret_ebp)
	);

    return -1;
}

/* int32_t read(int32_t fd, void* buf, int32_t nbytes)
 *
 * copies data into a buffer
 *
 * Inputs: fd - the current file descriptor in use
 		 : buf - a buffer to copy into
 		 : nbytes - the number of bytes to copy
 * Outputs: number of bytes read
 * Side Effects: fills the buf with data
 */
int32_t read(int32_t fd, void* buf, int32_t nbytes)
{
	pcb_struct * curr_pcb = get_curr_pcb();

	if((buf == NULL) || (fd < MIN_VAL) || (fd > MAX_VAL) || (nbytes < MIN_VAL))
		return -1;
	if((curr_pcb->fd_array[fd].flags == 0) || (fd == 1))
		return -1;

	return curr_pcb->fd_array[fd].fileop_ptr->read(fd, buf, nbytes);
}

/* int32_t write(int32_t fd, const void* buf, int32_t nbytes)
 *
 * writes onto the display or do nothing
 *
 * Inputs: fd - the current file descriptor in use
 		 : buf - nothing
 		 : nbytes - the number of bytes to  write
 * Outputs: number of bytes written
 * Side Effects: might write to the screen
 */
int32_t write(int32_t fd, const void* buf, int32_t nbytes)
{
	pcb_struct * curr_pcb = get_curr_pcb();

	if((buf == NULL) || (fd < MIN_VAL) || (fd > MAX_VAL) || (nbytes < MIN_VAL))
		return -1;
	if((curr_pcb->fd_array[fd].flags == 0) || (fd == 0))
		return -1;

	return curr_pcb->fd_array[fd].fileop_ptr->write(fd, buf, nbytes);
}

/* int32_t open(const uint8_t* filename)
 *
 * provides access to the filesystem
 *
 * Inputs: filename - the file to open
 * Outputs: -1 if failure
 		  :  the number of the fd that was just created if successful
 * Side Effects: fills the fd of the current pcb with information
 */
int32_t open(const uint8_t* filename)
{
	if (filename == NULL || *filename == '\0')
	    return -1;

	pcb_struct * curr_pcb = get_curr_pcb();

	dentry_t curr_dentry;

	int32_t ret_val = read_dentry_by_name(filename, &curr_dentry);
	if (ret_val == -1)
	    return ret_val;

	int dentry_type = curr_dentry.type;
	fileop_jumptable * fileop_ptr;
	uint32_t inode;
	uint32_t file_position;
	uint32_t flags;

	switch(dentry_type)
	{
	    case(RTC_TYPE):
	        fileop_ptr = &rtc_jumptable;
	        inode = 0;
	        file_position = -1;
	        flags = IN_USE;
	        break;

	    case(DIR_TYPE):
	        fileop_ptr = &dir_jumptable;
	        inode = 0;
	        file_position = 0;
	        flags = IN_USE;
	        break;
	        
	    case(FILE_TYPE):
	        fileop_ptr = &file_jumptable;
	        inode = curr_dentry.node;
	        file_position = 0;
	        flags = IN_USE;
	        break;

	    default:
	        ret_val = -1;
	        return ret_val;
	}

	ret_val = find_empty_fd(curr_pcb);

	if (ret_val == -1)
	    return ret_val;

	curr_pcb->fd_array[ret_val].flags = flags;
	curr_pcb->fd_array[ret_val].inode = inode;
	curr_pcb->fd_array[ret_val].fileop_ptr = fileop_ptr;
	curr_pcb->fd_array[ret_val].file_position = file_position;

	curr_pcb->fd_array[ret_val].fileop_ptr->open(filename);

	return ret_val;

}

/* int32_t close(int32_t fd)
 *
 * closes the specified file descriptor and makes it available for return
 * for later calls to open
 *
 * Inputs: fd - the file descriptor to close
 * Outputs: -1 if failure
 		  :  0 if successs
 * Side Effects: closes a file descriptor in the current pcb
 */
int32_t close(int32_t fd)
{
	pcb_struct * curr_pcb = get_curr_pcb();

    if ((fd < MIN_VAL) || (fd > (FD_ARRAY_SIZE-1)) || (fd == STDIN_NUM) || (fd == STDOUT_NUM) || (fd > MAX_VAL))
        return -1;
    if (curr_pcb->fd_array[fd].flags != IN_USE)
        return -1;

    curr_pcb->fd_array[fd].flags = NOT_IN_USE;

    return curr_pcb->fd_array[fd].fileop_ptr->close(fd);
}

/* int32_t getargs(uint8_t* buf, int32_t nbytes)
 *
 * reads the program's command line arguments into a user level buffer
 *
 * Inputs: buf - buffer to copy into
 *		 : nbytes - number of bytes read
 * Outputs: -1 if failure
 		  :  0 if successs
 * Side Effects: copies arguments into a user level buffer
 */
int32_t getargs(uint8_t* buf, int32_t nbytes)
{	
	int i;

	//gets the length of the arguments
	for(i = 0; file[i] != '\0'; i++);

	if(buf == NULL || nbytes < i || file[0] == '\0')
		return -1;
	
	//fills buffer with arguments
	for(i = 0; file[i] != '\0'; i++)
		buf[i] = file[i];

	buf[i] = '\0';

	return 0;
}

/* int32_t vidmap(uint8_t** screen_start)
 *
 * maps the text-mode video memory into user space at a preset virtual address
 *
 * Inputs: screen_start - the pointer to map the video memory to
 * Outputs: the starting addressed of the mapped memory
 * Side Effects: maps video memory into user space
 */
int32_t vidmap(uint8_t** screen_start)
{
	if((uint32_t)screen_start > HIGH_BOUND || (uint32_t)screen_start < LOW_BOUND || screen_start == NULL)
		return -1;

	pcb_struct * curr_pcb = get_curr_pcb();
   	int term_with_process = find_term(curr_pcb->process_id);

  	create_video_page(term_with_process);
	*screen_start = (uint8_t*)(HIGH_BOUND + (FOUR_MB * (term_with_process + 1)));

	return HIGH_BOUND + (FOUR_MB * (term_with_process + 1));
}

///////////////////HELPER FUNCTIONS////////////////////////

/* int32_t find_empty_fd(pcb_struct* curr_pcb)
 *
 * finds the next empty fd in a pcb struct
 * Inputs: curr_pcb - the cureent pcb to search for an empty fd in
 * Outputs: returns -1 of failure
 *		  : returns the fd number if finds a empty fd
 * Side Effects: nothing
 */
int32_t find_empty_fd(pcb_struct* pcb)
{
    int i;
    for (i = 0; i < FD_ARRAY_SIZE; i++)
        if (pcb->fd_array[i].flags == NOT_IN_USE)
            return (int32_t)i;
   
    return -1;
}

/* int32_t next_process_num()
 *
 * finds next process number
 * Inputs: none
 * Outputs: next process number
 * Side Effects: nothing
 */
int next_process_num()
{
	int i;

	for(i = 0; i < 6; i++)
	{
		if(pcb_arr[i] == -1)
			return i;
	}

	return -1;	
}

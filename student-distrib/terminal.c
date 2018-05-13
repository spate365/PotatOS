/* Terminal Display Driver */

/* Files Imported */
#include "terminal.h"
#include "system_calls.h"

volatile int first = 1;

/* void term_open(const uint8_t* filename)
 *
 * initializes terminal and values used by the terminal
 *
 * Inputs: filename (nothing)
 * Outputs: returns 0 when success
 * Side Effects: None
 */
int32_t term_open(const uint8_t* filename)
{
	int i, j;
	curr_running = 0;
	term_active = 0;

	t_terms[0].change_attrib = 0x09;	// lavender
	t_terms[1].change_attrib = 0x0C;	// peach
	t_terms[2].change_attrib = 0x03;	// teal

	t_terms[0].vidmem_cpy = VIDEO_1;
	t_terms[1].vidmem_cpy = VIDEO_2;
	t_terms[2].vidmem_cpy = VIDEO_3;

	for(i = 0; i < TOT_TERMS; ++i)
	{
		t_terms[i].x_position = 0;
		t_terms[i].y_position = 0;
		t_terms[i].b_index = 0;
		t_terms[i].read_index = 0;
		t_terms[i].read_flag = 0;
		t_terms[i].is_active = 0;

		t_terms[i].process_flag = 0;
		t_terms[i].rtc_flag = 0;
		t_terms[i].curr_process = -1;

		t_terms[i].esp = 0;
		t_terms[i].ebp = 0;

	    for (j = 0; j < NUM_ROWS * NUM_COLS; j++) {
	        *(uint8_t *)(t_terms[i].vidmem_cpy + (j << 1)) = ' ';
	        *(uint8_t *)(t_terms[i].vidmem_cpy + (j << 1) + 1) = t_terms[i].change_attrib;
	    }

		// clears the buffer in all 3 screens
		for(j = 0; j < BUFF_SIZE; ++j)
			t_terms[i].kb_buffer[j] = '\0';
	}

	return 0;
}

/* int32_t term_read(int32_t fd, void* buff, int32_t nbytes)
 *
 * Fills buff with buffer typed after enter is pressed
 *
 * Inputs: fd (nothing)
 *		   buff(nothing)
 *		   nbytes(bytes to read)
 * Outputs: returns number of bytes read
 * Side Effects: fills buff with data
 */
int32_t term_read(int32_t fd, void* buff, int32_t nbytes)
{
	int i;

	// error cases
	if (buff == NULL || nbytes < 0)
		return -1;
	if (nbytes == 0)
		return 0;

	char* buffer = (char*)buff;

	// spin until enter is pressed
	while(t_terms[curr_running].read_flag != 1);

	for(i = 0; i < t_terms[curr_running].read_index; i++)
		buffer[i] = t_terms[curr_running].kb_buffer[i];
	

	buffer[t_terms[curr_running].read_index] = '\n';
	t_terms[curr_running].read_index++;

	buffer[t_terms[curr_running].read_index] = '\0';
	
	t_terms[curr_running].read_flag = 0;

	return t_terms[curr_running].read_index; //num bytes read
}

/* void term_write(int32_t fd, void* buff, int32_t nbytes)
 *
 * writes to the screen
 *
 * Inputs: fd (nothing)
 *		   buff(nothing)
 *		   nbytes(bytes to write)
 * Outputs: returns number of bytes written
 *		  : -1 if failure
 * Side Effects: prints onto screen
 */
int32_t term_write(int32_t fd, const void* buff, int32_t nbytes)
{
	int i;

	// error cases
	if (buff == NULL || nbytes < 0)
		return -1;
	if(t_terms[curr_running].b_index == BUFF_SIZE)
		return 0;

	char* buffer = (char*)buff;

	cli();

	// print buffer character by character
	for(i = 0; i < nbytes; i++)
	{
		if(buffer[i])
		{
			if(scheduled_term == curr_running || total_active < TOT_TERMS)
				putc((char) buffer[i]);
			else
				putc2((char)buffer[i], scheduled_term);
		}
	}

	sti();

	return nbytes; //num bytes written
}

/* int32_t term_close(int32_t fd)
 *
 * clears the screen and returns
 *
 * Inputs: fd (nothing)
 * Outputs: returns 0 when success
 * Side Effects: clears the screen
 */
int32_t term_close(int32_t fd)
{
	return 0;
}

/* void change_screen(int32_t new_terminal)
 *
 * switches to another terminal
 *
 * Inputs: new_terminal - the terminal to switch to
 * Outputs: none
 * Side Effects: none
 */
void change_screen(int32_t new_terminal)
{
	if(curr_running == new_terminal && first == 0)
		return;

	// make a copy of the current terminal
	memcpy((void*)t_terms[curr_running].vidmem_cpy, (void*)VIDEO, FOUR_KB);

	// Load the new terminal
	memcpy((void*)VIDEO, (void*)t_terms[new_terminal].vidmem_cpy, FOUR_KB);

	update_term(new_terminal);
}

/* void update_term(int i)
 *
 * updates the screen from the data of the new terminal, and save the old
 * screen to the data of the old terminal
 *
 * Inputs: i - the new terminal number
 * Outputs: none
 * Side Effects: updates screen x, screen y and atrribute
 */
void update_term(int i)
{
	// copy the locations
	if(first == 0)
	{
		t_terms[curr_running].x_position = screen_x;
		t_terms[curr_running].y_position = screen_y;
	}
	else
		first = 0;

	// set the running to correct screen
	curr_running = i;	

	// sets the new locations
	screen_x = t_terms[curr_running].x_position;
	screen_y = t_terms[curr_running].y_position;
	ATTRIB = t_terms[curr_running].change_attrib;

	update_cursor(screen_x, screen_y);
}

/* void find_term(int process_num)
 *
 * finds the terminal that is currently running the process
 *
 * Inputs: process_num - the process that is running on the terminal
 * Outputs: the terminal number that is running the process
 * Side Effects: none
 */
int find_term(int process_num)
{
	int i;
	for(i = 0; i < TOT_TERMS; i++)
	{
		if(t_terms[i].curr_process == process_num)
			return i;
	}
	return 0;
}


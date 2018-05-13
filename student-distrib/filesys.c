/* File System */

/* Files Imported */
#include "filesys.h"

/* void init_filesys()
 *
 * initializes to file system for use
 * Inputs: mod = starting location of the mod structure (for data reading)
 * Outputs: None
 * Side Effects: None
 */
void init_filesys(uint32_t mod)
{
	global_index = 0;
	start_loc = (uint8_t*) mod;

	if(start_loc == NULL)
		return;

	boot_block = (boot_t*)(start_loc);
	file_locator = (dentry_t*)(start_loc + END_OF_DIR);
 	block_chain = (inode_t*)(start_loc + FILE_DATA);
}

/* int32_t rtc_init(const uint8_t* filename)
 *
 * copies the filename into the global dentry
 * Inputs: filename
 * Outputs: return 0 for success
 * Side Effects: copies the filename into the global dentry
 */
int32_t file_open(const uint8_t* filename)
{
	return 0;
}

/* int32_t dir_open(const uint8_t* filename)
 *
 * does nothing
 * Inputs: filename
 * Outputs: return 0 for success
 * Side Effects: nothing
 */
int32_t dir_open(const uint8_t* filename)
{
	return 0;
}

/* int32_t file_close(int32_t fd)
 *
 * does nothing
 * Inputs: fd (nothing)
 * Outputs: return 0 for success
 * Side Effects: nothing
 */
int32_t file_close(int32_t fd)
{
	return 0;
}

/* int32_t dir_close(int32_t fd)
 *
 * does nothing
 * Inputs: fd (nothing)
 * Outputs: return 0 for success
 * Side Effects: nothing
 */
int32_t dir_close(int32_t fd)
{
	return 0;
}

/* int32_t file_read(int32_t fd, void* buf, int32_t nbytes)
 *
 * reads a file
 * Inputs: fd (inode index)
 *		   buf (buffer to store data)
 *		   nbytes (to read)
 * Outputs: return number of bytes correctly read
 * Side Effects: fills buf with data
 */
int32_t file_read(int32_t fd, void* buf, int32_t nbytes)
{
	pcb_struct * curr_pcb = get_curr_pcb();
	
	int32_t num_bytes = 0;
	int32_t node_num = curr_pcb->fd_array[fd].inode;
	int32_t length = block_chain[node_num].length;

	//error checking
	if((curr_pcb->fd_array[fd].flags!=IN_USE) || (curr_pcb->fd_array[fd].fileop_ptr==NULL))
		return -1;

	// uint32_t inode_idx = curr_pcb->fd_array[fd].inode;
	uint32_t offset = curr_pcb->fd_array[fd].file_position;

	//printf("%d %d %d \n", node_num, offset, length);
	if(nbytes > length)
		num_bytes = read_data(node_num, offset, (uint8_t*)buf, length);
	else
		num_bytes = read_data(node_num, offset, (uint8_t*)buf, nbytes);

	if(length > offset && num_bytes == 0)
		num_bytes = nbytes;

	//increment file position by the number of bytes read
	curr_pcb->fd_array[fd].file_position += num_bytes;

	return num_bytes;
}


/* int32_t dir_read(int32_t fd, void* buf, int32_t nbytes)
 *
 * fills buffer with with one directory name at the global index
 * Inputs: fd (nothing)
 *		   buf (nothing)
 * 		   nbytes (nothing)
 * Outputs: return 0 for success
 * Side Effects: fills buffer with directory name of the index
 */
int32_t dir_read(int32_t fd, void* buf, int32_t nbytes)
{
	//error case
	if(global_index == *start_loc)
		return 0;

	char* buffer = (char*)buf;

	int index = read_dentry_by_index(global_index, &name);
	if(index == 0)
	{
		//copies the filename into the buffer
		strncpy(buffer, (char*)name.filename, FILE_NAME_B);
	 	global_index++;

	 	return FILE_NAME_B;
	}	
	return 0;
}

/* int32_t file_write(int32_t fd, void* buf, int32_t nbytes)
 *
 * does nothing
 * Inputs: fd (nothing)
 *		   buf (nothing)
 * 		   nbytes (nothing)
 * Outputs: return -1 for success
 * Side Effects: nothing
 */
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes)
{
	return -1;
}

/* int32_t dir_write(int32_t fd, void* buf, int32_t nbytes)
 *
 * does nothing
 * Inputs: fd (nothing)
 *		   buf (nothing)
 * 		   nbytes (nothing)
 * Outputs: return -1 for success
 * Side Effects: nothing
 */
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes)
{
	return -1;
}

/* int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry)
 *
 * gets dentry from name and stores it in the argument
 * Inputs: fname (file name)
 *		   dentry_t* dentry (nothing)
 * Outputs: return 0 for success
 * Side Effects: fills dentry with information
 */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry)
{
	int i;

	if(fname == NULL || dentry == NULL || (strlen((int8_t*)fname) > FILE_NAME_B))
		return -1;

	//loops through to find dentry by name
	for(i = 0; i < END_OF_DIR; i++)
	{
		if(strncmp((char*)fname, (char*) file_locator[i].filename, FILE_NAME_B) == 0)
		{
			memcpy(dentry, file_locator[i].filename, END_OF_DIR);
			return 0;
		}
	}

	return -1;
}

/* int32_t read_dentry_by_index(const uint8_t* fname, dentry_t* dentry)
 *
 * gets dentry from index and stores it in argument
 * Inputs: fname (index)
 *		   dentry_t* dentry (nothing)
 * Outputs: return 0 for success
 * Side Effects: fills dentry with information
 */
int32_t read_dentry_by_index(const uint32_t index, dentry_t* dentry)
{
	if(index < 0 || index >= END_OF_DIR || dentry == NULL)
		return -1;

	memcpy(dentry, file_locator + index, END_OF_DIR);

	return 0;
}

/* int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
 *
 * reads the data from the data blocks
 * Inputs: inode (inode number)
 *		   offset (offset in data blocks) 
 *		   buf (nothing)
 * 		   length (how long)
 * Side Effects: fills buf with data
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
{
	uint32_t offset_div = offset / FILE_DATA;	//correct block
 	uint32_t offset_mod = offset % FILE_DATA;	//correct index
 	int32_t file_length = block_chain[inode].length;
 	uint32_t num_bytes = 0;
 	int count = 0;

 	if(boot_block->num_inodes < inode || buf == NULL)
		return -1;
	if(offset >= file_length)
		return 0;

	//trim excess length
	if(length + offset > file_length)
		length = file_length - offset;

	//read data
	while((num_bytes < file_length) && (num_bytes < length))
	{		
		uint8_t* loc = (uint8_t*)start_loc + (boot_block->num_inodes + 1) * FILE_DATA;
		loc += block_chain[inode].data_block[offset_div + count] * FILE_DATA;

		if((length + offset_mod - num_bytes) < FILE_DATA)	//if length to read is less than size of block
		{
			//read length bytes
			memcpy(buf + num_bytes, (uint8_t*)(loc + offset_mod), (length - num_bytes));
			num_bytes += (length - num_bytes);
			break;		
		}
		else	//if length to read is larger than size of block
		{
			//read to the end of the block
			memcpy(buf + num_bytes, (uint8_t*)(loc + offset_mod), (FILE_DATA - offset_mod));
			num_bytes += (FILE_DATA - offset_mod);

			//move to next data block and start from the top
			offset_mod = 0;
			count++;
		}
	}

 	return num_bytes;
}

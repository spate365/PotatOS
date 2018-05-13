/* Unit Test File */

/* Files Imported */
#include "tests.h"

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	//loop through all exceptions
	for (i = 0; i < NUM_VEC; ++i)
	{
		if ((i < SIZE_IDT) || (i == KBINDEX) || (i == RTCINDEX) || (i == SYSINDEX))
		{
			if ((idt[i].offset_15_00 == NULL) && 
				(idt[i].offset_31_16 == NULL)){
				assertion_failure();
				result = FAIL;
			}
		}
	}

	return result;
}

/* Paging Test 1
 *
 * Checks the first two inputs in the page_directory array are filled correctly
 * Inputs: None
 * Outputs: None
 * Side Effects: Prints address of page_directory[0] and page_directory[1]
 * Coverage: Paging boundary cases
 * Files: paging.c
 */
void paging_test1()
{
	TEST_HEADER;
	
	//values contained in the paging structure
	clear();
	printf("%d\n", page_directory[0]);
	printf("%d\n", page_directory[1]);
}

/* Paging Test 2
 *
 * Checks that you cannot access an address outside of the range of video memory and kernel memory
 * Inputs: None
 * Outputs: None
 * Side Effects: Page faults, blue screen of death :(
 * Coverage: Paging boundary cases
 * Files: paging.c
 */
void paging_test2()
{
	TEST_HEADER;
	
	int * var;
	
	//should page fault
	var = NULL;
	*var = 5;
}

/* Paging Test 3
 *
 * Checks that you can access an address inside of the range of video memory and kernel memory
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: Paging boundary cases
 * Files: paging.c
 */
void paging_test3()
{
	TEST_HEADER;
	
	//should not page fault
	int * var;
	
	//tests kernel
	var = (int *)0x400000;
	*var = *var;
	
	var = (int *)0x400001;
	*var = *var;
	
	var = (int *)0x799999;
	*var = *var;
	
	//tests vid mem
	var = (int *)0xB8000;
	*var = *var;
	
	var = (int *)0xB8001;
	*var = *var;
	
	var = (int *)0xB8999;
	*var = *var;
}
// add more tests here

/* Checkpoint 2 tests */
// void rtc_test()
// {
// 	clear();
// 	rtc_test_flag = 1;
// 	rtc_open(NULL); 
// }

// void rtc_test2(int* frequency)
// {
// 	clear();

// 	rtc_test_flag = 1;

// 	while(1)
// 	{
// 		if(rtc_count == 0)
// 		{
// 			rtc_open(NULL);
// 			rtc_write(0,frequency,4);
// 		}
// 		if(*frequency >= 1024)
// 			break;
// 		if((rtc_count % 80) == 0)
// 		{	
// 			char c[1] = {'1'};
// 			term_write(0,(char*)c,1);
// 			*frequency *= 2;
// 			rtc_write(0,frequency,4);
// 		}
// 	}
//}

// void rtc_test3()
// {

// 	rtc_test_flag = 1;

// 	clear();

// 	rtc_open(NULL);

// 	rtc_read(0,0,0);

// }

void tr_test()
{
	char c_array[129];

	term_read(0, c_array, 129);

	printf("term_read works: %s\n", c_array);
}

// void fish_gif()
// {
// 	rtc_test_flag = 0;
	
// 	uint8_t file[] = "frame0.txt\0";
// 	uint8_t file2[] = "frame1.txt\0";

// 	file_open((uint8_t*) file);
// 	file_open((uint8_t*) file2);

// 	char buf[50000];
// 	char buf2[50000];

// 	rtc_open(NULL);

// 	while(1)
// 	{
// 		read_data(38, 0, (uint8_t*)buf, 50000);
// 		term_write(0, buf, 50000);

// 		rtc_read(0,0,0);
// 		clear();

// 		read_data(47, 0, (uint8_t*)buf2, 50000);
// 		term_write(0, buf2, 50000);

// 		rtc_read(0,0,0);
// 		clear();
// 	}
// }

void dir_read_test()
{
	uint8_t dirname[] = ".\0";
	file_open(dirname);
	uint8_t fname[32];

	while(dir_read(0, fname, 4)!=0);
}

void test_big_files()
{
 	int i, j;
 	rtc_open(NULL);
 	for(i = 0; i < 17; i++)
 	{
 		clear();
 		dentry_t name;
 		int a = read_dentry_by_index(i, &name);

 		uint8_t file[32];
 		memcpy(file, (uint8_t*) name.filename, 32);
		file_open((uint8_t*) file);
		uint32_t length = block_chain[name.node].length;
		char buf[length];

		a = read_data(name.node, 0, (uint8_t*)buf, length);
		term_write(0, buf, length);
		j = 0;
		while(j < 10)
		{
			rtc_read(0,0,0);
			j++;
		}
		//printf("%x", buf);
		//printf("\n%d",a);
 	}

}

void null_test()
{
	int a,b,c,d,e,f;
	
	b = rtc_write(0, NULL, 0);
	c = term_read(0,NULL, 0);
	d = term_write(0, NULL, 0);
	a = read_data(0,0,NULL,0);
	e = read_dentry_by_index(-1, NULL);
	f = read_dentry_by_name(NULL, NULL);

	if((a == 0)||(b == 0)||(c == 0)||(d == 0)||(e == 0))
		printf("FAILED NULL TEST\n");
	else
		printf("PASSED NULL TEST\n");

}
/* Checkpoint 3 tests */
void open_close_test()
{
	pcb_struct* pcb = (pcb_struct*)(EIGHT_MB - EIGHT_KB);
	create_pcb(pcb, 0, (uint8_t*)"\0");

	printf("Opening the File...\n");
	open ((uint8_t*) "frame0.txt");

	printf("file position: %d\n", pcb->fd_array[2].file_position);
	printf("inode number: %d\n", pcb->fd_array[2].inode);
	printf("in use: %d\n", pcb->fd_array[2].flags);

	printf("Closing the File...\n");
	close (2);

	printf("file position: %d\n", pcb->fd_array[2].file_position);
	printf("inode number: %d\n", pcb->fd_array[2].inode);
	printf("in use: %d\n", pcb->fd_array[2].flags);
}


void read_write_test()
{
	pcb_struct* pcb = (pcb_struct*)(EIGHT_MB - EIGHT_KB);
	create_pcb(pcb, 0, (uint8_t*)"\0");

	uint8_t buf[1024];
	open ((uint8_t*) "frame0.txt");
	read(2, buf, 187);

	term_write(0,buf,187);
}

/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests()
{
	/*****************Checkpoint 1*****************/

		// TEST_OUTPUT("idt_test", idt_test());
		// paging_test1();
		// paging_test2();
		// paging_test3();

	/*****************Checkpoint 2*****************/
	

	/* rtc open test */

		// rtc_test();

	/* rtc write tests */
		// int frequency = 8;
		// rtc_test2(&frequency);

	/* rtc read test */
		// rtc_test3();

	/* kb read test */

		// tr_test();

	/* filesys test */
		// test_big_files();
		// fish_gif();
		//dir_read_test();

	/* null test */
		// null_test();

	/*****************Checkpoint 3*****************/

	// execute((uint8_t*)"shell");
	//open_close_test();
	//read_write_test();


	/*****************Checkpoint 4*****************/
	/*****************Checkpoint 5*****************/


}


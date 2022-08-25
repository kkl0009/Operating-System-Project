#include "TestR5.h"
#include "../R1/r1functions.h"
#include "../../include/mem/heap.h"

static CMCB* allocated_head;
static CMCB* free_head;

static u32int memory_start;//When this wasn't static, there was some weird pointer stuff going on
static u32int total_heap_size;

/**
 * This function sets up the initial information for the heap, including its CMCB and LMCB. It also initializes the head of the 
 * allocated and free lists for the CMCBs. It is meant to be run only once, at the beginning of the system's execution (like a 
 * constructor)
 * 
 * @param heap_size - the size (in bytes) to set the heap
*/
void init_heap(u32int heap_size)
{
	//Allocate memory for the heap
	int heap_buffer = 500;

	memory_start = heap_buffer + kmalloc(heap_buffer + heap_size + sizeof(CMCB) + sizeof(LMCB));
	total_heap_size = memory_start;

	//Initialize the CMCB
	CMCB* new_cmcb = (void*)memory_start;//Check this line, this may not be the proper way to put this block in memory
	new_cmcb -> type = Free;
	new_cmcb -> address = memory_start;
	new_cmcb -> size = heap_size;
	//new_cmcb -> proc_name = NULL;
	new_cmcb -> next = NULL;
	new_cmcb -> prev = NULL;

	//Initialize the LMCB
	LMCB* new_lmcb = (void*)(memory_start + heap_size + sizeof(CMCB));
	new_lmcb -> type = Free;
	new_lmcb -> size = heap_size;

	//Initialize the pointers to the lists
	allocated_head = NULL;
	free_head = new_cmcb;

}

/**
 * This function is used to allocate a specific block of memory of a given size. Memory allocation for this system is performed using
 * a first-fit algorithm; that is, the first free block in the free list that is large enough for the allocated memory will be used to
 * allocate the memory. All memory blocks, allocated and free, include both a CMCB and an LMCB, which are used for accessing different
 * information about the blocks
 * 
 * @param bytes - the number of bytes to allocate
 * @return the memory address at which the allocated block was formed
*/
u32int allocate_mem(u32int bytes)
{

	//Determine the size of the full block for later use
	u32int block_size = bytes + sizeof(CMCB) + sizeof(LMCB);

	//Start the current CMCB to the head of the free list
	CMCB* curr_cmcb = free_head;
	while(curr_cmcb != NULL)
	{

		if(curr_cmcb -> size >= block_size)
		{
			//Swap the selected block from the Free to the Allocated list
			add_cmcb(remove_cmcb(curr_cmcb), Allocated);

			//Keep track of the original size of the free block for later use
			u32int old_size = curr_cmcb -> size;

			curr_cmcb -> size = bytes;

			//Create the new LMCB to go at the bottom of the allocated block
			LMCB* new_lmcb = (void*)(curr_cmcb -> address + curr_cmcb -> size + sizeof(CMCB));
			new_lmcb -> type = Allocated;
			new_lmcb -> size = bytes;

			//Set up the new CMCB for the free section to go after the allocated section
			CMCB* new_cmcb = (void*)(curr_cmcb -> address + block_size);
			new_cmcb -> address = curr_cmcb -> address + block_size;
			new_cmcb -> size = old_size - block_size;
			new_cmcb -> next = NULL;
			new_cmcb -> prev = NULL;

			//Add the newly created CMCB to the free list
			add_cmcb(new_cmcb, Free);

			//Update the existing LMCB at the bottom of the free block
			LMCB* free_lmcb = (void*)(new_cmcb -> address + sizeof(CMCB) + new_cmcb -> size);
			free_lmcb -> type = Free;
			free_lmcb -> size = new_cmcb -> size;

			
			return curr_cmcb->address + sizeof(CMCB);
		}

		//Move to next CMCB in the list
		curr_cmcb = curr_cmcb -> next;

	}
	println("\nNo sufficiently large memory blocks are available");
	return NULL;
}

/**
 * This function is used to free an allocated block of memory. The freed block of memory will be merged with any adjacent free blocks to
 * form larger free blocks. Free blocks and allocated blocks each use CMCBs and LMCBs to access important data about them
 * 
 * @param ipaddr - The address of the block of allocated memory to free (must be a valid address of an allocated block)
 * @return 1 if the allocated block is successsfully freed, and 0 otherwise
*/
int free_mem(void* ipaddr)
{
	u32int input_address = (u32int)ipaddr - sizeof(CMCB);

	//If there is no allocated memory, then the free function will not do anything
	if(allocated_head == NULL)
	{
		println("\nThere is no allocated memory");
		return 0;
	}

	//Looping through the allocated list to find the CMCB for the memory to free
	CMCB* curr_cmcb = allocated_head;
	while(curr_cmcb != NULL)
	{
		if(curr_cmcb -> address == input_address)
		{
			break;
		}

		curr_cmcb = curr_cmcb -> next;
	}

	//Determine whether an actual CMCB was found or not
	if(curr_cmcb == NULL)
	{
		println("\nERROR: There is no CMCB at the given address");
		return 0;
	}

	//Move the CMCB from the allocated list to the free list
	add_cmcb(remove_cmcb(curr_cmcb), Free);

	//Get the address location for the lmcb of the previous memory block
	u32int prev_lmcb_addr = curr_cmcb -> address - sizeof(LMCB);

	//Determine whether the address for the LMCB is within the bounds of the heap
	//In the case that it is outside of these bounds, that should just mean this block is the first block to appear within the heap
	if(prev_lmcb_addr > memory_start)
	{
		//Find the LMCB for the previous block using the address that was just calculated
		LMCB* prev_lmcb = (void*)prev_lmcb_addr;

		//Determine whether the block above the current block is free or not
		if(prev_lmcb -> type == Free)
		{
			//Find the CMCB for the block based on the size given in the LMCB
			CMCB* prev_cmcb = (void*)(prev_lmcb_addr - (prev_lmcb -> size + sizeof(CMCB)));

			//Reset the size so that it encompasses the entire free block now
			prev_cmcb -> size = prev_cmcb -> size + sizeof(LMCB) + sizeof(CMCB) + curr_cmcb -> size;

			//Remove the old CMCB from the free queue as it is no longer necessary
			remove_cmcb(curr_cmcb);

			//Reassign the curr_cmcb pointer so it points to the new CMCB
			curr_cmcb = prev_cmcb;
		}

	}

	//Calculate the address of the CMCB for the next memory block after the current block
	u32int next_cmcb_addr = curr_cmcb -> address + sizeof(CMCB) + curr_cmcb -> size + sizeof(LMCB);

	//Determine whether the address of this CMCB appears within the bounds of the heap
	if(next_cmcb_addr < memory_start + sizeof(CMCB) + total_heap_size + sizeof(LMCB))
	{

		//Find the CMCB based on the previously calculated address
		CMCB* next_cmcb = (void*)next_cmcb_addr;

		//Determine whether the next block is a free block or not
		if(next_cmcb -> type == Free)
		{
			//Reassign the size of the current CMCB so that it encompasses the adjacent memory block as well
			curr_cmcb -> size = curr_cmcb -> size + sizeof(LMCB) + sizeof(CMCB) + next_cmcb -> size;

			//Remove the newly found CMCB from the free list as it is no longer needed
			remove_cmcb(next_cmcb);
		}
	}

	//Reset the size for the LMCB at the bottom of the memory block so it can be used by future calls of free
	LMCB* this_lmcb = (void*)(curr_cmcb -> address + sizeof(CMCB) + curr_cmcb -> size);
	this_lmcb -> type = Free;
	this_lmcb -> size = curr_cmcb -> size;

	return 1;
}

/**
 * This function prints out either all free blocks or all allocated blocks. For each block of memory, it will print out the
 * type of block it is, the address of the block, and the size of the block.
 * 
 * @param the_type - an enum type (Allocated or Free), indicating whether to print the free or allocated list
*/
void show_cmcbs(enum memory_type the_type)
{
	int error_save = 0;
	char number[12];

	CMCB* curr_cmcb;
	if(the_type == Allocated)
	{
		curr_cmcb = allocated_head;
	}
	else
	{
		curr_cmcb = free_head;
	}

	println("");
	while(curr_cmcb != NULL)
	{
		print("{ ");
		if(curr_cmcb -> type == Allocated)
		{
			print("Type=Allocated,");
		}
		else if(curr_cmcb -> type == Free)
		{
			print("Type=Free,");
		}
		else if(curr_cmcb -> type == NULL)
		{
			print("Type=NULL,");
		}
		else
		{
			print("Type=Unknown,");
		}

		print(" Address=");

		bad_itoa(number, curr_cmcb -> address + sizeof(CMCB));
		print(number);

		print(", Size=");

		bad_itoa(number, curr_cmcb -> size);
		print(number);
		println(" }");

		curr_cmcb = curr_cmcb -> next;
		if(error_save > 1000)
		{
			println("\nEXITING THE FUNCTION: Infinite loop suspected");
			return;
		}
		error_save++;
	}
}

/**
 * This function determines whether the heap is empty. The heap is considered empty if there are no allocated blocks
 * in the heap.
 * 
 * @return 1 is the heap contains no allocated blocks, and 0 otherwise
*/
int is_empty()
{
	if(allocated_head == NULL)
		return 1;
	return 0;
}

/**
 * This function is used to add a CMCB to either the allocated or the free list, depending on what type it is. Regardless of the list
 * CMCBs are added to the list based on their address (so the addresses of each are sorted in increasing order)
 * 
 * @param new_cmcb - the new CMCB to add to the list
 * @param the_type - an enum type (Allocated or Free) indicating which of the two lists to add the CMCB to
*/
void add_cmcb(CMCB* new_cmcb, enum memory_type new_type)
{
	CMCB* curr_cmcb;
	new_cmcb -> type = new_type;
	if(new_type == Allocated)
	{
		if(allocated_head == NULL)
		{
			allocated_head = new_cmcb;
			new_cmcb -> next = NULL;
			new_cmcb -> prev = NULL;
			return;
		}
		curr_cmcb = allocated_head;
	}
	else
	{
		if(free_head == NULL)
		{
			free_head = new_cmcb;
			new_cmcb -> next = NULL;
			new_cmcb -> prev = NULL;
			return;
		}
		curr_cmcb = free_head;
	}

	if(curr_cmcb -> address > new_cmcb -> address)
	{
		curr_cmcb -> prev = new_cmcb;
		new_cmcb -> next = curr_cmcb;
		if(new_type == Allocated)
		{
			allocated_head = new_cmcb;
		}
		else
			free_head = new_cmcb;
		return;
	}

	while(curr_cmcb -> next != NULL)
	{
		if(curr_cmcb -> next -> address > new_cmcb -> address)
		{
			new_cmcb -> next = curr_cmcb -> next;
			new_cmcb -> prev = curr_cmcb;
			new_cmcb -> next -> prev = new_cmcb;
			curr_cmcb -> next = new_cmcb;
			return;
		}
		curr_cmcb = curr_cmcb -> next;
	}

	curr_cmcb -> next = new_cmcb;
	new_cmcb -> prev = curr_cmcb;
}

/**
 * This function is used to remove a CMCB from either the free or allocated list (whichever it is located in). It determines which
 * list to remove the CMCB on based on the value stored in the CMCBs type parameter
 * 
 * @param old_cmcb - the CMCB to remove from the list
*/
CMCB* remove_cmcb(CMCB* old_cmcb)
{
	enum memory_type old_type = old_cmcb -> type;

	if(old_cmcb -> next != NULL)
	{
		old_cmcb -> next -> prev = old_cmcb -> prev;
	}

	if(old_cmcb -> prev != NULL)
	{
		old_cmcb -> prev -> next = old_cmcb -> next;
	}
	else
	{
		if(old_type == Allocated)
			allocated_head = old_cmcb -> next;
		else
			free_head = old_cmcb -> next;
	}
	

	old_cmcb -> type = NULL;
	old_cmcb -> next = NULL;
	old_cmcb -> prev = NULL;

	return old_cmcb;
}
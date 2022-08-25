#include "Queue.h"
#include "../R1/r1functions.h"
#include "../mpx_supt.h"
#include <string.h>
#include "PCB.h"
#include <core/serial.h>

/**
 * This function prints out a queue, including its size, and all members of the queue in order
 * of appearance, whether it is priority based or not.
 * 
 * @param queue - the queue to print out
*/
void printQueue(struct Queue* queue) {
	print("(");
	print_int((*queue).count);
	println("):");
	struct PCB* currPCB = (*queue).head;
	while (currPCB != NULL) {
		print((*currPCB).name);
		print(" ");

		char priorityString[11];
		bad_itoa(priorityString, (*currPCB).priority);
		print("(");
		print(priorityString);
		print(") [");
		if((*currPCB).type == User)
			print("User");
		else
			print("System");
		println("]");

		currPCB = (*currPCB).next;
	}
}


/**
 * This function is used to add a PCB to a queue. It has the ability to add a value to a queue
 * either in FIFO order to based on priority, depending on the value of the flag parameter.
 * 
 * @param queue - the queue to add the PCB to
 * @param PCB - the PCB to add to the queue
 * @param flag - if this is 0, insert using priority, and if it is 1, insert using FIFO
*/
void enqueuePCB(struct Queue* queue, struct PCB* pcb, int flag){

	struct PCB* currPCB = (*queue).head;
	//FLAG = 0 (PRIORITY SORT)
	if(flag == 0){
		if((*queue).count == 0){
			(*queue).head = pcb;
			(*queue).tail = pcb;
			(*pcb).next = NULL;
			(*pcb).prev = NULL;
		}
		else if((*currPCB).priority < (*pcb).priority){
			(*pcb).next = (*queue).head;
			(*queue).head = pcb;
			(*(*pcb).next).prev = pcb;
			(*pcb).prev = NULL;
		}
		else{
			while((*currPCB).next != NULL && (*(*currPCB).next).priority >= (*pcb).priority){
				currPCB = (*currPCB).next;
			}
			(*pcb).next = (*currPCB).next;
			(*pcb).prev = currPCB;

			(*(*currPCB).next).prev = pcb;

			(*currPCB).next = pcb;
		}
		
	
		
	//FLAG = 1	(FIFO SORT)
	} else {
		if ((*queue).count == 0) {
			(*queue).head = pcb;
			(*queue).tail = pcb;
			(*pcb).next = NULL;
			(*pcb).prev = NULL;
		} else {
			(*(*queue).tail).next = pcb;
			(*pcb).prev = (*queue).tail;
			(*queue).tail = pcb;
		}

	}
	(*queue).count = (*queue).count + 1;
}

/**
 * This function is used to allocate memory for a queue
 * 
 * @return the newly allocated queue
*/
Queue* allocate_queue() {
	Queue* newQueue = sys_alloc_mem(sizeof(Queue));
	if (newQueue)
		return newQueue;
	else 
		return NULL;
}

/**
 * A helper function for the findPCB() function found in the PCB.c file. Determines whether
 * a PCB with the given name is found in the given queue.
 * 
 * @param queue - the queue to search for the PCB in
 * @param name - the name of the PCB to look for in the queue
 * 
 * @return 1 if a PCB with the given name is found, 0 otherwise (effectively a boolean)
*/
int findPCBHelper(struct Queue* queue, char name[21])
{
	struct PCB* currPCB = (*queue).head;
	while(currPCB != NULL) {
		if(are_equal((*currPCB).name, name))
			return 1;
		currPCB = (*currPCB).next;
	}
	return 0;
}

/**
 * This function is used to get the PCB with the input name from the input queue, and works
 * effectively the same as the findPCBHelper() function.
 * 
 * @param queue - the queue to search for the PCB in
 * @param name - the name of the PCB to search for
 * 
 * @return the PCB that was found in the queue
*/
struct PCB* getPCB(struct Queue* queue, char name[21])
{
	struct PCB* currPCB = (*queue).head;
	while(currPCB != NULL) {
		if(are_equal((*currPCB).name, name))
			return currPCB;
		currPCB = (*currPCB).next;
	}
	return NULL;
}

/**
 * This function is a helper function for the delete_pcb() function found in the PCB.c file.
 * It removes a PCB from the input queue and returns it, updating all pointers as necessary.
 * 
 * @param queue - the queue to delete the PCB from
 * @param name - the name of the PCB to delete
 * 
 * @return the PCB that was removed from the queue
*/
struct PCB* delete_pcb_helper(struct Queue* queue, char name[21])
{
	struct PCB* currPCB = (*queue).head;
	if(are_equal((*currPCB).name, name))
	{
		(*queue).head = (*currPCB).next;
		(*queue).count = (*queue).count - 1;
		(*(*queue).head).prev = NULL;
		return currPCB;
	}

	if(currPCB != NULL)
		currPCB = (*currPCB).next;

	while(currPCB != NULL) 
	{
		if(are_equal((*currPCB).name, name))
		{
			struct PCB* prevPCB = (*currPCB).prev;
			struct PCB* nextPCB = (*currPCB).next;

			(*prevPCB).next = nextPCB;

			if(nextPCB != NULL)
				(*nextPCB).prev = prevPCB;
			else
				(*queue).tail = prevPCB;

			(*queue).count = (*queue).count - 1;
			//free_pcb(currPCB);
			return currPCB;
		}
		currPCB = (*currPCB).next;
	}
	return NULL;
}



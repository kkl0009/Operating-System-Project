#include "PCB.h"
#include "../R1/r1functions.h"
#include "../mpx_supt.h"
#include <string.h>

void internal_show_pcb(PCB*);



// static int queuesReady = 0;
static Queue* readyQueue;
static Queue* blockedQueue;
static Queue* suspendedReadyQueue;
static Queue* suspendedBlockedQueue;
// static PCB* runningPCB;

Queue* getReadyQueue(){
	return readyQueue;
}

Queue* getBlockedQueue(){
	return blockedQueue;
}

/**
 * 
*/
int is_system_proc(char* name) {
	PCB* pcb = NULL;

	switch(findPCB(name)) {
		case ReadyQ:      pcb = getPCB(readyQueue, name);            break;
		case BlockedQ:    pcb = getPCB(blockedQueue, name);          break;
		case SusReadyQ:   pcb = getPCB(suspendedReadyQueue, name);   break;
		case SusBlockedQ: pcb = getPCB(suspendedBlockedQueue, name); break;
		default: break;
	}

	if (pcb != NULL && pcb->type == System)
		return 1;
	return 0;
}

/**
 * This function allocates memory for all four of the queues that are used in the system.
*/
void allocate_queues() {
	readyQueue = allocate_queue();
	blockedQueue = allocate_queue();
	suspendedReadyQueue = allocate_queue();
	suspendedBlockedQueue = allocate_queue();
}

/**
 * This function displays the contents of all queues in the system, including the
 * ready, suspended ready, blocked, and suspended blocked queues
*/
void show_all()
{
	show_ready();
	show_blocked();
}

/**
 * This function displays the contents of the ready and suspended ready queues
 * to the screen. Note that these queues are based on priority
*/
void show_ready() {
	print("\nReady Queue ");
	printQueue(readyQueue);
	print("\nSuspended Ready Queue ");
	printQueue(suspendedReadyQueue);
}

/**
 * This function displays the contents of the blocked and suspended blocked queues
 * to the screen. Note that these queues are based on FIFO
*/
void show_blocked() {
	print("\nBlocked Queue ");
	printQueue(blockedQueue);
	print("\nSuspended Blocked Queue ");
	printQueue(suspendedBlockedQueue);
}


/**
 * This method allocates memory for a PCB using sys_alloc_mem from mpx_supt.c.
 * 	See doc for free_pcb().
 * 
 * @return A pointer to the allocated PCB if successful or NULL if not successful
*/
PCB* allocate_pcb() {
	PCB* newPCB = sys_alloc_mem(sizeof(PCB));
	if (newPCB)
		return newPCB;
	else 
		return NULL;
}

/**
 * This method frees memory of a PCB. At the moment, it just calls sys_free_mem()
 * 	because all that allocate_pcb() does is call sys_alloc_mem(). It's possible that I
 * 	may need to actually allocate dynamic memory for the stack, but I'm not 100% sure on 
 *  that. If that is the case, this and allocate_pcb() will become a bit more complex
 * 
 * @return a status code reflective of success/failure
*/
int free_pcb(PCB* pcb) {
	return sys_free_mem(pcb);
}


/**
 * This function creates a new PCB struct with a given name (3-20 characters), type (User or System),
 * and priority (0-9), using the setup_pcb() function. This struct is then inserted into the ready queue, 
 * using the enqueuePCB() function. It also checks to see whether a given name was already used to create
 * a process, in which case it will immediately exit the function.
 * 
 * @param name - a name to give to the newly created PCB
 * @param type - the type of process to create, either User or System
 * @param priority - the priority to give to the PCB
 * 
 * @return returns 0 if there is an error, and 1 otherwise
*/
PCB* create_pcb(char* name, enum proc_types type, int priority) {

	if(findPCB(name) != NoneQ)
	{
		print("\nA PCB named \"");
		print(name);
		println("\" already exists");
		return NULL;
	}

	PCB* newPCB = setup_pcb(name, type, priority);
	enqueuePCB(readyQueue, newPCB, 0);

	//print("\nPCB Created:");
	//internal_show_pcb(newPCB);
	return newPCB;
}

/**
 * This function deletes the PCB with the given name. It must first determine which of the four queues
 * the PCB with the given name is currently located using the findPCB() function. It then removes the
 * PCB from that queue using the delete_pcb_helper() function in the Queue.c file.
 * 
 * @param name - the name of the process to delete from the system
*/
void delete_pcb(char name[21])
{
	switch(findPCB(name))
	{
		case ReadyQ:
			delete_pcb_helper(readyQueue, name);
			break;
		case BlockedQ:
			delete_pcb_helper(blockedQueue, name);
			break;
		case SusReadyQ:
			delete_pcb_helper(suspendedReadyQueue, name);
			break;
		case SusBlockedQ:
			delete_pcb_helper(suspendedBlockedQueue, name);
			break;
		default:
			print("\nA PCB named \"");
			print(name);
			println("\" does not exist");
	}
}

/**
 * This function blocks the PCB with the input name. This is accomplished by moving the process to
 * the blocked queue if it is in the ready queue, or to the suspended blocked queue if it is in the
 * suspended ready queue. Additionally, the state value in the moved PCB is updated to reflect its
 * new state.
 * 
 * @param name - the name of the process to block
*/
void block_pcb(char name[21])
{
	klogv("BLOCKING:");
	klogv(name);

	PCB* movedPCB;
	switch(findPCB(name))
	{
		case ReadyQ:
			movedPCB = delete_pcb_helper(readyQueue, name);
			(*movedPCB).next = NULL;
			(*movedPCB).state = Blocked;
			enqueuePCB(blockedQueue, movedPCB, 1);

			//klogv("SUCCESS");
			// print("\nSuccessfully blocked \"");
			// print(name);
			// println("\"");
			break;
		case SusReadyQ:
			movedPCB = delete_pcb_helper(suspendedReadyQueue, name);
			(*movedPCB).next = NULL;
			(*movedPCB).state = BlockedSuspended;
			enqueuePCB(suspendedBlockedQueue, movedPCB, 1);
			// print("\nSuccessfully blocked \"");
			// print(name);
			// println("\"");
			break;
		case BlockedQ:
		case SusBlockedQ:
			// print("\nThe process \"");
			// print(name);
			// println("\" is already blocked");
			break;
		default:
			klogv("Failure");
			// print("\nA PCB named \"");
			// print(name);
			// println("\" does not exist");
	}
}

/**
 * This function unblocks the PCB with the input name. This is accomplished by moving the process to
 * the ready queue if it is in the blocked queue, or to the suspended ready queue if it is in the
 * suspended blocked queue. Additionally, the state value in the moved PCB is updated to reflect its
 * new state.
 * 
 * @param name - the name of the process to unblock
*/
void unblock_pcb(char name[21])
{
	//klogv("YAY UNBLOK");
	//klogv(name);

	// if(findPCB(name) == ReadyQ)
	// 	klogv("BlockedQ");

	PCB* movedPCB;
	switch(findPCB(name))
	{
		case ReadyQ:
		case SusReadyQ:
			// print("\nThe process \"");
			// print(name);
			// println("\" is not blocked");
			break;
		case BlockedQ:
			movedPCB = delete_pcb_helper(blockedQueue, name);
			(*movedPCB).next = NULL;
			(*movedPCB).state = Ready;
			enqueuePCB(readyQueue, movedPCB, 0);

			//klogv("YEEEESSSSS");
			// print("\nSuccessfully unblocked \"");
			// print(name);
			// println("\"");
			break;
		case SusBlockedQ:
			movedPCB = delete_pcb_helper(suspendedBlockedQueue, name);
			(*movedPCB).next = NULL;
			(*movedPCB).state = ReadySuspended;
			enqueuePCB(suspendedReadyQueue, movedPCB, 0);
			// print("\nSuccessfully unblocked \"");
			// print(name);
			// println("\"");
			break;
		default:
			// print("\nA PCB named \"");
			// print(name);
			// println("\" does not exist");
			break;
	}
}

/**
 * This function suspends the PCB with the input name. This is accomplished by moving the process to
 * the suspended ready queue if it is in the ready queue, or to the suspended blocked queue if it is in the
 * blocked queue. Additionally, the state value in the moved PCB is updated to reflect its
 * new state.
 * 
 * @param name - the name of the process to suspend
 * @return 1 if suspended successfully, 0 otherwise
*/
int suspend_pcb(char name[21])
{
	PCB* movedPCB;
	switch(findPCB(name))
	{
		case ReadyQ:
			movedPCB = delete_pcb_helper(readyQueue, name);
			(*movedPCB).next = NULL;
			(*movedPCB).state = ReadySuspended;
			enqueuePCB(suspendedReadyQueue, movedPCB, 0);
			return 1;
		case SusReadyQ:
			return 0;
		case BlockedQ:
			movedPCB = delete_pcb_helper(blockedQueue, name);
			(*movedPCB).next = NULL;
			(*movedPCB).state = BlockedSuspended;
			enqueuePCB(suspendedBlockedQueue, movedPCB, 1);
			return 1;
		case SusBlockedQ:
			return 0;
		default:
		return 0;
	}
}

/**
 * This function resumes the PCB with the input name. This is accomplished by moving the process to
 * the ready queue if it is in the suspended ready queue, or to the blocked queue if it is in the
 * suspended blocked queue. Additionally, the state value in the moved PCB is updated to reflect its
 * new state.
 * 
 * @param name - the name of the process to resume
 * @return 1 if resumed successfully, 0 otherwise
*/
int resume_pcb(char name[21])
{
	PCB* movedPCB;
	switch(findPCB(name))
	{
		case ReadyQ:
			return 0;
		case SusReadyQ:
			movedPCB = delete_pcb_helper(suspendedReadyQueue, name);
			(*movedPCB).next = NULL;
			(*movedPCB).state = Ready;
			enqueuePCB(readyQueue, movedPCB, 0);
			return 1;
		case BlockedQ:
			return 0;
		case SusBlockedQ:
			movedPCB = delete_pcb_helper(suspendedBlockedQueue, name);
			(*movedPCB).next = NULL;
			(*movedPCB).state = Blocked;
			enqueuePCB(blockedQueue, movedPCB, 1);
			return 1;
		default:
		return 0;
	}
}


/**
 * This function resumes all PCBs in the suspended ready and suspended blocked states
 * 
*/
void resumeall_pcb()
{

	struct PCB* currPCB = (*suspendedReadyQueue).head;
	while (currPCB != NULL) {
		resume_pcb((*currPCB).name);
		currPCB = (*suspendedReadyQueue).head;
	}
	currPCB = (*suspendedBlockedQueue).head;
	while (currPCB != NULL) {
		resume_pcb((*currPCB).name);
		currPCB = (*suspendedBlockedQueue).head;
	}
}


/**
 * This function displays the fields of a single PCB to the screen. This is accomplished by obtaining
 * the PCB with the given name using the getPCB() function in the Queue.c file and printing out each
 * of its fields using the internal_show_pcb() function.
 * 
 * @param name - the name of the PCB to show
*/
void show_pcb(char name[21])
{
	PCB* showPCB;
	switch(findPCB(name))
	{
		case ReadyQ:
			showPCB = getPCB(readyQueue, name);
			break;
		case SusReadyQ:
			showPCB = getPCB(suspendedReadyQueue, name);
			break;
		case BlockedQ:
			showPCB = getPCB(blockedQueue, name);
			break;
		case SusBlockedQ:
			showPCB = getPCB(suspendedBlockedQueue, name);
			break;
		default:
			print("\nA PCB named \"");
			print(name);
			println("\" does not exist");
			return;
	}
	internal_show_pcb(showPCB);
}

/**
 * This function updates the priority of the PCB with the given name, and reinserts it into the queue
 * if it is located in the ready or suspended ready so that its priority will be kept up to date.
 * 
 * @param name - the name of the process which will have its priority updated
 * @param priority - the new priority value to add as a field in the PCB
*/
void priority_pcb(char name[21], int priority)
{
	PCB* priorityPCB;
	switch(findPCB(name))
	{
		case ReadyQ:
			priorityPCB = delete_pcb_helper(readyQueue, name);
			(*priorityPCB).next = NULL;
			(*priorityPCB).prev = NULL;
			(*priorityPCB).priority = priority;
			enqueuePCB(readyQueue, priorityPCB, 0);
			break;
		case SusReadyQ:
			priorityPCB = delete_pcb_helper(suspendedReadyQueue, name);
			(*priorityPCB).next = NULL;
			(*priorityPCB).prev = NULL;
			(*priorityPCB).priority = priority;
			enqueuePCB(suspendedReadyQueue, priorityPCB, 0);
			break;
		case BlockedQ:
			priorityPCB = getPCB(blockedQueue, name);
			(*priorityPCB).priority = priority;
			break;
		case SusBlockedQ:
			priorityPCB = getPCB(suspendedBlockedQueue, name);
			(*priorityPCB).priority = priority;
			break;
		default:
			print("\nA PCB named \"");
			print(name);
			println("\" does not exist");
			return;
	}
	print("\nUpdated priority of \"");
	print(name);
	println("\"");
}

/**
 * This function is used by various other functions in the PCB.c file to find which of the queues the PCB
 * with the input name is located in. Returns an enumerated type representing the queue that the PCB was
 * found in.
 * 
 * @param name - the name of the process to search for in the queues
 * @return an enumerated type representing the queue the PCB was found in (ReadyQ, BlockedQ, SusReadyQ, SusBlockedQ, NoneQ)
*/
enum queue_select findPCB(char name[21])
{
	if(findPCBHelper(readyQueue, name))
		return ReadyQ;
	else if(findPCBHelper(blockedQueue, name))
		return BlockedQ;
	else if(findPCBHelper(suspendedReadyQueue, name))
		return SusReadyQ;
	else if(findPCBHelper(suspendedBlockedQueue, name))
		return SusBlockedQ;
	return NoneQ;
}



/**
 * This method sets up a PCB with default values
 * 
 * @param name - the name of the PCB to allocate
 * @param type - an enum type to discern whether this is a User or System process
 * @param priority - the priority of this PCB from 0-9
 * 
 * @return a pointer to the initialized PCB or NULL if not successful
*/
PCB* setup_pcb(char* name, enum proc_types type, int priority) {
	PCB* newPCB = allocate_pcb();
	if (newPCB == NULL)
		return NULL;

	strcpy((*newPCB).name, name); // provided in the call to this method
	(*newPCB).priority = priority; // provided in the call to this method
	(*newPCB).type = type; // provided in the call to this method
	(*newPCB).state = Ready; // default is ready state
	(*newPCB).next = NULL; // this should be initialized by create_pcb
	(*newPCB).prev = NULL; // this should be initialized by create_pcb

	int i; // set every value in the stack to the null character
	for (i = 0; i < 1024; i++)
		(*newPCB).stack_base[i] = '\0';

	(*newPCB).stack_top = (*newPCB).stack_base + 1024 - sizeof(struct Context); // Added in R3

	return newPCB;
}

/**
 * This method is effectively a toString method for a PCB. Given a reference to a PCB, 
 * 	this method will output the value of each member. Note that this is NOT the show_pcb
 * 	that we are required to implement in the project specs. All the other show_pcb methods 
 * 	can use this as a helper method once they find references to PCBs that need shown.
 * 
 * @param pcb - the PCB to be printed
*/
void internal_show_pcb(PCB* pcb) {
	println("");
	print("Name: ");
	println((*pcb).name);

	print("Class: ");
	switch((*pcb).type) {
		case 0:  println("System");  break;
		case 1:  println("User");    break;
		default: println("Unknown"); break;
	}
	
	print("State: ");
	switch((*pcb).state) {
		case 0:  println("Ready");   break;
		case 1:  println("Running"); break;
		case 2:  println("Blocked"); break;
		case 3:  println("Blocked (Suspended)"); break;
		case 4:  println("Ready (Suspended)"); break;
		default: println("Unknown"); break;
	}

	print("Priority: ");
	char str[12];
	bad_itoa(str, (*pcb).priority);
	println(str);
}
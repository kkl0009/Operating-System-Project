#ifndef PCBCompile
#define PCBCompile


#include <core/serial.h>
#include "../R1/r1functions.h"
#include "../mpx_supt.h"
#include <string.h>
#include "Queue.h"
//#include "../R3/Context.c"

enum proc_types{System, User};
enum state{Ready, Running, Blocked, BlockedSuspended, ReadySuspended};
enum queue_select {ReadyQ, BlockedQ, SusReadyQ, SusBlockedQ, NoneQ};

void allocate_queues();
/**
 * This struct represents a Process Control Block 
*/
typedef struct PCB {
	char name[21]; /// name of process, must be unique, 20 character limit
	int priority;  /// priority of process, must be 0-9
	unsigned char stack_base[1024]; /// reference to the bottom of a 1024 byte length stack, init every char to \0
	unsigned char* stack_top; /// top of the stack, initialize in create pcb
	enum proc_types type; /// type of the process (system or user?)
	enum state state; /// state of the process (ready? running? blocked?)
	struct PCB* next; /// pointer to the next PCB in a queue
	struct PCB* prev; /// pointer to the previous PCB in a queue
} PCB;

/**
 * This method allocates a memory for a PCB with the given name and sets
 * 	the default values for each member of the struct
 * 
 * @param name - the name of the PCB to allocate
*/
struct Queue* getReadyQueue();
struct Queue* getBlockedQueue();

int is_system_proc(char* name);

struct PCB* setup_pcb(char* name, enum proc_types type, int priority);
int free_pcb(PCB* pcb);

PCB* create_pcb(char* name, enum proc_types type, int priority);
void delete_pcb(char* name);
void block_pcb(char* name);
void unblock_pcb(char* name);
int suspend_pcb(char* name);
int resume_pcb(char* name);
void resumeall_pcb();
void priority_pcb(char*name, int priority);
void show_pcb(char* name);
void show_all();
void show_ready();
void show_blocked();
enum queue_select findPCB(char name[21]);

void internal_show_pcb(PCB*);


#endif

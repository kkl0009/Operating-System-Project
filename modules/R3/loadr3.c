#include "loadr3.h"
#include "../R1/r1functions.h"
#include "../R2/PCB.h"
#include "../R2/Queue.h"
#include "procsr3.h"

/**
 * This function is used to load in the 5 processes used for testing R3. It simply calls the function to load the
 * processes in so they are run correctly.
*/
void loadr3()
{
	load_proc("proc1", User, 1, proc1);
	load_proc("proc2", User, 2, proc2);
	load_proc("proc3", User, 3, proc3);
	load_proc("proc4", User, 4, proc4);
	load_proc("proc5", User, 5, proc5);
}

/**
 * This function loads in a process with the given attributes so that it can be run in the system. This function was
 * made specifically for loading in processes for R3, and so the processes will be given CPU time when the "yield"
 * command is run.
 * 
 * @param name - the name to give the process that is loaded in
 * @param type - the type of process to create (either a User or a System process)
 * @param priority - the priority to give to the process that is loaded in
 * @param proc - a pointer to the function to execute as part of the created process
 * 
 * @return the PCB that was created and added to the suspended ready queue
*/
PCB* load_proc(char* name, enum proc_types type, int priority, void* proc)
{
	PCB* testPCB = create_pcb(name, type, priority); // Note: in the slides, it allows the user to specify the size of the stack when creating PCB, should we implement this?
	suspend_pcb(testPCB -> name);
	Context* cp = (Context *)(testPCB -> stack_top);
	memset(cp, 0, sizeof(Context));
	cp -> fs = 0x10;
	cp -> gs = 0x10;
	cp -> ds = 0x10;
	cp -> es = 0x10;
	cp -> cs = 0x8;
	cp -> ebp = (u32int)(testPCB -> stack_base);
	cp -> esp = (u32int)(testPCB -> stack_top);
	cp -> eip = (u32int) proc;
	cp -> eflags = 0x202;

	return testPCB;
}
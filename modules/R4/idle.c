#include "idle.h"

/**
 * This function represents the idle process (written before I realized this was
 * 	already in mpx_supt.c). This process is simply an infinite loop that idles every
 * 	time control is given to it.
*/
void idle_proc() {
	while (1) {
		// println("Idle process executed");
		//klogv("Idle process exec-ed");
		sys_req(IDLE, DEFAULT_DEVICE, NULL, NULL);
	}
}
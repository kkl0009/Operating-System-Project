#include "io_scheduler.h"
#include "../../include/core/serial.h"
#include "../../include/core/io.h"

static IOCB* iocb;

/**
 * Function to initialize the iocb in memory
 * @param e_flag a pointer to the event flag
 * @return returns the pointer to the event flag
*/
int* init_iocb(int* e_flag) {

	iocb = sys_alloc_mem(sizeof(IOCB));
	iocb -> queue = sys_alloc_mem(sizeof(IOQueue));
	iocb->queue->head = NULL;
	iocb->queue->tail = NULL;
	iocb->queue->count = 0;
	iocb->event_flag = e_flag;

	return iocb->event_flag;
}

/**
 * This takes requests from processes using system calls
 * @param op_code the op_code sent by the sys_call
 * @param device_id the device_id sent by the sys_call
 * @param buffer_ptr the pointer to the buffer indicated by the sys_call
 * @param count_ptr the pointer to the count variable indicated by the sys_call
 * @param currPCB the pointer to the PCB that is currently operating
*/
void request_io(int op_code, int device_id, char *buffer_ptr, int *count_ptr, PCB* currPCB) {

	if (!(op_code == READ || op_code == WRITE)) {
    	kpanic("Error: op_code is not valid");
	}


	IORequest* request = make_request(op_code, device_id, buffer_ptr, count_ptr, currPCB);

	if (iocb -> process == NULL) {
		//Service the request now
		write_iocb(iocb, request);
		service_request(iocb);
	} else {
		// Enqueue the request for later
		enqueueIO(iocb->queue, request);
	}
}

/**
 * This function handles the completion of an io request(changes pcb state to unblock, loads next io request from queue, blocks that process)
 * @param iocb IOCB containing completed io
*/
void io_completion(IOCB* iocb) {

	unblock_pcb(iocb -> process -> name);
	iocb -> process = NULL;

	// If there is another request waiting for that device, start it
	if (nextIO(iocb)) {
		service_request(iocb);
	}
}


/**
 * Function to check when IOCB event flag is set. If it is set, return process to ready state, check queue for next request
 */
void check_io() {
	if (iocb == NULL)
	{
		klogv("iocb == NULL");
		return;
	}

	if (*(iocb -> event_flag) != 0 && iocb -> process != NULL) {
		io_completion(iocb);
	}
}

/**
 * Function to act on requests when the device has become free.
 * @param iocb pointer to the IOCB that will have its request serviced by the device
 * 
 */
void service_request(IOCB* iocb) {
	*(iocb -> event_flag) = 0;

	if (iocb->op_code == READ)
	{
		com_read(iocb->buffer_ptr, iocb->count_ptr);
	}
	else 
	{ 
		com_write(iocb->buffer_ptr, iocb->count_ptr);
	}
}
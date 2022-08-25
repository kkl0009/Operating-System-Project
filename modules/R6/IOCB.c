#include "IOCB.h"
#include <core/serial.h>

/**
 * This functions creates a request "block" by making an IO request node. 
 * @param op_code the op_code sent by the sys_call
 * @param device_id the device_id sent by the sys_call
 * @param buffer_ptr the pointer to the buffer indicated by the sys_call
 * @param count_ptr the pointer to the count variable indicated by the sys_call
 * @param currPCB the pointer to the PCB that is currently operating
 * @return pointer to IORequest that was just created
*/
IORequest* make_request(int op_code, int device_id, char *buffer_ptr, int *count_ptr, PCB* currPCB) {

	IORequest* request = sys_alloc_mem(sizeof(IORequest));
	request -> op_code = op_code;
	request -> device_id = device_id;
	request -> buffer_ptr = buffer_ptr;
	request -> count_ptr = count_ptr;
	request -> process = currPCB;

	return request;
}

/**
 * This functions adds an IO request to the queue(FIFO). 
 * @param queue pointer to an IOQueue to be added to
 * @param request pointer to an IOReqest to be enqueued 
*/
void enqueueIO(IOQueue* queue, IORequest* request) {
	if ((*queue).count == 0) {
		(*queue).head = request;
		(*queue).tail = request;
		(*request).next = NULL;
		(*request).prev = NULL;
	} else {
		(*(*queue).tail).next = request;
		(*request).prev = (*queue).tail;
		(*queue).tail = request;
	}

	(*queue).count = (*queue).count + 1;
}

/**
 * This functions removes an IO request from the queue(FIFO). 
 * @param queue a pointer to the IOQueue 
 * @return the IORequest that was removed
*/

IORequest* dequeueIO(IOQueue* queue) {
	IORequest* request;
	if ((*queue).count == 0) {
		kpanic("Attempted to dequeue from empty queue");
	} else {
		queue->count -= 1;
		request = queue->head;
		
		if (queue->head->next != NULL) {
			queue->head = queue->head->next;
			queue->head->prev = NULL;
		}
		
	}
	return request;
}

/**
 * This functions dequeues an IO request from the IO Queue from a passed iocb and stores buffer_ptr, 
 *  count_ptr and process into the passed IOCB buffer_ptr, count_ptr and process values. This effectively records information about transfer.
 * @param iocb a pointer to an IOCB to have it's queue evaluated
 * @return integer representing whether there is another request in the queue (1) or there isn't (0)
*/
int nextIO(IOCB* iocb) {
	if (iocb -> queue -> count > 0) {
		IORequest* request = dequeueIO(iocb -> queue);
		write_iocb(iocb, request);
		sys_free_mem(request);
		return 1;
	} else {
		return 0;
	}
}

/**
 * This functions writes a request to the given iocb, copying all associated data to it
 * @param iocb a pointer to an IOCB that will be modified
 * @param request the request from which data will be copied
*/
void write_iocb(IOCB* iocb, IORequest* request) {
	iocb -> buffer_ptr = request -> buffer_ptr;
	iocb -> count_ptr = request -> count_ptr;
	iocb -> op_code = request -> op_code;
	iocb -> device_id = request -> device_id;
	iocb -> process = request -> process;
}

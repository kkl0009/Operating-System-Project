#include "../R2/PCB.h"

/**
 * This struct represents an IO operation requested by a process
*/
typedef struct IORequest {
	struct PCB* process;
	int device_id;
	int op_code;
	char *buffer_ptr;
	int *count_ptr;

	struct IORequest* next;
	struct IORequest* prev;
} IORequest;



/**
 * This struct acts as a queue of the IORequests for a device
*/
typedef struct IOQueue {
	int count;	
    struct IORequest* head;
	struct IORequest* tail;
} IOQueue;

/**
 * This struct represents a IOCB, used by IO scheduler
*/
typedef struct IOCB {

	int device_id;

	int op_code;

	//event flag (used by interrupt handler)
	int *event_flag;

	char *buffer_ptr;
	int *count_ptr;


	//address of pcb requesting operation
	struct PCB* process;

	struct IOQueue* queue;

	
} IOCB;

IORequest* make_request(int op_code, int device_id, char *buffer_ptr, int *count_ptr, PCB* currPCB);
void enqueueIO(IOQueue* queue, IORequest* request);
IORequest* dequeueIO(IOQueue* queue);
int nextIO(IOCB* iocb);
void write_iocb(IOCB* iocb, IORequest* request);
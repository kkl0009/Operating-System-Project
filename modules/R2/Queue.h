#ifndef QueueCompile
#define QueueCompile



#include "../R1/r1functions.h"
#include "../mpx_supt.h"
#include <string.h>
#include "PCB.h"
#include <core/serial.h>

typedef struct Queue {
	int count;
    struct PCB* head;
	struct PCB* tail;
} Queue;

void printQueue(struct Queue* queue);


void enqueuePCB(Queue* queue, struct PCB* pcb, int flag);

int findPCBHelper(struct Queue* queue, char name[21]);

Queue* allocate_queue();

struct PCB* delete_pcb_helper(struct Queue* queue, char name[21]);

struct PCB* getPCB(struct Queue* queue, char name[21]);

#endif
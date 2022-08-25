#include "../R2/PCB.h"
#include "serial_commands.h"
#include "IOCB.h"

int* init_iocb();
void request_io(int op_code, int device_id, char *buffer_ptr, int *count_ptr, PCB* currPCB);
void io_completion(IOCB* iocb);
void check_io();
void service_request(IOCB* iocb);
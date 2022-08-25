#include <string.h>
#include "../R2/PCB.h"

void loadr3();
PCB* load_proc(char* name, enum proc_types type, int priority, void* proc);
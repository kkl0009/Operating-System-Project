#include <system.h>

enum memory_type {Allocated, Free};

typedef struct CMCB{
	enum memory_type type;
	u32int address;
	u32int size;
	char proc_name[21];
	struct CMCB* next;
	struct CMCB* prev;
} CMCB;

typedef struct LMCB{
	enum memory_type type;
	u32int size;
} LMCB;

void init_heap(u32int size);
u32int allocate_mem(u32int bytes);
int free_mem(void* ipaddr);
void show_cmcbs(enum memory_type the_type);
int is_empty();
void add_cmcb(CMCB* new_cmcb, enum memory_type new_type);
CMCB* remove_cmcb(CMCB* old_cmcb);
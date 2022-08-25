/*
  ----- kmain.c -----

  Description..: Kernel main. The first function called after
      the bootloader. Initialization of hardware, system
      structures, devices, and initial processes happens here.
      
      Initial Kernel -- by Forrest Desjardin, 2013, 
      Modifications by:    Andrew Duncan 2014,  John Jacko 2017
      				Ben Smith 2018, and Alex Wilson 2019
*/

#include <stdint.h>
#include <string.h>
#include <system.h>

#include <core/io.h>
#include <core/serial.h>
#include <core/tables.h>
#include <core/interrupts.h>
#include <mem/heap.h>
#include <mem/paging.h>

#include "modules/R1/comhand.h"
#include "modules/mpx_supt.h"
#include "modules/R3/loadr3.h"
#include "modules/R4/idle.h"
#include "modules/R5/TestR5.h"
#include "modules/R6/serial_commands.h"

#include "modules/R6/io_scheduler.h"
#include "modules/R6/newTestProcs.h"


void kmain(void)
{
   extern uint32_t magic;
   // Uncomment if you want to access the multiboot header
   // extern void *mbd;
   // char *boot_loader_name = (char*)((long*)mbd)[16];

  
   // 0) Initialize Serial I/O 
   // functions to initialize serial I/O can be found in serial.c
   // there are 3 functions to call
   init_serial(COM1);
   set_serial_in(COM1);
   set_serial_out(COM1);

 
   klogv("Starting MPX boot sequence...");
   klogv("Initialized serial I/O on COM1 device...");

   init_heap(50000);
   klogv("Heap created...");

   // 1) Initialize the support software by identifying the current
   //     MPX Module.  This will change with each module.
   // you will need to call mpx_init from the mpx_supt.c
  mpx_init(MEM_MODULE);
   mpx_init(IO_MODULE);
   mpx_init(MODULE_F);

   sys_set_malloc(allocate_mem);
   sys_set_free(free_mem);

   if (!is_empty()) {
      kpanic("Heap is not empty!");
   }
   klogv("Mem module successfully initiated...");
 	
   // 2) Check that the boot was successful and correct when using grub
   // Comment this when booting the kernel directly using QEMU, etc.
   if ( magic != 0x2BADB002 ){
     //kpanic("Boot was not error free. Halting.");
   }
   
   // 3) Descriptor Tables -- tables.c
   //  you will need to initialize the global
   // this keeps track of allocated segments and pages
   klogv("Initializing descriptor tables...");
   init_idt();
   init_gdt();


    // 4)  Interrupt vector table --  tables.c
    // this creates and initializes a default interrupt vector table
    // this function is in tables.c
    init_pic();
    init_irq();
    klogv("Interrupt vector table initialized!");
    
   // 5) Virtual Memory -- paging.c  -- init_paging
   //  this function creates the kernel's heap
   //  from which memory will be allocated when the program calls
   // sys_alloc_mem UNTIL the memory management module  is completed
   // this allocates memory using discrete "pages" of physical memory
   // NOTE:  You will only have about 70000 bytes of dynamic memory
   //
   klogv("Initializing virtual memory...");
   init_paging();

   // 6) Call YOUR command handler -  interface method
   klogv("Transferring control to commhand...");

   static int e_flag = 1;
   init_iocb(&e_flag);
   com_open(&e_flag, 1200);
   
   //startup(); // startup process: splash screen, allocate queues
   allocate_queues();
   load_proc("main", System, 9, run_comhand); // create main proc
   load_proc("idle", System, 0, idle_proc); // create idle proc
   load_proc("secondaryidle", System, 0, idle_proc); // create secondary idle proc
   //load_proc("com_write", User, 5, COMWRITE);
   // load_proc("com_write2", User, 5, COMWRITE);
   resume_pcb("main"); //resume these processes 
   resume_pcb("idle");
   resume_pcb("secondaryidle");
   //resume_pcb("com_write");
   // resume_pcb("com_write2");
   asm volatile ("int $60"); // interrupt 60 to context switch into main

   // 7) System Shutdown on return from your command handler
   klogv("Starting system shutdown procedure...");

   com_close();
   
   /* Shutdown Procedure */
   klogv("Shutdown complete. You may now turn off the machine. (QEMU: C-a x)");
   hlt();
}

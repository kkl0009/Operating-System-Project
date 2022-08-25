#
# Makefile for MPX Modules

AS      = nasm
CC      = i386-elf-gcc
CFLAGS  = -Wall -Wextra -Werror -nostartfiles -nostdlib -nodefaultlibs -ffreestanding -g -c
LD      = i386-elf-ld
LDFLAGS = 
ASFLAGS = -f elf -g

# add new files as you create them.
# I'll start you off with just mpx_supt.o
OBJFILES =\
mpx_supt.o \
R1/comhand.o \
R1/charhand.o \
R1/r1functions.o \
R1/commands.o \
R1/time_commands.o \
R2/PCB.o \
R2/Queue.o \
R3/Context.o \
R3/loadr3.o \
R3/procsr3.o \
R4/idle.o \
R4/alarm.o \
R5/TestR5.o \
R6/DCB.o \
R6/IOCB.o \
R6/io_scheduler.o \
R6/serial_commands.o \
R6/newTestProcs.o

.c.s:
	$(CC) $(CFLAGS) -S -o $@ $<
.s.o:
	$(AS) $(ASFLAGS) -o $@ $<
.c.o:
	$(CC) $(CFLAGS) -I../include -o $@ $<

all: modules.o

modules.o: $(OBJFILES)
	$(LD) -r -o modules.o $(OBJFILES)

clean:
	rm modules.o $(OBJFILES)

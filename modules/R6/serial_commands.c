#include <system.h>

#include <core/serial.h>
#include <core/io.h>
#include <core/tables.h>
#include <string.h>

#include "./serial_commands.h"
#include "DCB.c"
#include "../mpx_supt.h"

#define PIC_COMMAND 0x20
#define PIC_MASK 0x21
#define INIT_REG 0x30
#define IVT_ENTRY 0x24

#define PIC_LEVEL_COM1 4
#define ON 1
#define OFF 0

#define EOI 0x20

static DCB* device;

void first_level_int_handler(void);
void second_level_output_int_handler(void);
void second_level_input_int_handler(void);




static char old_handler;




void set_int(int bit, int on) {
	if (on) {
		outb(COM1 + 1, inb(COM1 + 1) | 1 << bit);
	}
	else {
		outb(COM1 + 1, inb(COM1 + 1) & ~(1 << bit));
	}
}


int com_open(int* e_flag, int baud_rate) {

    //cli();

	device = sys_alloc_mem(sizeof(DCB));

	cli();

	//1) Ensure that the parameters are valid, and that the device is not currently open.

	if(e_flag == NULL)
		return -101;//Invalid e_flag parameter

	if(baud_rate != 110 &&
		baud_rate != 150 &&
		baud_rate != 300 &&
		baud_rate != 600 &&
		baud_rate != 1200 && //Note: This is the recommended baud for this project
		baud_rate != 2400 &&
		baud_rate != 4800 &&
		baud_rate != 9600 &&
		baud_rate != 19200)
		return -102; //Invalid baud_rate divisor
	//TODO: Check whether the port is already open (error -103)
	/* if(device -> open_flag == 1){
	return -103;
	} */

	//2) Initialize the DCB. In particular, this should include indicating that the device is open, saving a copy of the event flag pointer,
	//   and setting the initial device status to idle. In addition, the ring buffer parameters must be initialized.
	device -> open_flag = 1;
	device -> event_flag_ptr = e_flag;
	device -> status_code = Idle;
	
	int i = 0;
	while(i < RING_BUFFER_SIZE)
	{
		device -> ring_buffer[i] = '\0';
		i++;
	}
	device -> input_index = 0;
	device -> count = 0;

	//3) Save the address of the current interrupt handler, and install the new handler in the interrupt vector.

	old_handler = idt_get_gate(IVT_ENTRY);
	idt_set_gate(IVT_ENTRY, (u32int)first_level_int_handler, 0x08, 0x8E);

	//4) Compute the required baud rate divisor
	long baud_rate_div = 115200 / (long) baud_rate;

	// ***SAM'S IMPL*** disable ints
	outb(COM1 + 1, 0x00);

	//5) Store the value 0x80 in the Line Control Register. This allows the first two port addresses to access the Baud Rate Divisor register
	outb(COM1 + 3, 0x80);

	//6) Store the high order and low order bytes of the baud rate divisor into the MSB and LSB registers, respectively.
	outb(COM1 + 1, baud_rate_div >> 8);
	outb(COM1,     baud_rate_div);

	//7) Store the value 0x03 in the Line Control Register. This sets the line characteristics to 8 data bits, 1 stop bit, and no parity. It
	//   also restores normal functioning of the first two ports.
	outb(COM1 + 3, 0x03);


	// ***SAM'S IMPL*** enable FIFO, clear, 14byte threshold
	outb(COM1 + 2, 0b11000111);

	//8) Enable the appropriate level in the PIC mask registers
	// set_int(PIC_LEVEL_COM1, ON);
	outb(0x21, inb(0x21) & ~(1<<PIC_LEVEL_COM1));

	//9) Enable overall serial port interrupts by storing the value 0x08 in the Modem Control register.
	// outb(COM1 + 4, 0x08);

	//10) Enable input ready interrupts only by storing the value 0x01 in the Interrupt Enable register.
	outb(COM1 + 1, 0x01);
	(void) inb(COM1);

	sti();

	return 0;
}

int com_close() {

	//1) Ensure that the port is currently open.
	if(device -> open_flag == 0){
		return -201; //Serial port not open error
	}

	//2) Clear the open indicator in the DCB.
	device -> open_flag = 0;

	//3) Disable the appropriate level in the PIC mask register.
	outb(0x21, inb(0x21) | (1<<PIC_LEVEL_COM1));

	//4) Disable all interrupts in the ACC by loading zero values to the Modem Status register and the Interrupt Enable register.
	char input_value = 0x00;

	outb(COM1 + 6, input_value);
	outb(COM1 + 1, input_value);

	//5) Restore the original saved interrupt vector.
	outb(INIT_REG, old_handler);


	return 0;
}




int com_read(char* buf_p, int* count_p) {

	// char test[12];
	// bad_itoa(test, *count_p);
	// klogv("IN COM READ");
	// klogv(buf_p);
	// klogv(test);

	// 1. Validate the supplied parameters 
	if (buf_p < (char*) 0) {
		return -302; // invalid buffer address
	}
	if (count_p < (int*) 0 || *count_p <= 0) {
		return -303; // invalid count address or count value
	}


	// 2. Ensure the port is open and the status is idle
	if (device->open_flag != 1) {
		return -301; // serial port not open
	}
	if (device->status_code != Idle) {
		return -304; // device is not idle / busy
	}


	// 3. Initialize the input buffer variables and set the status to Reading
	device->input_buffer_ptr       = buf_p;
	device->input_buffer_count_ptr = count_p;
	device->status_code            = Reading;


	// 4. Clear the caller's event flag
	*(device->event_flag_ptr) = 0;


	// 5. Copy characters from ring buffer to requestor's buffer, until the ring buffer
	// is emptied, the requested count has been reached, or a CR (ENTER) code has 
	// been found. The copied characters should, of course, be removed from the ring 
	// buffer. Either input interrupts or all interrupts should be disabled while copying.

	cli();

	device->actual_read_count = 0;
	int CR_detected = 0;

	while ((device->count > 0) 
		&& (device->actual_read_count < *count_p) 
		&& (!CR_detected)) 
	{

		// klogv("IN RING BUFFER STUFF");

		char character_to_add = device->ring_buffer[device->input_index];

		if ('\n' == character_to_add || '\r' == character_to_add) {
			CR_detected = 1;
		}

		*(buf_p + device->actual_read_count*sizeof(char)) = character_to_add;
		device->ring_buffer[device->input_index] = '\0';
		device->count--;

		device->input_index = (device->input_index+1) % RING_BUFFER_SIZE;
		device->actual_read_count++;

	}

	sti();

	// klogv("AFTER RING BUFFER STUFF");
	// klogv(buf_p);


	// 6. If more characters are needed, return. If the block is complete, continue with step 7.

	if (device->actual_read_count < *count_p && !CR_detected) {
		return 0;
	}


	// 7. Reset the DCB status to idle, set the event flag, and return the actual count to the 
	// requestor's variable.
	device->status_code = Idle;
	*(device->event_flag_ptr) = 1;
	*count_p = device->actual_read_count;

	return 0;

}




int com_write(char* buf_p, int* count_p) {

	// 1. Ensure that the input parameters are valid
	if (buf_p < (char*) 0) {
		return -402; // invalid buffer address
	}
	if (count_p < (int*) 0 || *count_p <= 0) {
		return -403; // invalid count address or count value
	}


	// 2. Ensure that the port is currently open and idle
	if (device->open_flag != 1) {
		return -401; // serial port not open
	}
	if (device->status_code != Idle) {
		return -404; // device is not idle / busy
	}

	// 3. Install the buffer pointer and counters in the DCB, and set the current status to writing
	device->output_buffer_ptr       = buf_p;
	device->output_buffer_count_ptr = count_p;
	device->status_code             = Writing;


	// 4. Clear the caller's event flag
	*(device->event_flag_ptr) = 0;

	//cli();

	// 5. Get the first character from the requestor's buffer and store it in the output register
	char first_character = *buf_p;
	outb(COM1, first_character);
	device->actual_written_count = 1;


	// 6. Enable write interrupts by setting bit 1 of the Interrupt Enable Register.
	// This must be done by setting the register to the logical OR of its previous contents and 0x02.
	char reg_content = inb(COM1 + 1);
	char modified_reg_content = reg_content | 0x02;
	outb(COM1 + 1, modified_reg_content);

	//sti();

	return 0; // success code

}








/// Interrupt handlers



void first_level_int_handler() {

	cli();

	// 1. If the port is not open, clear the interrupt and return.
	if (device->open_flag == 0) {
		outb(PIC_COMMAND, EOI);
		return;
	}

	// 2. Read the interrupt ID register to determine the exact cause of the interrupt. Bit 0 
	// must be 0 to indicate the interrupt was actually caused by the serial port. In this case,
	// bits 2 and 1 indicate the specific interrupt type as follows:
	// 00 == modem status, 01 == Output interrupt
	// 10 == input interrupt, 11 = line status

	char reg_value = inb(COM1 + 2);

	// 3. call the appropriate second level handler

	if((reg_value & 0x01) == 0x00) {
		if((reg_value & 0x06) == 0b00000000) {
			inb(COM1 + 6);
		}
		else if((reg_value & 0x06) == 0b00000010) {
			second_level_output_int_handler();
		}
		else if((reg_value & 0x06) == 0b00000100) {
			second_level_input_int_handler();
		}
		else if((reg_value & 0x06) == 0b00000110) {
			inb(COM1 + 5);
		}
	}

	sti();

	// 4. Clear the interrupt by sending EOI to the PIC command register.
	outb(PIC_COMMAND, EOI);

	//int i = 1; i=i; // unclear on why this works, but it avoids page faults
}



// handler goes hand in hand with com_write
void second_level_output_int_handler() {

	// 1. If the current status is not writing, ignore the interrupt and return
	if (device->status_code != Writing) {
		return;
	}


	// 2. Otherwise, if the count has not been exhausted, get the next character from the
	// requestor's output buffer and store it in the output register. Return without 
	// signaling completion.
	int output_buffer_count = *(device->output_buffer_count_ptr);

	if (device->actual_written_count < output_buffer_count) {
		char* next_char_addr = device->output_buffer_ptr + device->actual_written_count * sizeof(char);
		char next_char = *next_char_addr;
		device->actual_written_count++;
		outb(COM1, next_char);
		return;
	}

	// 3. Otherwise, all characters have been transferred. Reset the status to idle. Set the
	// event flag and return the count value. Disable write interrupts by clearing bit 1 in
	// the interrupt enable register.
	device->status_code = Idle;
	*(device->event_flag_ptr) = 1;
	*(device->output_buffer_count_ptr) = device->actual_written_count;

	char int_enable_reg = inb(COM1+1);
	int_enable_reg = int_enable_reg & ~0x02; // ~0x02 == 1111 1101
	outb(COM1+1, int_enable_reg);

}



// handler goes hand in hand with com_read
void second_level_input_int_handler() {

	// 1. Read a character from the input register
	char input = inb(COM1);

	outb(COM1, input);

	// 2. If the current status is not reading, store the character in the ring buffer. If the 
	// buffer is full, discard the character. In either case return to the first level handler. 
	// Do not signal completion.
	if (device->status_code != Reading) {

		if (device->count < RING_BUFFER_SIZE) {

			int index_to_add_at = (device->input_index + device->count) % RING_BUFFER_SIZE;
			device->ring_buffer[index_to_add_at] = input;
			device->count++;

		}

		return;

	}


	// 3. Otherwise, the current status is reading. Store the character in the requestor's
	// input buffer. 

	if(input != '\n' && input != '\r')
	{
		char* next_char_addr = device->input_buffer_ptr + device->actual_read_count * sizeof(char);
		*next_char_addr = input;
		device->actual_read_count++;
	}

	// klogv("INT HANDLER");
	// klogv(next_char_addr);
	// char test[12];
	// bad_itoa(test, device->actual_read_count);
	// klogv(test);

	// 4. If the count is not completed and the character is not CR, return. Do not signal
	// completion
	int character_is_CR = ('\n' == input || '\r' == input);

	if (device->actual_read_count < *(device->input_buffer_count_ptr) && !character_is_CR) {
		return;
	}


	// 5. Otherwise, the transfer has completed. Set the status to idle. Set the event flag and
	// return the requestor's count value. 

	device->status_code = Idle;
	*(device->event_flag_ptr) = 1;
	*(device->input_buffer_count_ptr) = device->actual_read_count;

	// klogv("DEVICE COUNT?");
	// bad_itoa(test, *(device->input_buffer_count_ptr));
	// klogv(test);

}
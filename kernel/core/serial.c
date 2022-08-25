/*
  ----- serial.c -----

  Description..: Contains methods and variables used for
    serial input and output.
*/

#include <stdint.h>
#include <string.h>

#include <core/io.h>
#include <core/serial.h>

#include "../../modules/mpx_supt.h"
#include "../../modules/R1/charhand.h"

#define NO_ERROR 0

// Active devices used for serial I/O
int serial_port_out = 0;
int serial_port_in = 0;

/*
  Procedure..: init_serial
  Description..: Initializes devices for user interaction, logging, ...
*/
int init_serial(int device)
{
  outb(device + 1, 0x00); //disable interrupts
  outb(device + 3, 0x80); //set line control register
  outb(device + 0, 115200/9600); //set bsd least sig bit
  outb(device + 1, 0x00); //brd most significant bit
  outb(device + 3, 0x03); //lock divisor; 8bits, no parity, one stop
  outb(device + 2, 0xC7); //enable fifo, clear, 14byte threshold
  outb(device + 4, 0x0B); //enable interrupts, rts/dsr set
  (void)inb(device);      //read bit to reset port
  return NO_ERROR;
}

/*
  Procedure..: serial_println
  Description..: Writes a message to the active serial output device.
    Appends a newline character.
*/
int serial_println(const char *msg)
{
  int i;
  for(i=0; *(i+msg)!='\0'; i++){
    outb(serial_port_out,*(i+msg));
  }
  outb(serial_port_out,'\r');
  outb(serial_port_out,'\n');  
  return NO_ERROR;
}

/*
  Procedure..: serial_print
  Description..: Writes a message to the active serial output device.
*/
int serial_print(const char *msg)
{
  int i;
  for(i=0; *(i+msg)!='\0'; i++){
    outb(serial_port_out,*(i+msg));
  }
  if (*msg == '\r') outb(serial_port_out,'\n');
  return NO_ERROR;
}

/*int (*student_read)(char *buffer, int *count);
  Procedure..: set_serial_out
  Description..: Sets serial_port_out to the given device address.
    All serial output, such as that from serial_println, will be
    directed to this device.
*/
int set_serial_out(int device)
{
  serial_port_out = device;
  return NO_ERROR;
}

/*
  Procedure..: set_serial_in
  Description..: Sets serial_port_in to the given device address.
    All serial input, such as console input via a virtual machine,
    QEMU/Bochs/etc, will be directed to this device.
*/
int set_serial_in(int device)
{
  serial_port_in = device;
  return NO_ERROR;
}

/**
 * This function executes the polling loop to consume input from the COM1 serial port. When characters
 *  are read in, control is passed to character handling functions in charhand.c
 * 
 * @param buffer - the character buffer to be filled and eventually sent back to comhand.c
 * @param count - the length of the character buffer
*/
int *polling(char *buffer, int *count) { 
  // insert your code to gather keyboard input via the technique of polling.
  // You must validat each key and handle special keys such as delete, back space, and
  // arrow keys
  int cursor_index = 0; 

  // create a char* pointing at "letter" + "\0"
  char letter_ptr[2];
  letter_ptr[1] = '\0';

  while(1) { // loop until break (return character should break this)

    if (inb(COM1+5) & 1) {      // poll the status bit

      letter_ptr[0] = inb(COM1);  // read the character into a char*

      if (letter_ptr[0] == '[') { // special character sequences are typically preceded by a [
        letter_ptr[0] = inb(COM1);
        handle_special_char(letter_ptr, buffer, count, &cursor_index);
        inb(COM1); inb(COM1); inb(COM1); // avoids erroneous input from shift + arrow key
      }
      else
        handle_char(letter_ptr, buffer, count, &cursor_index); // handle the input char
      if ('\n' == letter_ptr[0] || '\r' == letter_ptr[0]) {
      	handle_enter(buffer);
        break;
      }
    }
  }
  return count;
}


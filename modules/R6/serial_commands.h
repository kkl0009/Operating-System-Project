int com_open(int* e_flag, int baud_rate);

int com_close(void);


/**
 * Obtains input characters and loads them into the requestor's buffer.
 * 
 * @param buf_p - a far pointer to the starting address of the buffer to receive input characters
 * @param count_p - the address of an integer count value indicating the number of characters to be read
 * 
 * @return 0 - success code
 * @return -301 - port not open
 * @return -302 - invalid buffer address
 * @return -303 - invalid count address or count value
 * @return -304 - device busy
*/
int com_read(char* buf_p, int* count_p);


/**
 * The com_write function is used to transfer a block of data to the serial port.
 * 
 * @param buf_p - a pointer to the starting address of the block of characters to be written
 * @param count_p - the address of an integer count value indicating the number of characters to be transferred
 * 
 * @return 0 - success code
 * @return -401 - serial port not open
 * @return -402 - invalid buffer address
 * @return -403 - invalid count address or count value
 * @return -404 - device busy
*/
int com_write(char* buf_p, int* count_p);
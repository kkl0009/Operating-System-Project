#define RING_BUFFER_SIZE 200

enum device_status{ Idle, Reading, Writing };

/**
 * This struct represents a Device Control Block, soley managed by a device driver
 * Represents an ongoing 
*/
typedef struct DCB {

	// A flag indicating whether the port is open (1 = open, 0 = closed)
	int open_flag;

	// A pointer to the associated event flag, this is a far pointer to an integer event flag.
	// This flag is set to 0 at the beginning of an operation, and set to 1 to indicate when
	// the operation is complete.
	int* event_flag_ptr;

	// A status code, with possible values idle, reading, and writing
	enum device_status status_code;

	// Addresses and counters associated with the current input buffer
	char* input_buffer_ptr;
	int*  input_buffer_count_ptr;
	int   actual_read_count;

	// Addresses and counters associated with the current output buffer
	char* output_buffer_ptr;
	int*  output_buffer_count_ptr;
	int   actual_written_count;

	// An array to be used as the input ring buffer, with associated input index, output
	// index, and counter
	char ring_buffer[RING_BUFFER_SIZE];
	int  input_index;
	int  output_index; 
	int  count;

	
} DCB;
#include <core/serial.h>
#include <string.h>
#include "../mpx_supt.h"
#include "r1functions.h"

void draw_input(char *buffer, int *cursor_index);
void handle_delete(char *buffer, int *count, int *cursor_index);
void handle_arrow(char *letter, char *buffer, int *count, int *cursor_index);
void handle_backspace(char *buffer, int *count, int *cursor_index);
void handle_enter(char *buffer);
void add_char(char* letter, char *buffer, int *count, int *cursor_index);
void print_history();

#define HISTORY_MAX_SIZE 10
#define BUFFER_SIZE 100

int command_history_index = -1;
int command_history_size = 0;
char command_history[HISTORY_MAX_SIZE][BUFFER_SIZE];

/**
 * This function handles characters A-Za-z0-9, space bar, and backspace. Input is used to update
 * 	the supplied character buffer and count. 
 * 
 * @param letter - the character to process represented as an array of the character followed by \0
 * @param buffer - the character buffer to be filled and later processed by comhand.c
 * @param count - the length of the character buffer
*/
void handle_char(char* letter, char *buffer, int *count, int *cursor_index) {
	if (   ('a' <= *letter && 'z' >= *letter)
		|| ('A' <= *letter && 'Z' >= *letter)
		|| ('0' <= *letter && '9' >= *letter)
		|| (' ' == *letter)
		|| (':' == *letter)
		|| ('/' == *letter)) {

		add_char(letter, buffer, count, cursor_index);
	}
	else if (127 == *letter) {
		handle_backspace(buffer, count, cursor_index);
	}
	draw_input(buffer, cursor_index);
	
}

/**
 * This function handles arrow keys and the delete key. Input is used to update
 * 	the supplied character buffer and count. 
 * 
 * @param letter - the character to process represented as an array of the character followed by \0
 * @param buffer - the character buffer to be filled and later processed by comhand.c
 * @param count - the length of the character buffer
*/
void handle_special_char(char* letter, char *buffer, int *count, int *cursor_index) {
	if ('A' <= *letter && *letter <= 'D') {
		handle_arrow(letter, buffer, count, cursor_index);
	}
	else if (*letter == '3') {
		handle_delete(buffer, count, cursor_index);
	}
	draw_input(buffer, cursor_index);
}


/**
 * This function draws what has been input, and is run every time a key is pressed
 *  while receiving input
 * 
 * @param buffer - the character pointer of the current input buffer
 * @param cursor_index - the pointer to the integer keeping track of the cursor index in the buffer
*/ 
void draw_input(char *buffer, int *cursor_index) {
	// Clear the line
	clearln();

	print("> ");
	// Print the buffer
	print(buffer);

	// Set cursor pos
	set_cursor_pos(cursor_index);
}

/**
 * This function handles the delete key, shifting back all characters after the cursor index by one index
 *  and reducing the size.
 * 
 * @param buffer - the character pointer of the current input buffer
 * @param count - the pointer to the integer keeping track of the length of the buffer that has been used
 * @param cursor_index - the pointer to the integer keeping track of the cursor index in the buffer
*/ 
void handle_delete(char *buffer, int *count, int *cursor_index) {
	if (*cursor_index < *count) {
		int i;
		for (i = *cursor_index; i < *count; i++) {
			*(buffer+(sizeof(char)*(i))) = *(buffer+(sizeof(char)*(i+1)));			
		}
		*count -= 1;
	}
}

/**
 * This function handles the arrow key, increasing or decreasing the cursor index appropriately for side arrows,
 *  and increasing or decreasing the command history index (and updating the buffer) appropriately for the up and down arrows
 * 
 * @param letter - the character pointer of the character representing the last keypress of the user
 * @param buffer - the character pointer of the current input buffer
 * @param count - the pointer to the integer keeping track of the length of the buffer that has been used
 * @param cursor_index - the pointer to the integer keeping track of the cursor index in the buffer
*/ 
void handle_arrow(char *letter, char* buffer, int *count, int *cursor_index) {
	if ('D' == *letter) {
			// handle left arrow
		if (*cursor_index > 0)
			*cursor_index -= 1;
	} else if ('C' == *letter) {
		// handle right arrow
		if (*cursor_index < *count)
			*cursor_index += 1;
	} else if ('A' == *letter) {
		if (command_history_index < command_history_size - 1) {
			command_history_index++;
			strcpy(buffer, command_history[command_history_index]);
			*count = strlen(buffer);
			*cursor_index = *count;
		}
	} else if ('B' == *letter) {
		if (command_history_index > 0) {
			command_history_index--;
			strcpy(buffer, command_history[command_history_index]);
			*count = strlen(buffer);
			*cursor_index = *count;
		}
	}
}

/**
 * This function handles the backspace key, shifting back all characters after the cursor index by one index
 *  and decrementing the count and cursor indexes
 * 
 * @param buffer - the character pointer of the current input buffer
 * @param count - the pointer to the integer keeping track of the length of the buffer that has been used
 * @param cursor_index - the pointer to the integer keeping track of the cursor index in the buffer
*/ 
void handle_backspace(char *buffer, int *count, int *cursor_index) {
	if (*cursor_index > 0) {
		int i;
		for (i = *cursor_index - 1; i < *count; i++) {
			*(buffer+(sizeof(char)*(i))) = *(buffer+(sizeof(char)*(i+1)));			
		}
		*count -= 1;
		*cursor_index -= 1;
	}
}


/**
 * This function handles the enter/return key, committing the last command to the command history if different than the previous.
 *  If this occurs, the command history array is shifted to the right, dropping the oldest command, and filling in the first element
 *  with the last command.
 * 
 * @param buffer - the character pointer of the current input buffer
*/ 
void handle_enter(char *buffer) {
	
	// Set the history index back to -1
	command_history_index = -1;

	// If command is the same as last, return
	if (strcmp(buffer, command_history[0]) == 0) {
		return;
	}

	//Shifts the history array to the right, drops oldest command
	int i;
	for (i = command_history_size; i > 0; i--) {
		strcpy(command_history[i], command_history[i - 1]);
	}

	// Copies the command to index 0
	strcpy(command_history[0], buffer);
	if (command_history_size < HISTORY_MAX_SIZE) {
		command_history_size++;
	}
}


/**
 * This function handles character keys that add to the buffer, including letters, numbers, and some special characters
 * 
 * @param letter - the character pointer of the character representing the last keypress of the user
 * @param buffer - the character pointer of the current input buffer
 * @param count - the pointer to the integer keeping track of the length of the buffer that has been used
 * @param cursor_index - the pointer to the integer keeping track of the cursor index in the buffer
*/ 
void add_char(char* letter, char *buffer, int *count, int *cursor_index) {
	if (*count + 1 < BUFFER_SIZE) {
		int i;
		for (i = *count; i >= *cursor_index; i--) {
			*(buffer+(sizeof(char)*(i + 1))) = *(buffer+(sizeof(char)*(i)));			
		}
		*(buffer+(sizeof(char)*(*cursor_index))) = *letter;
		*count += 1;
		*cursor_index += 1;
	}
}

/**
 * This function handles the printing of the command history in chronological order
 *  when called by the user function "history"
*/
void print_history() {
	println("\n___Command History___");
	int i;
	for (i = command_history_size - 1; i >= 0; i--) {
		print(" ");
		print_int(command_history_size - i);
		print("\t");
		println(command_history[i]);
	}
	
}
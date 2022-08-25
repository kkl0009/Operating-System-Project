#include "../mpx_supt.h"
#include "r1functions.h"

void handle_char(char*, char*, int*, int*);
void handle_special_char(char*, char*, int*, int*);

void draw_input(char *buffer, int *cursor_index);
void handle_delete(char *buffer, int *count, int *cursor_index);
void handle_arrow(char *letter, char *buffer, int *count, int *cursor_index);
void handle_backspace(char *buffer, int *count, int *cursor_index);
void handle_enter(char *buffer);
void add_char(char* letter, char *buffer, int *count, int *cursor_index);
void print_history();
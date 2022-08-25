
#include "../../modules/mpx_supt.h"

void print(char* buffer);
void print_int(int value);
void println(char* buffer);
void printColor(char* buffer);
void changeColor(int new_color);
int getColor();
char* select_color();
void clearln();
void set_cursor_pos(int *cursor_index);

void trim_front(char*);
void get_command(char*, char*);
void advance_pointer(char*);
int are_equal(char*, char*);
int rest_empty(char*);

int is_int(char*);

void trim_back(char*);


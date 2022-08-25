#include <core/serial.h>

#include <system.h>
#include <string.h>
#include "../../modules/mpx_supt.h"

void clearln();
void set_cursor_pos(int *cursor_index);
char* select_color();
void printColor(char* buffer);

#define WHITE "\033[38;2;255;255;255m"
#define RED "\033[38;2;255;0;0m"
#define GREEN "\033[38;2;0;255;0m"
#define BLUE "\033[38;2;0;0;255m"
#define PURPLE "\033[38;2;128;0;128m"
#define YELLOW "\033[38;2;255;255;0m"
#define ORANGE "\033[38;2;255;165;0m"
#define BROWN "\033[38;2;165;42;42m"
#define PINK "\033[38;2;255;192;203m"
#define GRAY "\033[38;2;128;128;128m"

/*
* For colors:
* 0 = white
* 1 = red
* 2 = green
* 3 = blue
* 4 = purple
* 5 = yellow
* 6 = orange
* 7 = brown
* 8 = pink
* 9 = gray
*/
int current_color = 0;



/**
 * This function prints the input string of characters in the current color
 * 
 * @param buffer - a character pointer of a string that is to be displayed
*/
void print(char* buffer) {
    // klogv("PASSED INTO PRINT");
    // klogv(buffer);

    // int i = 0;
    // while(*buffer != '\0')
    // {
    //     if(*(buffer + i + 1) != '\0')
    //         *(buffer + i + 1) = *(buffer + i);
    //     else
    //     {
    //         *(buffer + i + 1) = *(buffer + i);
    //         *(buffer + i + 2) = '\0';
    //         break;
    //     }

    //     i++;
    // }
    // if(*buffer != '\0')
    //     *buffer = ' ';

	//printColor(select_color());
    printColor(buffer);
}


/**
 * This function prints the integer number that is input. It only works with positive integers less than 20 digits long
 * 
 * @param value - an integer that is to be displayed, must be a positive integer with less than 20 digits
*/
void print_int(int value) {
    if (value == 0) {
        print("0");
        return;
    }

    int number = value;
    int i = 19;
    char output[20] = { 0 };
    output[20] = '\0';
    while (number > 0) {
        i--;
        output[i] = (number % 10) + '0';
        number /= 10;
    }
    char *str = output + i * sizeof(char);
    print(str);
}


/**
 * This function prints the input string of characters in the current color, followed by a new line character
 * 
 * @param buffer - a character pointer of a string that is to be displayed
*/
void println(char* buffer) {
	print(buffer);
	print("   \n");
}

/**
 * The printColor(char* buffer) method is used to print text to the screen
 * It is used by most functions used throughout R1 that need to print out text
 * 
 * @param buffer - the string to print to the screen
*/
void printColor(char* buffer){
    char character = '0';
    int len = 0;
    for (len = 0; character != '\0'; len++) {
        character = *(buffer+(sizeof(char) * len));
    }
    len++;

    // klogv("IN PRINT COLOR");
    // klogv(buffer);
    // char test[12];
    // bad_itoa(test, len);
    // klogv(test);

    // klogv("IN PRINT COLOR");
    // klogv(buffer);

    sys_req(WRITE, DEFAULT_DEVICE, buffer, &len);
}

/**
 * This function changes the current_color field in this file to the input integer
 * The new integer represents a new color, as each integer corresponds to a color
 * The integer field will be used to determine which color to print text in
 * 
 * @param new_color - the new color to set
*/
void changeColor(int new_color){
    current_color = new_color;
}

/**
 * This function is used to return the current color that the field is set to
 * It is the accessor method for the current_color field
 * 
 * @return integer value of current_color field
*/
int getColor(){
    return current_color;
}

/**
 * This function is used to determine which escape sequence to run based on the value of the current_color field
 * The escape sequences are stored in constants at the top of the file named according to the color they correspond to
 * It is mostly used for printing text to the screen
 * 
 * @return a string with the escape code for the current color
*/
char* select_color(){
    if(current_color == 0)
        return WHITE;
    else if(current_color == 1)
        return RED;
    else if(current_color == 2)
        return GREEN;
    else if(current_color == 3)
        return BLUE;
    else if(current_color == 4)
        return PURPLE;
    else if(current_color == 5)
        return YELLOW;
    else if(current_color == 6)
        return ORANGE;
    else if(current_color == 7)
        return BROWN;
    else if(current_color == 8)
        return PINK;
    else if(current_color == 9)
        return GRAY;
    return WHITE;
}


/**
 * This function uses escape sequences to clear the line of text
*/
void clearln() {
	print("\033[2K\033[1000D");
}

/**
 * This function uses escape sequences to set the cursor position within the text. It moves
 *  the cursor all the way left, then moves right until it's at the correct index
 * 
 * @param cursor_index - the index of where the cursor is to be moved within the text
*/
void set_cursor_pos(int *cursor_index) {

	print("\033[1000D");
	int i;
	for (i = 0; i < *cursor_index + 2; i++) {
		print("\033[1C");
	}
}


/**
 * This function is used to remove any space characters (' ') from the front of the input string
 * 
 * @param input - the input string to remove the space from
*/
void trim_front(char* input)
{
    int i = 0;
    while(*(input + i) == ' ')
    {
        i++;
    }

    int j = 0;

    while(1)
    {
        *(input + j) = *(input + i);
        if(*(input + j) == '\0')
        {
            break;
        }
        i++, j++;
    }
    j++;
    *(input + j) = '\0';
}



/**
 * This function is used to find the next keyword in the input string
 * The next keyword is defined as the next group of characters of everything that is not a space or \0 character
 * 
 * @param input - the full string to find the keyword from
 * @param command - the keyword that was extracted from the input string
*/
void get_command(char* input, char* command)
{
    int i = 0;
    while(*(input + i) != '\0' && *(input + i) != ' ')
    {
        *(command + i) = *(input + i);
        i++;
    }
    *(command + i) = '\0'; 
}



/**
 * This function is used to remove the next keyword in the input string (up to the next space character)
 * Used for advancing the command parsing to the next keyword
 * 
 * @param input - the string to have the keyword removed from
*/
void advance_pointer(char* input)
{
    int i = 0;
    while(*(input + i) != ' ' && *(input + i) != '\0')
    {
        i++;
    }

    int j = 0;

    while(1)
    {
        *(input + j) = *(input + i);
        if(*(input + i) == '\0')
        {
            break;
        }
        i++, j++;
    }
}



/**
 * This function is used to determine whether two input strings contain the same characters in the same order (are equal)
 * It is used for determining which commands were input in the command handler
 * 
 * @param str1 - the first string to compare
 * @param str2 - the second string to compare
 * @return 1 if strings are equal, 0 otherwise
*/
int are_equal(char* str1, char* str2)
{
    int i = 0;
    while(*(str1 + i) != '\0' && *(str2 + i) != '\0')
    {
        if(*(str1 + i) != *(str2 + i))
            return 0;
        i++;
    }

    if(*(str1 + i) == '\0' && *(str2 + i) == '\0') 
        return 1;
    return 0;
}



/**
 * Determines whether the rest of the input string contains a keyword or not
 * This is done by determining if the rest of the string has any characters that are not a space or \0
 * 
 * @param input - the string to check for keywords in
 * @return 1 if no keywords were found, 0 if a keyword was found
*/
int rest_empty(char* input)
{
	int i = 0;
	while(*(input + i) != '\0')
	{
		if(*(input + i) != ' ')
			return 0;
		i++;
	}
	return 1;
}


/**
 * This method determines whether a string is a valid integer.
 *  It returns 0 if false and 1 if true.
 * 
 * @param string - the string to check
 * 
*/
int is_int(char* string) {
    if (string == NULL)
        return 0;

    char* s = string; // make a different char* so string isn't mutated
    while (*s != '\0') {
        if (*s < '0' || *s > '9')
            return 0;
        s++;
    }
    return 1;
}

/**
 * This function is used to remove extra spaces from the end of a string (char pointer)
 * 
 * @param string - the string to remove the whitespace from
*/
void trim_back(char* string) {
    int last = -1;

    int i = 0;
    while(*(string + i) != '\0')
    {
        if(*(string + i) != ' ')
            last = i;
        i++;
    }
    if(last == -1)
        return;
    *(string + last + 1) = '\0';
}
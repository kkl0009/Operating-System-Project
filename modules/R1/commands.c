#include "../../include/string.h"
#include "../mpx_supt.h"
#include "r1functions.h"
#include "time_commands.h"

/**
 * get_version() functions returns the version of mpx the user is running
 *
 */

void get_version() {

	char vz[2];
	int version = 6;
	tostring(vz, version);

	println("");
	println(vz);

}


/**
 * help() function returns a short explanation of how to use the commands
 *
 * @param command - a command that is a character array that can be run on the AmogOS
 */
		
void help(char command[]){
	println("");
	if(strcmp(command, "version") == 0){
		println("Displays the version of MPX in use"); 
	}
	else if(strcmp(command, "shutdown") == 0){
		println("With confirmation, shuts down the system");
	}
	else if(strcmp(command, "date") == 0){
		println("Date can be used 3 different ways:");
		println(" date - gets and outputs the system date");
		println(" date get - gets and outputs the system date");
		println(" date set [month]/[day]/[year] - sets system date to the specified date");
	}
	else if(strcmp(command, "time") == 0){
		println("Time can be used 3 different ways:");
		println(" time - gets and outputs the system time");
		println(" time get - gets and outputs the system time");
		println(" time set [hour]:[minute]:[second] - sets system time to the specified time");
	}
	else if (strcmp(command, "clear") == 0) {
		println("Clears the screen");
	}
	else if(strcmp(command, "crewmate") == 0) {
		println("Allows you to select a crewmate (changes the color of text)");
	}
	else if (strcmp(command, "history") == 0) {
		println("Displays up to 10 past commands in chronological order");
	}
	else if(strcmp(command, "pcb") == 0){
		println("PCB can be used in several different ways (all names must be 3-20 characters long)");
		println(" pcb - displays all processes to the screen");
		println(" pcb suspend [name] - suspends the PCB with the given name");
		println(" pcb resume [name] - unsuspends the PCB with the given name");
		println(" pcb resumeall - unsuspends all PCBs");
		println(" pcb priority [name] [0-9] - changes the priority of the PCB with the given name to the input number (from 0-9)");
		println(" pcb show [name] - displays the PCB with the given name to the screen");
		println(" pcb all - displays all processes to the screen (same as first command)");
		println(" pcb ready - displays all ready processes to the screen");
		println(" pcb blocked - displays all blocked processes to the screen");
	}
	else if (strcmp(command, "loadr3") == 0) {
		println("Loads the five R3 processes (puts them in a suspended ready state)");
	}
	else if (strcmp(command, "alarm") == 0) {
		println("alarm [hour]:[minute]:[second] [message]");
	}
	else if(strcmp(command, "mem") == 0){
		println("mem commands deal with managing system memory");
		// println("\tmem init [bytes] - initializes the heap, allocating the specified number of bytes");
		// println("\tmem allocate [bytes] - allocates the specified amount of bytes using the first-fit method");
		// println("\tmem free [address] - frees the block of memory at the specified address");
		// println("\tmem isempty - shows whether the heap is empty (contains only free memory)");
		println(" mem showallocated - displays the blocks of allocated memory in the heap");
		println(" mem showfree - displays the blocks of free memory in the heap");
	}
	else{
		print("Command \"");
		print(command);
		println("\" does not exist.");
	}

	
}

/**
 * help_for_help() function returns a short explanation of how to use the help commands
 *
 * 
 */

void help_for_help() {
	println("\nThe help command helps you get information about other commands.");
	println("Currently available commands:");
	println("- time");
	println("- date");
	println("- clear");
	println("- version");
	println("- history");
	println("- shutdown");
	println("- crewmate");
	println("- pcb");
	println("- loadr3");
	println("- alarm");
	println("- mem");
	println("Use \"help [command]\" for more info on a particular command.");
}







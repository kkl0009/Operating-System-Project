#include <string.h>
#include "../../modules/R1/charhand.h"
#include "r1functions.h"
#include "time_commands.h"
#include "commands.h"

/**
 * This functions is effectively the main method of the OS. Control is given to this function
 *  by kmain.c. This function receives input from the polling function on a loop and makes 
 *  decisions about what to do with the commands that are inputted. After determining what 
 *  command was inputted, comhand passes control to commands.c or time_commands.c. Currently, 
 *  some commands are also handled in line. 
*/
void startup(void);
void run_comhand(void);

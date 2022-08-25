#include "comhand.h"
#include "../R2/PCB.h"
#include "../R2/Queue.h"
#include "../R3/loadr3.h"
#include "../R4/alarm.h"
#include "../R5/TestR5.h"

//The following constants describe commands that the user has the option to run
#define VERSION "version"
#define HELP "help"
#define SHUTDOWN "shutdown"
#define YES "yes"
#define NO "no"
#define DATE "date"
#define TIME "time"
#define GET "get"
#define SET "set"
#define HISTORY "history"
#define CLEAR "clear"
#define COLOR "crewmate"
#define WHITE "white"
#define RED "red"
#define GREEN "green"
#define BLUE "blue"
#define PURPLE "purple"
#define YELLOW "yellow"
#define ORANGE "orange"
#define BROWN "brown"
#define PINK "pink"
#define GRAY "gray"

//R2 commands
#define PCB "pcb"
#define SUSPEND_PCB "suspend"
#define RESUME_PCB "resume"
#define RESUMEALL_PCB "resumeall"
#define PRIORITY_PCB "priority"
#define SHOW_PCB "show"
#define SHOWALL_PCB "all"
#define SHOWREADY_PCB "ready"
#define SHOWBLOCKED_PCB "blocked"

//R3 commands
#define LOADR3 "loadr3"

//R4 commands
#define ALARM "alarm"

//R5 commands
#define MEM "mem"
#define SHOW_ALLOCATED "showallocated"
#define SHOW_FREE "showfree"

enum pcb_func {Suspend, Resume, Priority, Show};

#define BUFFER_SIZE 100 ///< Size of the command buffer

void date_logic(char*);
void time_logic(char*);
void pcb_logic(char*);
void pcb_parsing(char*, enum pcb_func);
void mem_logic(char*);

/**
 * The startup function is executed before comhand. It clears the screen, outputs the amogus
 *  guy, gives a welcome message, and hands back control to comhand.
*/
void startup() {
	print("\033[H");
	print("\033[2J");
	println("                   ______  \n\
             ,/lll*******llll-,  \n\
           /lll/:::::::::::::\\ll\\.  \n\
          /ll/:::::::::::::::::\\lll\\  \n\
         /l/|:::::::/lllllllllllllllll\\  \n\
        |ll/|:::::/lll”’’’’’’’’’’’’’’*ll\\  \n\
    ,,--’ll||::::/ll/                 |ll|  \n\
 /llllllll|l|::::|l|                  |ll|  \n\
|l|::::|ll|F|::::|ll\\                 |ll|  \n\
|1|’’’’|ll|F|::::\\lll\\              ./ll/  \n\
|ll|NJJ|ll|F|::::::\\lll,,,,,,,,,,,,/lll/  \n\
|ll|NJJ|ll|F|:::::::::\\llllllllllll/|ll|  \n\
|ll|NJJ|ll|F\\:::::::::::::::::::::::|ll|  \n\
|ll|NJJ|ll|Fs\\::::::::::::::::::::::|ll|  \n\
|ll|NJJ|ll|FFs\\::::::::::::::::::::/|ll|  \n\
|ll|NJJ|ll|FFFs\\::::::::::::::::::/F|ll|  \n\
|ll|NJJ|ll|FFFFFs\\::::::::::::::/FFF|ll|  \n\
|ll|NJJ|ll|FFFFFFFFFFsssssssssFFFFF|ll|  \n\
\\ll\\NJJ|ll|FFFFFFFFFFFFFFFFFFFFFFFF|ll|  \n\
  \\lllllll|FFFFFFFF|llllllllll/FFFF/ll|  \n\
       |ll|FFFFFFFF|ll””ll|FFFFFFF/ll/  \n\
       |ll|FFFFFFFF|ll| |l|FFFFFFF|l|  \n\
       |ll|FFFFFFFF|ll| |ll\\FFFFF/ll|  \n\
        \\ll\\FFFFFFF/ll|  \\lllllllll/  \n\
         \\lllllllllll/");
	println("Welcome to amogOS! Use the \"help\" command to get started.");
}

void shutdown() {
        /*print("\033[H");
        print("\033[2J");
        println("\
*\n\
                                       * \n\
        *                                     * \n\
                        * \n\
                                  * \n\
                                             _   _ \n\
                 *                       *  | |_| | \n\
   *                          *            /      |\\ \n\
          *     You were ejected…          |      | | \n\
                                          /””””\\  | | \n\
                   *                      \\____/  /” \n\
  *                         *               “----”      * \n\
               *                             \n\
                                     *           \n\
         *                      *                      *\n\
\n\
                                         *\n\
    *\n");*/

    Queue* readyQueue = getReadyQueue();
    readyQueue->head = NULL;

	sys_req(EXIT, DEFAULT_DEVICE, NULL, NULL);
}


/**
 * This functions is effectively the main method of the OS. Control is given to this function
 *  by kmain.c. This function receives input from the polling function on a loop and makes 
 *  decisions about what to do with the commands that are inputted. After determining what 
 *  command was inputted, comhand passes control to commands.c or time_commands.c. Currently, 
 *  some commands are also handled in line. 
*/
void run_comhand() {
	/*println("                   ______  \n\
             ,/lll*******llll-,  \n\
           /lll/:::::::::::::\\ll\\.  \n\
          /ll/:::::::::::::::::\\lll\\  \n\
         /l/|:::::::/lllllllllllllllll\\  \n\
        |ll/|:::::/lll”’’’’’’’’’’’’’’*ll\\  \n\
    ,,--’ll||::::/ll/                 |ll|  \n\
 /llllllll|l|::::|l|                  |ll|  \n\
|l|::::|ll|F|::::|ll\\                 |ll|  \n\
|1|’’’’|ll|F|::::\\lll\\              ./ll/  \n\
|ll|NJJ|ll|F|::::::\\lll,,,,,,,,,,,,/lll/  \n\
|ll|NJJ|ll|F|:::::::::\\llllllllllll/|ll|  \n\
|ll|NJJ|ll|F\\:::::::::::::::::::::::|ll|  \n\
|ll|NJJ|ll|Fs\\::::::::::::::::::::::|ll|  \n\
|ll|NJJ|ll|FFs\\::::::::::::::::::::/|ll|  \n\
|ll|NJJ|ll|FFFs\\::::::::::::::::::/F|ll|  \n\
|ll|NJJ|ll|FFFFFs\\::::::::::::::/FFF|ll|  \n\
|ll|NJJ|ll|FFFFFFFFFFsssssssssFFFFF|ll|  \n\
\\ll\\NJJ|ll|FFFFFFFFFFFFFFFFFFFFFFFF|ll|  \n\
  \\lllllll|FFFFFFFF|llllllllll/FFFF/ll|  \n\
       |ll|FFFFFFFF|ll””ll|FFFFFFF/ll/  \n\
       |ll|FFFFFFFF|ll| |l|FFFFFFF|l|  \n\
       |ll|FFFFFFFF|ll| |ll\\FFFFF/ll|  \n\
        \\ll\\FFFFFFF/ll|  \\lllllllll/  \n\
         \\lllllllllll/");*/
	println("Welcome to amogOS! Use the \"help\" command to get started.\n");
	//int l = 8;
	//sys_req(WRITE, DEFAULT_DEVICE, "message", &l);
	
	//startup();
	// create character buffer
	char cmdBuffer[BUFFER_SIZE];
	// char* cmdBuffer;
	int bufferSize;
	int quit = 0;

	while (!quit) {
		// cmdBuffer = "";
		memset(cmdBuffer, '\0', BUFFER_SIZE);
		bufferSize = 100;

		// int i;
		// for(i = 0; i < bufferSize; i++)
		// {
		// 	*(cmdBuffer + i) = '\0';
		// }


		//trim_back(cmdBuffer);

		// print("> ");
		sys_req(READ, DEFAULT_DEVICE, cmdBuffer, &bufferSize);

		// klogv("PASSED BUFF SIZE");
		// char tester[12];
		// bad_itoa(tester, bufferSize);
		// klogv(tester);

		// // trim_front(cmdBuffer);
		// bufferSize--;
		// *(cmdBuffer + bufferSize) = '\0';

		// int j = 0;
		// char *currChar;
		// currChar = cmdBuffer;
		// while(*currChar != '\0')
		// {
		// 	currChar = (cmdBuffer + j);
		// 	klogv(currChar);
		// 	if(*currChar == '\n' || *currChar == '\r')
		// 		klogv("NEWLINE");
		// 	if(*currChar == '\0')
		// 		klogv("NULL");
		// 	if(*currChar ==  ' ')
		// 		klogv("SPACE");

		// 	j++;
		// 	bad_itoa(tester, j);
		// 	klogv(tester);
		// }

		// klogv("HERE");

		// klogv(cmdBuffer);
		// print(cmdBuffer);

	    trim_front(cmdBuffer);

	    char command[100];
	    get_command(cmdBuffer, command);

	    // klogv("COMMAND");
	    // klogv(command);
	    // print(command);

	    if(are_equal(command, VERSION)) {
	    	advance_pointer(cmdBuffer);
	    	if(rest_empty(cmdBuffer))
	        	get_version();
	        else
	        	println("\n Invalid option for version command");
	    }

	    else if(are_equal(command, HELP)) {
	    	advance_pointer(cmdBuffer);
	    	if(!rest_empty(cmdBuffer)) {
	    		trim_front(cmdBuffer);
	        	help(cmdBuffer);
	        }
	        else
	        	help_for_help();
	    }

	    else if(are_equal(command, SHUTDOWN)) {
	        advance_pointer(cmdBuffer);
	        if(rest_empty(cmdBuffer)) {
	        	println("\n Are you sure you want to shut down? (yes/no)");

	        	memset(cmdBuffer, '\0', BUFFER_SIZE);
				bufferSize = 100;
	        	sys_req(READ, DEFAULT_DEVICE, cmdBuffer, &bufferSize);
	        	trim_front(cmdBuffer);

	        	command[0] = '\0';
	    		get_command(cmdBuffer, command);

		    	if(are_equal(command, YES)) {
		    		shutdown();
		        	break;
		    	}
		    	else
		    		println("\n Returning to command prompt...");
		    }
		    else
		    	println("\n Invalid option for shutdown command");
	    }

	    else if(are_equal(command, DATE))
	        date_logic(cmdBuffer);

	    else if(are_equal(command, TIME))
	        time_logic(cmdBuffer);

	    else if(are_equal(command, HISTORY)) {
	        advance_pointer(cmdBuffer);
	        if(rest_empty(cmdBuffer))
	        	print_history();
	        else
	        	println("\n Invalid option for history command");
	    }

	    else if(are_equal(command, CLEAR)) {
	        advance_pointer(cmdBuffer);
	        if(rest_empty(cmdBuffer)) {
	        	print("\033[H");
	        	print("\033[2J");
	        } else {
	        	println("\n Invalid option for clear command");
	        }
	    }

	    else if(are_equal(command, COLOR)) {
	        advance_pointer(cmdBuffer);
	        if(rest_empty(cmdBuffer)) {
	        	int color = getColor();
	        	println("\nSelect a color for your crewmate: ");
	        	changeColor(0);
	        	print(" white, ");
	        	changeColor(1);
	        	print("red, ");
	        	changeColor(2);
	        	print("green, ");
	        	changeColor(3);
	        	print("blue, ");
	        	changeColor(4);
	        	print("purple, ");
	        	changeColor(5);
	        	print("yellow, ");
	        	changeColor(6);
	        	print("orange, ");
	        	changeColor(7);
	        	print("brown, ");
	        	changeColor(8);
	        	print("pink, ");
	        	changeColor(9);
	        	print("gray\n");
	        	changeColor(color);

	        	memset(cmdBuffer, '\0', BUFFER_SIZE);
				bufferSize = 1;
	        	sys_req(READ, DEFAULT_DEVICE, cmdBuffer, &bufferSize);
	        	trim_front(cmdBuffer);

	        	command[0] = '\0';
	    		get_command(cmdBuffer, command);

	    		if(are_equal(command, WHITE)) {
	    			changeColor(0);
		    		println("\nSelected WHITE");
		    	}
		    	else if(are_equal(command, RED)) {
	    			changeColor(1);
		    		println("\nSelected RED");
		    	}
		    	else if(are_equal(command, GREEN)) {
	    			changeColor(2);
		    		println("\nSelected GREEN");
		    	}
		    	else if(are_equal(command, BLUE)) {
	    			changeColor(3);
		    		println("\nSelected BLUE");
		    	}
		    	else if(are_equal(command, PURPLE)) {
	    			changeColor(4);
		    		println("\nSelected PURPLE");
		    	}
		    	else if(are_equal(command, YELLOW)) {
	    			changeColor(5);
		    		println("\nSelected YELLOW");
		    	}
		    	else if(are_equal(command, ORANGE)) {
	    			changeColor(6);
		    		println("\nSelected ORANGE");
		    	}
		    	else if(are_equal(command, BROWN)) {
	    			changeColor(7);
		    		println("\nSelected BROWN");
		    	}
		    	else if(are_equal(command, PINK)) {
	    			changeColor(8);
		    		println("\nSelected PINK");
		    	}
		    	else if(are_equal(command, GRAY)) {
	    			changeColor(9);
		    		println("\nSelected GRAY");
		    	}
		    	else
		    		println("\nReturning to command prompt...");
	        }
	        else
	        	print("\nThe crewmate command does not take any parameters\n");
	    }

	    else if(are_equal(command, PCB))
	        pcb_logic(cmdBuffer);

	    else if (are_equal(command, LOADR3)) {
	    	advance_pointer(cmdBuffer);
	    	println("");
	    	if (rest_empty(cmdBuffer)) {
	    		loadr3();
	    		println(" Loaded and suspended R3 processes");
	    	} else {
	    		println(" Invalid options for loadr3 command");
	    	}
	    }

	    else if (are_equal(command, ALARM)) {
	    	advance_pointer(cmdBuffer);
	    	if (rest_empty(cmdBuffer)) {
	    		println("\n The alarm command takes a time followed by a message");
	    	}
	    	else {
	    		trim_front(cmdBuffer);

	    		char time[100];
				get_command(cmdBuffer, time);

				advance_pointer(cmdBuffer);
				trim_front(cmdBuffer);

				if(rest_empty(cmdBuffer))
					println("\n Please enter a message for the alarm");
				else {

					trim_back(cmdBuffer);

					int i = 0;
					while(*(cmdBuffer + i) != '\0')
					{
						if(i > 29)
						{
							println("\n Messages for alarms cannot exceed 30 characters");
							break;
						}
						i++;
					}

					if(i <= 29)
						setupAlarm(time, cmdBuffer); //cmdBuffer = the message to be displayed
				}
			}
	    }

	    else if(are_equal(command, MEM)) {
	    	mem_logic(cmdBuffer);
	    }

	    else {
	        print("\nBad input: \"");
	        print(cmdBuffer);
	        println("\" not recognized as a valid command");
	    }

		sys_req(IDLE, DEFAULT_DEVICE, NULL, NULL);
	}
}

/**
 * This function is a helper method for run_comhand(). This function receives a command starting
 *  with "date" and determines whether it is a valid "date set" or "date get", then passes control
 *  to setdate_wrapper() or getdate() in time_commands.c.
 * 
 * @param cmdBuffer - a command beginning with "date"
*/
void date_logic(char* cmdBuffer) {
	advance_pointer(cmdBuffer);
    if(rest_empty(cmdBuffer))
    	getdate();
    else {

        trim_front(cmdBuffer);
        char command[100];
		get_command(cmdBuffer, command);

		if(are_equal(command, GET)) {
			advance_pointer(cmdBuffer);
			if(rest_empty(cmdBuffer))
				getdate();
        	else {
        		//println("\nInvalid format for date get command");
        	}
		}

		else if(are_equal(command, SET)) {
			advance_pointer(cmdBuffer);
			if(rest_empty(cmdBuffer))
    			println("\nYou must enter a date to set for this command");
	        else {
	        	trim_front(cmdBuffer);

	        	command[0] = '\0';
				get_command(cmdBuffer, command);

	        	advance_pointer(cmdBuffer);
	        	if(rest_empty(cmdBuffer))
	        		setdate_wrapper(command); //TODO: add command wrapper here]
	        	else
	        		println("\nInvalid format for date set command");
	        }
		}
		else
			println("\nInvalid option for date command");
	}
}

/**
 * This function is a helper method for run_comhand(). This function receives a command starting
 *  with "time" and determines whether it is a valid "time set" or "time get", then passes control
 *  to  settime_wrapper() or gettime() in time_commands.c.
 * 
 * @param cmdBuffer - a command beginning with "time"
*/
void time_logic(char* cmdBuffer) {
	advance_pointer(cmdBuffer);
    if(rest_empty(cmdBuffer))
    	gettime();
    else {
        trim_front(cmdBuffer);

        char command[100];
		get_command(cmdBuffer, command);

		if(are_equal(command, GET)) {
			advance_pointer(cmdBuffer);
			if(rest_empty(cmdBuffer))
				gettime();
        	else
        		println("\nInvalid format for time get command");
		}

		else if(are_equal(command, SET)) {
			advance_pointer(cmdBuffer);
			if(rest_empty(cmdBuffer))
    			println("\nYou must enter a time to set for this command");
	        else {
	        	trim_front(cmdBuffer);

	        	command[0] = '\0';
				get_command(cmdBuffer, command);

	        	advance_pointer(cmdBuffer);
	        	if(rest_empty(cmdBuffer))
	        		settime_wrapper(command); //TODO: time logic here
	        	else
	        		println("\nInvalid format for time get command");
	        }
	    }

	    else 
	    	println("\nInvalid format for the time get command");

	}
}

/**
 * This function is a helper method for run_comhand(). This function receives a command starting
 * with "pcb" and determines whether it is a valid "pcb" command, then passes control to the
 * appropriate functions depending on which option was selected
 * 
 * @param cmdBuffer - a command beginning with "pcb"
*/
void pcb_logic(char* cmdBuffer)
{
	advance_pointer(cmdBuffer);
	if(rest_empty(cmdBuffer))
	{
		show_all();
	}
	else
	{
		trim_front(cmdBuffer);

		char command[100];
		get_command(cmdBuffer, command);

		if(are_equal(command, SUSPEND_PCB))
		{
			pcb_parsing(cmdBuffer, Suspend);
		}
		else if(are_equal(command, RESUME_PCB))
		{
			pcb_parsing(cmdBuffer, Resume);
		} 
		else if(are_equal(command, RESUMEALL_PCB))
		{
			advance_pointer(cmdBuffer);
			if(rest_empty(cmdBuffer))
			{
				resumeall_pcb();
				println("\nAny and all suspended processes have been resumed");
			}
			else
				println("\nInvalid input for PCB command");
		}
		else if(are_equal(command, PRIORITY_PCB))
		{
			pcb_parsing(cmdBuffer, Priority);
		}
		else if(are_equal(command, SHOW_PCB))
		{
			pcb_parsing(cmdBuffer, Show);
		}
		else if(are_equal(command, SHOWALL_PCB))
		{
			advance_pointer(cmdBuffer);
			if(rest_empty(cmdBuffer))
			{
				show_all();
			}
			else
				println("\nInvalid input for PCB command");
		}
		else if(are_equal(command, SHOWREADY_PCB))
		{
			advance_pointer(cmdBuffer);
			if(rest_empty(cmdBuffer))
			{
				show_ready();
			}
			else
				println("\nInvalid input for PCB command");
		}
		else if(are_equal(command, SHOWBLOCKED_PCB))
		{
			advance_pointer(cmdBuffer);
			if(rest_empty(cmdBuffer))
			{
				show_blocked();
			}
			else
				println("\nInvalid input for PCB command");
		}
		else
		{
			println("\nInvalid input for PCB command");
		}
	}
}

/**
 * This function is a helper method for pcb_logic(). This function receives a command string
 * which includes the name of the PCB to perform an operation on, as well as any additional
 * parameters such as priority. Also includes a code to tell the function which operation to
 * perform based on previous string parsing in the pcb_logic() function
 * 
 * @param cmdBuffer - a command beginning with "pcb"
 * @param code - an integer representing which function to perform
*/
void pcb_parsing(char* cmdBuffer, enum pcb_func code)
{
	advance_pointer(cmdBuffer);
	if(rest_empty(cmdBuffer))
	{
		println("\nPlease enter a name for your PCB");
	}
	else
	{
		trim_front(cmdBuffer);

		char command[100];
		get_command(cmdBuffer, command);

		int i = 0;
		while(*(command + i) != '\0')
		{
			if(i > 19)
			{
				println("\nNames for PCBs cannot exceed 20 characters");
				return;
			}
			i++;
		}
		if(i < 3)
		{
			println("\nNames for PCBs must be at least 3 characters");
			return;
		}

		advance_pointer(cmdBuffer);
		if(!rest_empty(cmdBuffer) && code != Priority) {
			println("\nNames for PCBs cannot include spaces");
		}
		else
		{
			if(code == Priority) {
				trim_front(cmdBuffer);

				char command2[100];
				get_command(cmdBuffer, command2);

				int priority = -1;

				if(*command2 >= '0' && *command2 <= '9' && (*(command2 + 1) == ' ' || *(command2 + 1) == '\0'))
				{
					priority = ((int)(*command2)) - 48;
				}

				advance_pointer(cmdBuffer);
				if(!rest_empty(cmdBuffer))
				{
					println("\nIncorrect format for pcb priority command");
					return;
				}

				if(priority == -1)
				{
					println("\nPriority must be a value from 0 to 9");
					return;
				}

				if (is_system_proc(command)) {
					println("\nUser cannot update system processes, this incident will be reported");
					return;
				}
				
				priority_pcb(command, priority);
			}
			else if(code == Suspend) {
				if (is_system_proc(command)) {
					println("\nUser cannot update system processes, this incident will be reported");
					return;
				}

				if(suspend_pcb(command))
				{
					print("\nSuccessfully suspended ");
					println(command);
				}
				else
				{
					print("\nFailed to suspend ");
					println(command);
				}
			}
			else if(code == Resume) {

				if (is_system_proc(command)) {
					println("\nUser cannot update system processes, this incident will be reported");
					return;
				}

				if(resume_pcb(command))
				{
					print("\nSuccessfully resumed ");
					println(command);
				}
				else
				{
					print("\nFailed to resume ");
					println(command);
				}
			}
			else if(code == Show) {
				show_pcb(command);
			}
			else
				print("\nError in pcb_parsing()");//If this command is run, there is an error with the interation of pcb_logic() and pcb_parsing()
		}
	}
}

void mem_logic(char* cmdBuffer) {
	advance_pointer(cmdBuffer);
	if(rest_empty(cmdBuffer))
	{
		println("\nPlease enter a valid memory command");
	}
	else {
		trim_front(cmdBuffer);

		char command[100];
		get_command(cmdBuffer, command);

		if(are_equal(command, SHOW_ALLOCATED))
		{
			show_cmcbs(Allocated);
		}
		else if(are_equal(command, SHOW_FREE))
		{
			show_cmcbs(Free);
		}
		else
		{
			println("\nInvalid input for mem command");
		}
	}
}
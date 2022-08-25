#include <string.h>
#include "alarm.h"
#include "../R2/PCB.h"
#include "../R2/Queue.h"
#include "../../include/string.h"
#include "../R1/r1functions.h"


#include "../../include/string.h"
#include "../R1/time_commands.h"
#include "../../include/core/io.h"

#define TIME_DELIM ":"
#define SECONDS_IN_DAY 86400


/**
 * alarmInfo struct datatype for use in storing alarms. Linkedlist based data structure, with sequential iteration.
 * 
 * 
 * 
*/

typedef struct alarmInfo
{
	// time to trigger alarm (in number of seconds alarm pcb has run)
	int alarmTime;

	char alarmMessage[31];
	struct alarmInfo* next;
} alarmInfo;

int totalTimeSincePCBCreation = 0;
int timeOfLastCheck = -1;
int currentTime;

struct alarmInfo* head = NULL;


/**
 * Returns the total number of seconds passed since midnight
 * 
 * @return returns the total number of seconds passed since midnight 
 * 
*/
int getTotalSeconds() {
	unsigned int hour, minute, second;
	// Get the current time
  	outb(0x70, 0x04);
  	hour = convertFromBCD(inb(0x71));
  	outb(0x70, 0x02);
  	minute = convertFromBCD(inb(0x71));
  	outb(0x70, 0x00);
  	second = convertFromBCD(inb(0x71));
  	return second + (minute * 60) + (hour * 3600);
}

/**
 * Updates the timekeeping variables. The totalTimeSincePCBCreation needs to be accurately updated
 * for alarm triggers. 
 * 
*/
void updateTimekeeping() {
	// Set the timekeeping variables
  	currentTime = getTotalSeconds();
	if (currentTime >= timeOfLastCheck)
		totalTimeSincePCBCreation += (currentTime - timeOfLastCheck);
	else
		totalTimeSincePCBCreation += (SECONDS_IN_DAY - timeOfLastCheck) + currentTime;
	timeOfLastCheck = currentTime;

}


/**
 * Creates a new alarm with the specified time and message. Alarm data is inserted into a linked list of alarm data.
 *  A PCB is created for the alarm process if no other alarms are active.
 * 
 * @param time A time value from 00:00:00 to 23:59:59 
 * @param message A string of characters to be displayed when the alarm is triggered (30 char max)
*/
void setupAlarm(char* time, char message[31])
{
	//Get the hour, minute, and second from the input time
	char* hour = strtok(time, TIME_DELIM);
	char* min = strtok(NULL, TIME_DELIM);
	char* sec = strtok(NULL, TIME_DELIM);
	char* nothing = strtok(NULL, TIME_DELIM);

	//Determine whether the input values are integers
	if (!is_int(hour) || !is_int(min) || !is_int(sec) || nothing != NULL) {
		println("\nInvalid input: Time must consist of 3 valid integers");
		return;
	}

	int hourInt = atoi(hour);
	int minInt = atoi(min);
	int secInt = atoi(sec);

	//Determine whether the input values are within the acceptable bounds of a time
	if (hourInt   < 0 || hourInt   > 23 ||
		minInt < 0 || minInt > 59 ||
		secInt < 0 || secInt > 59) {
		println("\nInvalid input: Time values out of bounds");
		return;
	}

	//Set the values of the current struct
	alarmInfo* info = sys_alloc_mem(sizeof(alarmInfo));

	// Set the timekeeping variables
  	updateTimekeeping();

	if (head == NULL)
		totalTimeSincePCBCreation = 0;

	// Calculate the trigger time
	int alarmTriggerTime = (secInt + (minInt * 60) + (hourInt * 3600));
  	//print_int(totalTimeSincePCBCreation + alarmTriggerTime - currentTime);
	if (getTotalSeconds() > alarmTriggerTime) {
		// If the alarm is set for the following day
		alarmTriggerTime = totalTimeSincePCBCreation + alarmTriggerTime - currentTime + SECONDS_IN_DAY;
	} else {
		// If the alarm is set for the current day
		alarmTriggerTime = totalTimeSincePCBCreation + alarmTriggerTime - currentTime;
	}


	info -> alarmTime = alarmTriggerTime;
	int i = 0;
	while(i < 31)
	{
		info -> alarmMessage[i] = message[i];
		i++;
	}

	//Place the current struct into the list of alarm structs
	alarmInfo* current = head;
	if(head == NULL)
		head = info;
	else
	{
		while(current != NULL)
		{
			if(current -> next == NULL)
			{
				current -> next = info;
				break;
			}
			current = current -> next;
		}
	}
	print("\nAlarm successfully added with message '");
	print(message);
	print("' set for ");
	print_int(alarmTriggerTime - totalTimeSincePCBCreation);
	println(" seconds from now.");


	current = head;
	while(current != NULL)
	{
		current = current -> next;
	}

	Queue* readyQueue = getReadyQueue();
	if(!findPCBHelper(readyQueue, "Alarm"))
	{
		loadAlarm();
	}
}


/**
 * This function runs as a seperate process. It iterates through the alarms to check to see if any are to be triggered.
 *  Triggered alarms are removed. If there are no more alarms, the process terminates.
*/
void checkAlarms() {
	totalTimeSincePCBCreation = 0;
	while(1) {
  		// Set the timekeeping variables
  		updateTimekeeping();

  		// Iterate through alarms
  		alarmInfo* last = NULL;
		alarmInfo* current = head;
		while(current != NULL)
		{
			if (current -> alarmTime <= totalTimeSincePCBCreation) {
				print("\n!!!!!     Alarm triggered: '");
				print(current -> alarmMessage);
				println("'    !!!!!\n");
				print("Current time:");
				gettime();
				
				alarmInfo* nextAlarm = current -> next;
				if (current == head)
					head = nextAlarm;
				last -> next = nextAlarm;

				sys_free_mem(current);

				last = current;
				current = nextAlarm;
			} else {
				last = current;
				current = current -> next;
			}

		}
		if (head == NULL)
			break;
    	//println("All alarms checked");
      	sys_req(IDLE, DEFAULT_DEVICE, NULL, NULL);
   	}
   	totalTimeSincePCBCreation = 0;
   	timeOfLastCheck = -1;
   	//println("Exiting alarm process...");
   	sys_req(EXIT, DEFAULT_DEVICE, NULL, NULL);
   	println("Error exiting alarm process");
}


/** 
 * The checkAlarms process is created.
 * 
*/
void loadAlarm()
{
	PCB* alarmPCB = create_pcb("Alarm", User, 1); // Note: in the slides, it allows the user to specify the size of the stack when creating PCB, should we implement this?
	Context* cp = (Context *)(alarmPCB -> stack_top);
	memset(cp, 0, sizeof(Context));
	cp -> fs = 0x10;
	cp -> gs = 0x10;
	cp -> ds = 0x10;
	cp -> es = 0x10;
	cp -> cs = 0x8;
	cp -> ebp = (u32int)(alarmPCB -> stack_base);
	cp -> esp = (u32int)(alarmPCB -> stack_top);
	cp -> eip = (u32int) checkAlarms;
	cp -> eflags = 0x202;

	//println("\nCreated process \"Alarm\"");
}


#include "../../include/string.h"
#include "time_commands.h"
#include "../../include/core/io.h"
#include "r1functions.h"

#define TIME_DELIM ":"
#define DATE_DELIM "/"

/**
 * function getdate() gets the current date from the Real Time Clock (RTC)
 * 
 */

void getdate(){
	
	outb(0x70,0x09);
	unsigned int yearlast = convertFromBCD(inb(0x71));
	
	outb(0x70,0x32);
	unsigned int yearfirst = convertFromBCD(inb(0x71));
	
	unsigned int year= (yearfirst)*100 + yearlast;
	
	char yr[11];
	bad_itoa(yr, year);

	outb(0x70, 0x08);
	unsigned int month = convertFromBCD(inb(0x71));
	char mnth[3];
	tostring(mnth, month);

	outb(0x70, 0x07);
	unsigned int day = convertFromBCD(inb(0x71));
	char dy[3];
	tostring(dy, day);
	
	println("");
	print(mnth);
	print("/");
	print(dy);
	print("/");
	println(yr);	
}

/**
 * function setdate_wrapper() is a helper function that cleans and parses user input and then calls setdate() to set date.
 * @param char* time - character array representing user's date input
 */

void setdate_wrapper(char* time) {
	println("");

	char* month = strtok(time, DATE_DELIM);
	char* day = strtok(NULL, DATE_DELIM);
	char* year = strtok(NULL, DATE_DELIM);
	char* nothing = strtok(NULL, DATE_DELIM);

	if (!is_int(year) || !is_int(month) || !is_int(day) || nothing != NULL) {
		println("Invalid input: Date must consist of 3 valid integers");
		return;
	}

	setdate(atoi(year), atoi(month), atoi(day));
}

/**
 * function setdate() is a function that sets date in system's RTC. Function is evoked by setdate_wrapper()
 * @param unsigned int year - unsigned int value representing year
 * @param unsigned int month - unsigned int value representing month
 * @param unsigned int day - unsigned int value representing day
 */

void setdate(unsigned int year, unsigned int month, unsigned int day){
	if (    year < 1700 || year > 2200||          
		month < 1 || month > 12 ||
		day   < 1 || day   > 31) {
		println("Invalid input: Date values out of bounds");
		return;
	}

	unsigned int yearfirst = year/100;
	unsigned int yearlast = year % 100; 

	cli();

	outb(0x70, 0x32);
	outb(0x71, convertToBCD(yearfirst));

	outb(0x70, 0x09);
	outb(0x71, convertToBCD(yearlast));

	outb(0x70, 0x08);
	outb(0x71, convertToBCD(month));

	outb(0x70, 0x07);
	outb(0x71, convertToBCD(day));

	sti();
}

/**
 * function settime_wrapper() is a helper function that cleans and parses user input and then calls settime() to set  time
 * @param char* time - character array representing user's time input
 */

void settime_wrapper(char* time) {
	println("");

	char* hour = strtok(time, TIME_DELIM);
	char* min = strtok(NULL, TIME_DELIM);
	char* sec = strtok(NULL, TIME_DELIM);
	char* nothing = strtok(NULL, TIME_DELIM);

	if (!is_int(hour) || !is_int(min) || !is_int(sec) || nothing != NULL) {
		println("Invalid input: Time must consist of 3 valid integers");
		return;
	}

	settime(atoi(hour), atoi(min), atoi(sec));
}

/**
 * function settime() is a function that sets time in system's RTC. Function is evoked by settime_wrapper()
 * @param unsigned int hour - unsigned int value representing hour
 * @param unsigned int minute - unsigned int value representing minute
 * @param unsigned int second - unsigned int value representing second
 */

void settime(int hour, int minute, int second){

	if (hour   < 0 || hour   > 23 ||
		minute < 0 || minute > 59 ||
		second < 0 || second > 59) {
		println("Invalid input: Time values out of bounds");
		return;
	}

	cli();

	outb(0x70,0x04);
	outb(0x71, convertToBCD(hour));

	outb(0x70, 0x02);
	outb(0x71, convertToBCD(minute));

	outb(0x70, 0x00);
	outb(0x71, convertToBCD(second));

	sti();

}

/**
 * function gettime() gets the current time from the Real Time Clock (RTC)
 * 
 */

void gettime(){
	outb(0x70, 0x04);
	unsigned int hour = convertFromBCD(inb(0x71));
	char hr[3];
	tostring(hr, hour);

	outb(0x70, 0x02);
	unsigned int minute = convertFromBCD(inb(0x71));
	char min[3];
	tostring(min, minute);

	outb(0x70, 0x00);
	unsigned int second = convertFromBCD(inb(0x71));
	char sec[3];
	tostring(sec, second);
	
	println("");
	print(hr);
	print(":");
	print(min);
	print(":");
	print(sec);
	println(" ");
	
}
/**
 * function convertFromBCD() converts a Binary Coded Decimal (BCD) value that is stored as an unsigned int to a regular integer(stored as an unsigned int)
 * @param unsigned int- BCD value being converted to a regualr int
 */



unsigned int convertFromBCD(unsigned int x){
	
	unsigned int firstDigit = x >> 4;
	unsigned int secondDigit = x & 0b1111;
	return firstDigit * 10 + secondDigit;

}

/**
 * function convertToBCD() - converts a regular decimal value that is stored as an unsigned int to a BCD integer(stored as an unsigned int)
 * @param unsigned int- decimal value being converted to a BCD.
 */



unsigned int convertToBCD(unsigned int x){
	unsigned int firstDigit= x/10;
	unsigned int secondDigit = x %10;
	return ((firstDigit << 4) | (secondDigit));

}

/**
 * function toString() - turns an integer to a char array[] string
 * @param - char parameter will store the number that is converted to a char array
 @param - int num is the number that is being converted to char array
 */

void tostring(char asString[3], int num) {
    asString[0] = (num/10)%10 + 48;
    asString[1] = num%10 + 48;
    asString[2] = '\0';
}



#include <system.h>
#include <string.h>

/* ************************************ *
 * Author:  Forrest Desjardins    	*
 * Creation Date:  2014           	*
 * 				  	*
 *   DISCLAIMER!!!!!:  		  	*
 *  The following methods are provided	*
 *  for your use as part of the MPX	*
 *  project. While we have tested these	*
 *  We make certification of 	 	*
 *  complete correctness.		*
 *   Additionally not all methods are 	*
 *  implemented :  See:  itoa		*
 * ************************************ */


/*
  Procedure..: strlen
  Description..: Returns the length of a string.
  Params..: s-input string
*/
int strlen(const char *s)
{
  int r1 = 0;
  if (*s) while(*s++) r1++;
  return r1;//return length of string
}

/*
  Procedure..: strcpy
  Description..: Copy one string to another.
  Params..: s1-destination, s2-source
*/
char* strcpy(char *s1, const char *s2)
{
  char *rc = s1;
  while( (*s1++ = *s2++) );
  return rc; // return pointer to destination string
}

/*
  Procedure..: atoi
  Description..: Convert an ASCII string to an integer
  Params..: const char *s -- String
*/
int atoi(const char *s)
{
  int res=0; 
   int charVal=0;
   char sign = ' '; 
   char c = *s;
   

   while(isspace(&c)){ ++s; c = *s;} // advance past whitespace

   
   if (*s == '-' || *s == '+') sign = *(s++); // save the sign
   
   
   while(*s != '\0'){ 
        charVal = *s - 48;
 	res = res * 10 + charVal; 
	s++;

   } 

   
   if ( sign == '-') res=res * -1;

  return res; // return integer
}
/*
  Procedure..: strcmp
  Description..: String comparison
  Params..: s1-string 1, s2-string 2
*/
int strcmp(const char *s1, const char *s2)
{

  // Remarks:
  // 1) If we made it to the end of both strings (i. e. our pointer points to a
  //    '\0' character), the function will return 0
  // 2) If we didn't make it to the end of both strings, the function will
  //    return the difference of the characters at the first index of
  //    indifference.
  while ( (*s1) && (*s1==*s2) ){
    ++s1;
    ++s2;
  }
  return ( *(unsigned char *)s1 - *(unsigned char *)s2 );
}


/* ---------------------------------------
    Functions below this point are given.
    No need to tamper with these!
   --------------------------------------- */

/*
  Procedure..: strcat
  Description..: Concatenate the contents of one string onto another.
  Params..: s1-destination, s2-source
*/
char* strcat(char *s1, const char *s2)
{
  char *rc = s1;
  if (*s1) while(*++s1);
  while( (*s1++ = *s2++) );
  return rc;
}

/*
  Procedure..: isspace
  Description..: Determine if a character is whitespace.
  Params..: c-character to check
*/
int isspace(const char *c)
{
  if (*c == ' '  ||
      *c == '\n' ||
      *c == '\r' ||
      *c == '\f' ||
      *c == '\t' ||
      *c == '\v'){
    return 1;
  }
  return 0;
}

/*
  Procedure..: memset
  Description..: Set a region of memory.
  Params..: s-destination, c-byte to write, n-count
*/
void* memset(void *s, int c, size_t n)
{
  unsigned char *p = (unsigned char *) s;
  while(n--){
    *p++ = (unsigned char) c;
  }
  return s;
}

/*
  Procedure..: strtok
  Description..: Split string into tokens
  Params..: s1-string, s2-delimiter
*/
char* strtok(char *s1, const char *s2)
{
  static char *tok_tmp = NULL;
  const char *p = s2;

  //new string
  if (s1!=NULL){
    tok_tmp = s1;
  }
  //old string cont'd
  else {
    if (tok_tmp==NULL){
      return NULL;
    }
    s1 = tok_tmp;
  }

  //skip leading s2 characters
  while ( *p && *s1 ){
    if (*s1==*p){
      ++s1;
      p = s2;
      continue;
    }
    ++p;
  }

  //no more to parse
  if (!*s1){
    return (tok_tmp = NULL);
  }

  //skip non-s2 characters
  tok_tmp = s1;
  while (*tok_tmp){
    p = s2;
    while (*p){
      if (*tok_tmp==*p++){
	*tok_tmp++ = '\0';
	return s1;
      }
    }
    ++tok_tmp;
  }

  //end of string
  tok_tmp = NULL;
  return s1;
}


/**
 * Bad itoa converts an integer to a string
 * 
 * @param string - a char array limited to be 11 characters in length 
 *    because the max integer in C is 10 characters long
 * 
 * @param num - the number to be converted into a string & placed in 
 *    the string array
*/ 
void bad_itoa(char string[12], int num) {
  int i;
  for (i = 0; i < 11; i++) 
    string[i] = '\0';

  int isNegative = num < 0;
  if (isNegative) {
    string[0] = '-';
    num *= -1;
  }

  int num_of_digits = 0;
  int temp = num;
  do {
    num_of_digits++;
    temp /= 10;
  } while(temp != 0);

  for (i = 0; i < num_of_digits; i++) {
    char c = num%10 + 48;
    string[num_of_digits-i - ((isNegative) ? 0 : 1)] = c;
    num /= 10;
  }
}


/* And finally....
   For the brave ones! (Note: you'll need to add a prototype to string.h)
   sprintf must work properly for the following types to receive extra credit:
     1) characters
     2) strings
     3) signed integers
     4) hexadecimal numbers may be useful
     ...
     \infty) Or feel free to completely implement sprintf
             (Read the man Page: $ man sprintf)
   int sprintf(char *str, const char *format, ...); 
*/



/*
* Name: Gretel Rajamoney
* Date: May 25, 2021
* Assignment: 5
* Program: keygen.c
*
* Sources:
// client.c file that was provided on canvas assignment page
// https://replit.com/@cs344/83clientc?lite=true#client.c
// server.c file that was provided on canvas assignment page
// https://replit.com/@cs344/83serverc?lite=true#server.c
// library containing all socket option functions:
// https://www.gnu.org/software/libc/manual/html_node/Socket_002dLevel-Options.html#Socket_002dLevel-Options
// library containing a list of socket helper functions write(), read(), fork(), memset():
// https://www.tutorialspoint.com/unix_sockets/socket_helper_functions.htm
// linux manual page containing instructions on how to fork:
// https://man7.org/linux/man-pages/man2/fork.2.html
// syntax with how to find the size of a file:
// https://www.geeksforgeeks.org/c-program-find-size-file/
// ascii table:
// http://www.asciitable.com/
// syntax for how to check an array for alphabetic characters:
// https://stackoverflow.com/questions/31010152/function-to-check-for-alphabetic-characters
// syntax for how to read in a text file into a character array:
// https://stackoverflow.com/questions/22697407/reading-text-file-into-char-array/22697886
*/


// imports all necessary libraries
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <time.h>

// constant & global variable definitions
#define ALPHABETSIZE	27


int main(int argc, char* argv[])
{
	// calls the random number generator
	srand(time(NULL));
	char* generated;

	// if there are not two or more arguments return error message
	if (argc != 2)
	{
		printf("you do not have enough arguments");
		printf("\n");
		// exits the program after error message
		exit(1);
	}

	// if there are enough arguments
	else
	{
		int stringlength;
		// converts the string into numbers
		stringlength = atoi(argv[1]);
		generated = (char*)malloc(sizeof(char) * (stringlength + 1));

		static const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

		int x;
		for (x = 0; x < stringlength; x++)
		{
			generated[x] = alphabet[rand() % ALPHABETSIZE];
		}

		generated[stringlength] = 0;

		printf("%s\n", generated);
	}

	fflush(stdout);
	return 0;
}




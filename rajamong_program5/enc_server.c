/* 
* Name: Gretel Rajamoney
* Date: May 25, 2021
* Assignment: 5
* Program: enc_server.c
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
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// constant & global variable definitions
#define ALPHABETSIZE	26
char valid[] = "yes1";


// error function used for reporting issues
// syntax copied from given server.c file
void error(const char* message) 
{
	perror(message);
	exit(1);
}


// encrypts the message using the key
void encryptmessage(char message[], char key[])
{
	// variable to store the encripted characters
	char encrypted;

	int x;
	// runs from the beginning of the index until the ends is reached
	for (x = 0; message[x] != '\n'; x++) 
	{
		// if the character at that input is a space
		if (message[x] != ' ')
		{
			// subtracts the input by the ascii value for A
			message[x] = message[x] - 65;
		}
		// if it is not a space
		else
		{
			// replace the input with a value of 26
			message[x] = ALPHABETSIZE;
		}

		// if the character at that input is a space
		if (key[x] != ' ')
		{
			// subtracts the input by the ascii value for A
			key[x] = key[x] - 65;
		}
		// if it is not a space
		else
		{
			// replace the input with a value of 26
			key[x] = ALPHABETSIZE;
		}

		// adds the value in the index of key with the value of the index of message
		encrypted = key[x] + message[x];
		// uses the modules operater to store the remainder after dividing by 27
		encrypted = encrypted % (ALPHABETSIZE + 1);

		// if the character stored in the index of the temp array is not 26
		if (encrypted != ALPHABETSIZE)
		{
			// adds the temp character value added by the ascii value for A
			// this newly calculated value is stores in the index of message
			message[x] = encrypted + 65;
		}

		// if the character stored in the index of the temp array is 26
		else
		{
			// stores a space character in the index of the message array
			message[x] = ' ';
		}
	}

	// replaces the last value in the array with a null terminator
	message[x] = '\0';
	return;
}


// function to set up the address struct for the server socket
void setupaddressstruct(struct sockaddr_in* address, int portnumber)
{
	// clears out the address struct
	memset((char*)address, '\0', sizeof(*address));

	// ensures that the address is network capable
	address->sin_family = AF_INET;
	// stores the port number
	address->sin_port = htons(portnumber);
	// allow a client at any address to connect to this server
	address->sin_addr.s_addr = INADDR_ANY;
}



int main(int argc, char* argv[])
{
	int connectionsocket;
	char buffer[100000];
	struct sockaddr_in serveraddress;
	struct sockaddr_in clientaddress;
	socklen_t clientinfosize = sizeof(clientaddress);

	// checks if the necessary amount of arguments were provided
	// syntax from server.c file to check usage & arguments
	if (argc < 2)
	{
		// prints out error message if not
		fprintf(stderr, "USAGE: %s port\n", argv[0]);
		exit(1);
	}

	// creates a socket that will listen for connections
	int listensocket = socket(AF_INET, SOCK_STREAM, 0);
	// syntax from server.c file
	if (listensocket < 0)
	{
		error("ERROR opening socket");
	}

	// set up the address struct for the server socket
	// syntax from the server.c file provided on canvas
	setupaddressstruct(&serveraddress, atoi(argv[1]));
	// associated the socket to the port
	if (bind(listensocket, (struct sockaddr*)&serveraddress, sizeof(serveraddress)) < 0)
	{
		error("ERROR on binding");
	}

	// starts listening for connections, up to 5 allowed to queue up
	// syntax from the server.c file provided on canvas
	listen(listensocket, 5);

	// accepts a connection, blocking if one is not available until one connects
	// syntax from the server.c file provided on canvas
	while (1)
	{
		// accepts the connection request which creates a connection socket
		connectionsocket = accept(listensocket, (struct sockaddr*)&clientaddress, &clientinfosize);

		// prints error if it failed to accept the connection request
		if (connectionsocket < 0)
		{
			error("ERROR on accept");
		}

		// creates a new process by duplicating the calling process
		int pid = fork();

		// if the forking process successfully runs
		// this means that the child will make the connection
		if (pid == 0)
		{
			// stores the size of the buffer 
			int buffersize = sizeof(buffer);

			// replaces all values in the buffer with null
			int a;
			for (a = 0; a < buffersize; a++)
			{
				buffer[a] = '\0';
			}

			// reads in the message whether the client is valid or not
			read(connectionsocket, buffer, buffersize - 1);
			// uses strcmp function to check if the client is valid
			int check = strcmp(buffer, valid);

			// if the value in check is 0, the client is valid
			if (check == 0)
			{
				// replies back to the client 
				write(connectionsocket, valid, 5);
			}

			// if the value in check is 1, the client is not valid
			else
			{
				// terminates program
				exit(2);				
			}
			
			// stores the size of the buffer 
			buffersize = sizeof(buffer);

			// replaces all values in the buffer with null
			int b;
			for (b = 0; b < buffersize; b++)
			{
				buffer[b] = '\0';
			}

			// keeps track of where we are in the buffer
			char* placeholder = buffer;
			// character variable for the key
			char* start;
			int linecount, remaining;
			int first = 1;
			int messagelength, messagesize;

			while (1)
			{
				// sets up all of the variables
				if (first == 1)
				{
					// sets the remaining bytes variable to the buffersize
					remaining = buffersize;
					first = first + 1;
					// variable to count the number of new lines
					linecount = 0;
				}
				
				// calculates how many bytes have been read in and stores it
				int completed = read(connectionsocket, placeholder, remaining);

				// if there are zero bytes left, we are done reading them in
				if (completed == 0)
				{
					exit(1);
				}				

				int x;
				// runs for all of the bytes read in
				for (x = 0; x < completed; x++)
				{
					// if there is a new line at the current index
					if (placeholder[x] == '\n')
					{
						// increment the line count variable
						linecount = linecount + 1;

						// if no new lines have been found
						if (linecount != 1)
						{
							continue;	
						}

						// if a new line has been found
						else
						{
							// creates the value for the key  
							start = placeholder + x + 1;
						}
					}
				}

				// moves the placeholder to its new location by added the bytes read in
				placeholder = placeholder + completed;
				// calculates the new remaining bytes left by subtracting the completed bytes
				remaining = remaining - completed;

				// if the linecount is equal to two, we are finished
				if (linecount == 2)
				{
					break;
				}
				// if the linecount is not equal to two, we are not finished
				else
				{
					continue;
				}
			}

			// calculates the length of the message
			messagelength = start - buffer;
			// creates the message variable
			char message[messagelength];
			// calculates the size of the message
			messagesize = sizeof(message);

			// replaces all values in the message with null
			int c;
			for (c = 0; c < messagesize; c++)
			{
				message[c] = '\0';
			}

			// copies the the buffer characters to the message variable
			strncpy(message, buffer, messagelength);
			// encrypts the message by calling encrypting function
			encryptmessage(message, start);
			// sends the encrypted message out
			write(connectionsocket, message, messagesize);
			
		}

		// if the forking process returns -1
		// there was an error and no child process was created
		else if (pid == -1)
		{
			// prints error message since the forking was unsuccessful
			printf("ERROR on forking");
			exit(1);
		}

		else
		{
			// closes the connection socket
			close(connectionsocket);

			// creates variable to terminate process
			int terminate = 0;

			do 
			{
				// suspends execution of the calling process
				waitpid(pid, &terminate, 0);
			} while (!WIFEXITED(terminate) && !WIFSIGNALED(terminate));
		}
	}

	// closes the listening socket
	close(listensocket);
	return 0;
}

/*
* Name: Gretel Rajamoney
* Date: May 25, 2021
* Assignment: 5
* Program: enc_client.c
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
#include <sys/types.h>  // ssize_t
#include <sys/socket.h> // send(),recv()
#include <netdb.h>      // gethostbyname()
#include <netinet/in.h>


// constant & global variable definitions
#define ALPHABETSIZE	26
char valid[] = "yes1";


// error function used for reporting issues
// syntax copied from given server.c file
void error(const char* message) 
{
	perror(message);
	exit(0);
}

int validate(char* filename, int filesize)
{
	// creates a buffer that is the same size as the file
	char buffer[filesize];
	// reads in the file using open function in read mode
	int copy = open(filename, 'r');
	// allowed characters including the new line character
	const char* validcharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
	int x;

	// runs from the begining to the end of the file
	for (x = 0; x < filesize; x++)
	{
		// reads it into the buffer character array
		int check = read(copy, buffer, 1);

		// if the check value is zero, we are at the end of the file
		if (check == 0)
		{
			// exits the function
			exit(1);
		}

		// if the check value is not zero, we are not at the end of the file
		else
		{
			// checks whether it is either a capital letter, space, or new line
			if ((buffer[0] >= 'A' && buffer[0] <= 'Z') || (buffer[0] == ' ') || (buffer[0] == '\n'))
			{
				// if it is, continue because it is valid
				continue;
			}

			// if it is not a valid character
			else
			{
				return 1;
			}
		}
	}

	return 0;
}
// checks whether the file has only valid characters
int filevalidator(char* filename, int filesize)
{
	// creates a buffer that is the same size as the file
	char buffer[filesize];
	// reads in the file using open function in read mode
	int copy = open(filename, 'r');
	// allowed characters including the new line character
	const char* validcharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
	int x;

	// runs from the begining to the end of the file
	for (x = 0; x < filesize; x++)
	{
		// reads it into the buffer character array
		int check = read(copy, buffer, 1);

		// if the check value is zero, we are at the end of the file
		if (check == 0)
		{
			// exits the function
			exit(1);
		}

		// if the check value is not zero, we are not at the end of the file
		else
		{
			// checks whether it is either a capital letter, space, or new line
			if ((buffer[0] >= 'A' && buffer[0] <= 'Z') || (buffer[0] == ' ') || (buffer[0] == '\n'))
			{
				// if it is, continue because it is valid
				continue;
			}

			// if it is not a valid character
			else
			{
				// print out error message and then exit
				fprintf(stderr, "%s contains invalid characters\n", filename);				
				exit(2);
			}
		}	
	}
}


// sends both the text file and the key file to the server
void sendfiletoserver(char* file, int connectionsocket, int filelength)
{
	// creates a buffer character array
	char buffer[100000];
	// stores the size of the buffer into variable
	int buffersize = sizeof(buffer);
	
	// replaces all values in the buffer with null
	int a;
	for (a = 0; a < buffersize; a++)
	{
		buffer[a] = '\0';
	}

	// reads in the file using open function in read mode
	int copy = open(file, 'r');

	int x;
	int remaining;
	
	// runs from the begining to the end of the file
	for (x = 0; x < filelength; x++)
	{
		// stores the total amount of bytes that have been read
		remaining = read(copy, buffer, buffersize);

		// if there are no more bytes left to read, exit loop
		if (remaining == 0)
		{
			exit(1);
		}

		// recalculates the total amount of bytes left to read
		filelength = filelength - remaining;
	}

	// creates a placeholder variable to keep track of buffer location
	char* placeholder = buffer;
	int completed = 0;

	int y;
	
	// runs until there are no more bytes left to write
	for (y = 0; y < remaining; y++)
	{
		// calculates how many more bytes are left to write
		remaining = remaining - completed;
		// moves the placeholder forward in the buffer array
		placeholder = placeholder + completed;
		// calculates how many bytes have been successfully written
		completed = write(connectionsocket, placeholder, remaining);
	}

	// exits the function by returning out
	return;
}


// function to set up the address struct for the server socket
void setupaddressstruct(struct sockaddr_in* address, int portnumber, char* hostname)
{
	// clears out the address struct
	memset((char*)address, '\0', sizeof(*address));

	// ensures that the address is network capable
	address->sin_family = AF_INET;
	// stores the port number
	address->sin_port = htons(portnumber);
	
	// gets the DNS entry for this host name
	struct hostent* hostinfo = gethostbyname(hostname);

	if (hostinfo == NULL) 
	{
		printf("CLIENT: ERROR, no such host\n");
		exit(0);
	}

	// copies the first IP address from the DNS entry to sin_addr.s_addr
	memcpy((char*)&address->sin_addr.s_addr, hostinfo->h_addr_list[0], hostinfo->h_length);
}


int main(int argc, char* argv[])
{
	// checks the usage and arguments
	// syntax copies from client.c file
	if (argc < 3)
	{
		fprintf(stderr, "USAGE: %s hostname port\n", argv[0]);
		exit(0);
	}

	// creates the socket
	// syntax from client.c 
	int connectionsocket = socket(AF_INET, SOCK_STREAM, 0);

	// checks whether the socket was opened correctly
	if (connectionsocket < 0)
	{
		// prints out error and exits if it didnt
		error("CLIENT: ERROR opening socket");
	}

	// stores the hostname into a character array
	char hostname[] = "localhost";
	// creates the server address struct
	struct sockaddr_in serveraddress;
	// sets up the server address struct
	setupaddressstruct(&serveraddress, atoi(argv[3]), hostname);

	// checks whether the socket is connected to the server
	// syntax from client.c file provided on canvas
	if (connect(connectionsocket, (struct sockaddr*)&serveraddress, sizeof(serveraddress)) < 0)
	{
		// prints out error and exits if it didnt connect
		error("CLIENT: ERROR connecting");
	}
	// if the socket connected to the server
	else
	{
		// sends signal to the server program
		write(connectionsocket, valid, 5);
	}

	// creates the buffer character array
	char buffer[100000];
	// stores the size of the buffer into a variable
	int buffersize = sizeof(buffer);
	
	// reads in the response signal from the server program
	read(connectionsocket, buffer, sizeof(buffer));
	// uses strcmp function to check if the client is valid
	int check = strcmp(buffer, valid);

	// if the value in check is 1, unable to connect to the server
	if (check != 0)
	{
		// terminates program
		exit(2);
	}

	// if the value in check is 0, able to connect to the server
	else
	{
		// opens the file at argument one using read mode
		FILE* file1 = fopen(argv[1], "r");
		
		// if the file is equal to NULL, it has not been found
		if (file1 == NULL)
		{
			// prints out error before exiting
			error("ERROR file not found");
		}
		
		// seek to the ends of the file at argument one
		fseek(file1, 0L, SEEK_END);
		// calculates the size of the file and stores it
		long int file1size = ftell(file1);
		// closes the file at argument one
		fclose(file1);

		// calls file validator file to ensure that it only has valid characters
		filevalidator(argv[1], file1size);

		// opens the file at argument two using read mode
		FILE* file2 = fopen(argv[2], "r");

		// if the file is equal to NULL, it has not been found
		if (file2 == NULL)
		{
			// prints out error before exiting
			error("ERROR file not found");
		}

		// seek to the ends of the file at argument two
		fseek(file2, 0L, SEEK_END);
		// calculates the size of the file and stores it
		long int file2size = ftell(file2);
		// closes the file at argument two
		fclose(file2);

		// if the size of the key file is shorter than the message file
		if (file2size <= file1size)
		{
			// print out error before exiting
			printf("ERROR the key is too short\n");
			exit(1);
		}

		// if the size of the key file is equal to or larger than the message file
		else
		{
			// replaces all values in the buffer with null
			int a;
			for (a = 0; a < buffersize; a++)
			{
				buffer[a] = '\0';
			}

			//sends the text file to the server using the socket
			sendfiletoserver(argv[1], connectionsocket, file1size);

			//send the key file to the server using the socket
			sendfiletoserver(argv[2], connectionsocket, file2size);

			//read server response
			int x = read(connectionsocket, buffer, buffersize - 1);

			
			if (x < 0)
			{
				printf("Client: ERROR reading from socket");
				exit(1);
			}


			printf("%s\n", buffer);
			close(connectionsocket);
			return 0;
		}
	}
}
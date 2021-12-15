// NAME: GRETEL RAJAMONEY
// ASSIGNMENT: #3


// RESOURCES BESIDES THE EXPLORATIONS THAT WERE USED
// https://www.gnu.org/software/libc/manual/html_node/Basic-Signal-Handling.html
// https://access.redhat.com/solutions/3226681
// https://www.geeksforgeeks.org/use-fflushstdin-c/
// https://man7.org/linux/man-pages/man3/strdup.3.html#:~:text=The%20strdup()%20function%20returns,copies%20at%20most%20n%20bytes.
// http://www.tutorialspoint.com/c_standard_library/c_function_getenv.htm


// imports all necessary libraries
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <termios.h>
#include <sys/types.h>
#include <stdbool.h>
#include <sys/stat.h>

// creates constant variables
#define FILE_SIZE	256
#define MAX_ARGUMENTS	512
#define MAX_CHARACTERS	2048

// creates global variables
int signChecker = 0;
int firstCharacter = 0;

// function definitions
void readCommands(char*[], char[], char[], int*, int);
int commandFinder(struct sigaction, char[], char[], char* [], int*, int*);
void handleZ(int);
void handleC(int);

// this function is called to execute the command that was inputted by the user
int commandFinder(struct sigaction SIGINT_action, char inputName[], char outputName[], char* stringArray[], int* exitStatus, int* backgroundProcess)
{
	// syntax found from exploration: process api - creating and terminating processes
	pid_t childpid = -3; 

	// if the fork is successful, the value of spawnpid will be 0 in the child the child's pid in the parent (exploration syntax)
	childpid = fork();

	// creates temporary variables to hold function returns
	int input = 0;
	int result = 0;

	if (childpid == 0)
	{
		// accepts ctrl c as a default
		// https://www.gnu.org/software/libc/manual/html_node/Basic-Signal-Handling.html
		SIGINT_action.sa_handler = SIG_DFL;
		sigaction(SIGINT, &SIGINT_action, NULL);

		// handles the inputted name for the input file
		if (strcmp(inputName, ""))
		{
			// opens the file with the inputted name
			input = open(inputName, O_RDONLY);
			
			if (input == -1)
			{
				// if the file can not be opened, error message is printed before exiting
				printf("cannot open %s for input\n", inputName);
				exit(1);
			}

			// duplicated a file descriptor and moves copy of old to new
			result = dup2(input, 0);

			// dup2 returns -1 if it cannot successfully duplicate file descriptor
			if (result == -1)
			{
				// if -1 is returned, then an error message is printed before exiting
				printf("cannot assign %s for input file\n", inputName);
				exit(2);
			}

			// changes the properties of the file that is open
			// FD_CLOEXEC flag is set so that fcntl runs on a file by file basis
			// syntax taken from exploration: processes and i/o
			fcntl(input, F_SETFD, FD_CLOEXEC);
		}

		// creates output variable for the output file
		int output;

		// handles the inputted name for the output file
		if (strcmp(outputName, ""))
		{
			// https://access.redhat.com/solutions/3226681
			// repeatedly open, read, and close separate files in the same opened directory
			output = open(outputName, O_WRONLY | O_CREAT | O_TRUNC, 0666);

			if (output == -1)
			{
				// if output carries the value -1, then the file was not able to open
				printf("cannot open %s for output\n", outputName);
				exit(1);
			}

			// duplicated a file descriptor and moves copy of old to new
			result = dup2(output, 1);

			// dup2 returns -1 if it cannot successfully duplicate file descriptor
			if (result == -1)
			{
				// if -1 is returned, then an error message is printed before exiting
				printf("cannot assign %s for output file\n", outputName);
				exit(2);
			}

			// changes the properties of the file that is open
			// FD_CLOEXEC flag is set so that fcntl runs on a file by file basis
			// syntax taken from exploration: processes and i/o
			fcntl(output, F_SETFD, FD_CLOEXEC);
		}

		// exact syntax for executing a function with an array of strings
		// syntax from exploration: process api - executing a new program
		if (execvp(stringArray[0], (char* const*)stringArray))
		{
			// prints out the necessary error message before exitting
			printf("%s: no such file or directory\n", stringArray[0]);
			// flushes the output buffer and moves the buffered data to the console
			// https://www.geeksforgeeks.org/use-fflushstdin-c/
			fflush(stdout);
			exit(2);
		}
	}

	else
	{
		// runs the process in the background if the signChecker global variable is false
		if (*backgroundProcess && !signChecker)
		{
			// in the parent process waits for the child's termination
			pid_t parentpid = waitpid(childpid, exitStatus, WNOHANG);
			// prints out the childs process id
			printf("background pid is %d\n", childpid);
			// flushes the output buffer and moves the buffered data to the console
			// https://www.geeksforgeeks.org/use-fflushstdin-c/
			fflush(stdout);
		}
		// runs the process not in the background is the signChecker global variable is true
		else
		{
			pid_t parentpid = waitpid(childpid, exitStatus, 0);
		}
	}

	// searches for the terminated background process to print out exit status
	while ((childpid = waitpid(-1, exitStatus, WNOHANG)) > 0)
	{
		printf("child %d terminated\n", childpid);
		
		// if the background process exited by status
		if (WIFEXITED(*exitStatus))
		{
			// prints out the exit value of the terminated background process
			printf("exit value %d\n", WEXITSTATUS(*exitStatus));
		}
		// if the background process was terminated by the signal
		else
		{
			// prints out the terminated signal value of the terminated background process
			printf("terminated by signal %d\n", WTERMSIG(*exitStatus));
		}
		// flushes the output buffer and moves the buffered data to the console
		// https://www.geeksforgeeks.org/use-fflushstdin-c/
		fflush(stdout);
	}
	
}

// if the user inputs ctrl z, this function is called to handle it
void handleZ(int signal) 
{
	int count = 0;

	if (signChecker == 1) 
	{
		// if the sign tracker is set, print out exiting statement
		printf("Exiting foreground-only mode\n");
		// flushes the output buffer and moves the buffered data to the console
		// https://www.geeksforgeeks.org/use-fflushstdin-c/
		fflush(stdout);
		// set the sign tracker back to false
		signChecker = 0;
		count = 1;
	}

	if (signChecker == 0 && count == 0)
	{
		// if the sign tracker is not set, print out entering statement
		printf("\n");
		printf("Entering foreground-only mode (& is not ignored)\n");
		// flushes the output buffer and moves the buffered data to the console
		// https://www.geeksforgeeks.org/use-fflushstdin-c/
		fflush(stdout);
		// set the sign tracker to true
		signChecker = 1;
	}
}

// if the user inputs ctrl c, this function is called to handle it
void handleC(int signal)
{
	// prints out statement of signal when ctrl c is inputted
	printf("\n");
	printf("terminated by signal %d\n", signal);
	// flushes the output buffer and moves the buffered data to the console
	// https://www.geeksforgeeks.org/use-fflushstdin-c/
	fflush(stdout);
}

// this function reads in the users input, and performs function calls accordingly
void readCommands(char* stringArray[], char inputName[], char outputName[], int* backgroundProcess, int pid)
{
	// creates an input array of the max length
	char input[MAX_CHARACTERS];

	// takes in the input
	printf(": ");
	// redirecting input and output from exploration: processes and i/o
	// flushes the output buffer and moves the buffered data to the console
	// https://www.geeksforgeeks.org/use-fflushstdin-c/
	fflush(stdout);
	// syntax from exploration: stdin, stdout, stderr & c i/o library
	// the function fgets reads at most one less than the size of MAX_CHARACTERS into the 
	// buffer pointed to by input, and stops reading if it encounters the end of the file or 
	// a new line, if it reads anew line it stores it in the butter and null terminates the buffer
	fgets(input, MAX_CHARACTERS, stdin);

	// creates variable to represent a new line
	int blank = 0;

	// iterates to read in the whole input string
	for (int i = 0; i < MAX_CHARACTERS && !blank; i++)
	{
		// if there is not a new line the variable is not set
		if (input[i] != '\n')
		{
			blank = 0;
		}
		// if there is a new line the variable is set
		else
		{
			// if a new line is found, the blank variable is set to true
			// the new line at that location is removed
			input[i] = '\0';
			blank = 1;
		}
	}

	// if the input is blank, then a blank is then returned back
	if (!strcmp(input, ""))
	{
		// returns a pointer to the new string which is a duplicate
		// https://man7.org/linux/man-pages/man3/strdup.3.html#:~:text=The%20strdup()%20function%20returns,copies%20at%20most%20n%20bytes.
		stringArray[0] = strdup("");
		return;
	}


	// moves the input into strings and converts them using strtok
	// https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
	const char space[2] = " ";
	char *fileMove = strtok(input, space);

	// handles the characters '>', '<', '&', and '$$'
	for (int x = 0; fileMove; x++)
	{
		// searches for the '>' symbol
		if (!strcmp(fileMove, ">"))
		{
			// this means that it is the output file name
			fileMove = strtok(NULL, space);
			strcpy(outputName, fileMove);
		}
		// searches for the '<' symbol
		else if (!strcmp(fileMove, "<"))
		{
			// this means that it is the input file name
			fileMove = strtok(NULL, space);
			strcpy(inputName, fileMove);
		}
		// searches for the '&' symbol
		else if (!strcmp(fileMove, "&"))
		{
			// this means it is a background process
			// so the variable is set to flag it
			*backgroundProcess = 1;
		}
		// else it is a '$$' symbol & must be replaced with pid
		else
		{
			stringArray[x] = strdup(fileMove);

			// replace all $$ with our pid at the end
			for (int y = 0; stringArray[x][y]; y++)
			{
				if (stringArray[x][y] == '$' && stringArray[x][y + 1] == '$')
				{
					stringArray[x][y] = '\0';
					snprintf(stringArray[x], FILE_SIZE, "%s%d", stringArray[x], pid);
				}
			}
		}

		// moves to the next line of the file
		fileMove = strtok(NULL, space);
	}
}


int main()
{
	// creates variables
	int id = getpid();
	int exitStatus = 0;
	int backgroundProcess = 0;

	// handler code from exploration: signals - concepts and types
	// signal processing code from exploration: signal handling api

	// handler for ctrl z
	struct sigaction sa_sigtstp = { 0 };
	// register catchSIGTSTP as the signal handler
	sa_sigtstp.sa_handler = handleZ;
	// block all catchable signals while catchSIGTSTP is running
	sigfillset(&sa_sigtstp.sa_mask);
	// no flags set
	sa_sigtstp.sa_flags = 0;
	// terminal to stop the process group that is running
	sigaction(SIGTSTP, &sa_sigtstp, NULL);

	// handler for ctrl c
	struct sigaction sa_sigint = { 0 };
	// register SIG_IGN as the signal handler
	sa_sigint.sa_handler = handleC; //SIGCONT test instead
	// block all catchable signals while SIG_IGN is running
	sigfillset(&sa_sigint.sa_mask);
	// no flags set
	sa_sigint.sa_flags = 0;
	// terminal to interrupt the process when the user types it
	sigaction(SIGINT, &sa_sigint, NULL);
	

	// creates two character arrays of the maximum file size 
	// one is for the input file, the other is for the output file
	char inputName[FILE_SIZE] = "";
	char outputName[FILE_SIZE] = "";

	// initializes a null array that is the size of the maximum argument
	char* input[MAX_ARGUMENTS] = { NULL };

	// creates a variable to check whether the while loop should keep running
	bool keepRunning = true;

	while (keepRunning)
	{
		// calls in the readCommands function to read in the users input
		readCommands(input, inputName, outputName, &backgroundProcess, id);

		// if the first inputted character is a null terminator, continue running
		if (input[firstCharacter][firstCharacter] == '\0')
		{
			//continue;
		}

		// if the first inputted character is a '#' symbol, continue running
		else if (input[firstCharacter][firstCharacter] == '#')
		{
			//continue;
		}

		// if the inputted value is the command 'cd', change the directory
		else if (strcmp(input[0], "cd") == 0) {
			// moves into the directory that was requested
			if (input[1]) {
				// checks if the directory can be opened or exists
				int isDirectory = chdir(input[1]);

				// if the directory is unable to open, print out error message
				if (isDirectory == -1) {
					printf("directory not found\n");
					// flushes the output buffer and moves the buffered data to the console
					// https://www.geeksforgeeks.org/use-fflushstdin-c/
					fflush(stdout);
				}
			}

			else {
				// searches for the environment string pointed to by name and returns the
				// associated value to the string, returns NULL is that environment does not exist
				// http://www.tutorialspoint.com/c_standard_library/c_function_getenv.htm
				char* homePath = getenv("HOME");
				chdir(homePath);
			}
		}

		// if the inputted value is the command 'status', return whether it was 
		// exited by status or whether it was a terminated signal value
		else if (strcmp(input[firstCharacter], "status") == 0) 
		{
			// if the background process exited by status
			if (WIFEXITED(exitStatus))
			{
				// prints out the exit value of the terminated background process
				printf("exit value %d\n", WEXITSTATUS(exitStatus));
			}
			// if the background process was terminated by the signal
			else
			{
				// prints out the terminated signal value of the terminated background process
				printf("terminated by signal %d\n", WTERMSIG(exitStatus));
			}
		}

		// if the inputted line is 'exit' the program stops running
		else if (strcmp(input[firstCharacter], "exit") == 0)
		{
			keepRunning = false;
		}

		// default calls the commandFinder function in order to run
		else 
		{
			commandFinder(sa_sigint, inputName, outputName, input, &exitStatus, &backgroundProcess);
		}

		// iterates through the input array until a null element is reached
		for (int i = 0; input[i] != 0; i++) 
		{
			// sets the value stored at that element to null
			input[i] = NULL;
		}

		backgroundProcess = 0;

		// runs in order to reset the variables before the next loop iteration
		if (backgroundProcess == 0)
		{
			// sets the first character of both arrays to a null element
			inputName[firstCharacter] = '\0';
			outputName[firstCharacter] = '\0';
		}
	} 

	return 0;
}














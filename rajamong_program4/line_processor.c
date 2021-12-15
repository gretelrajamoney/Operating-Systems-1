/*
* Name: Gretel Rajamoney
* Program: line_processor.c
* Assignment: Project 4
* Date: 5-11-2021
* Sources: 
* memory allocation for a character pointer:
* https://stackoverflow.com/questions/51657540/memory-allocation-for-char-pointer
* coping data into variables using memset function:
* https://www.educative.io/edpresso/copying-data-using-the-memset-function-in-c
* threads - concepts and api exploration for thread syntax:
* https://oregonstate.instructure.com/courses/1810930/pages/exploration-threads-concepts-and-api?module_item_id=20734171
* synchronization for concurrent execution exploration for lock/unlock syntax:
* https://oregonstate.instructure.com/courses/1810930/pages/exploration-synchronization-for-concurrent-execution?module_item_id=20734173
* condition variables exploration for signaling and waiting syntax:
* https://oregonstate.instructure.com/courses/1810930/pages/exploration-condition-variables?module_item_id=20734177
* concatenating two strings using the strcat function:
* https://www.tutorialspoint.com/c_standard_library/c_function_strcat.htm
* synchronization mechanisms beyond mutex exploration for terminating syntax:
* https://oregonstate.instructure.com/courses/1810930/pages/exploration-synchronization-mechanisms-beyond-mutex?module_item_id=20734175
* copying the number of characters from one memory block to another:
  https://www.educative.io/edpresso/c-copying-data-using-the-memcpy-function-in-c
*/


// imports necessary libraries
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <dirent.h> 
#include <fcntl.h>

// defines constant variables
#define MAXCHARACTERS 80
#define MAXLINES 49
#define MAXINPUT 1000
#define BUFFERSIZE 10
#define INPUTANDBUFFER 1010
#define THREADCOUNT 4
#define SIGNCOUNT 2

// creates global variables 
bool endprogram = false;
bool buffercheck = false;
char space = ' ';

// global function definitions
const char* handleCarrot(char*);
const char* handleSpaces(char*);
void* firstThreadFunction(void*);
void* secondThreadFunction(void*);
void* thirdThreadFunction(void*);
void* fourthThreadFunction(void*);

// buffer 1, shared resource between input thread and line separator thread
char* firstbuffer[BUFFERSIZE];
int firstbuffercount, firstputindex, firstpickindex = 0;
// creates first mutex variable to be initialized in int main
pthread_mutex_t firstmutex;
pthread_cond_t firstemptycond, firstfullcond = PTHREAD_COND_INITIALIZER;

// buffer 2, shared between line separator thread and plus sign thread
char* secondbuffer[BUFFERSIZE];
int secondbuffercount, secondputindex, secondpickindex = 0;
// creates second mutex variable to be initialized in int main
pthread_mutex_t secondmutex;
pthread_cond_t secondemptycond, secondfullcond = PTHREAD_COND_INITIALIZER;

// buffer 3, shared between plus sign thread and output thread
char* thirdbuffer[BUFFERSIZE];
int thirdbuffercount, thirdputindex, thirdpickindex = 0;
// creates third mutex variable to be initialized in int main
pthread_mutex_t thirdmutex; 
pthread_cond_t thirdemptycond, thirdfullcond = PTHREAD_COND_INITIALIZER;


// buffer 4, shared between input thread and output thread
char* fourthbuffer[BUFFERSIZE];
int fourthbuffercount, fourthputindex, fourthpickindex = 0;
// creates fourth mutex variable to be initialized in int main
pthread_mutex_t fourthmutex; 
pthread_cond_t fourthemptycond, fourthfullcond = PTHREAD_COND_INITIALIZER;


// first thread function - also called the input thread 
// reads in lines of characters from the standard input
void* firstThreadFunction(void* args)
{
    // while the buffer is still not finished
    while (buffercheck == false)
    {
        // checks for the end of the stdinput function
        char* finished;
        // variable to store the buffer input with max size of 1000
        // memory allocation for a character pointer
        // https://stackoverflow.com/questions/51657540/memory-allocation-for-char-pointer
        char* input = (char*)malloc(MAXINPUT * sizeof(char)); 
        // variable to store the size of the input
        int inputsize = sizeof(*input);
        // copies the data into the variable using memset()
        // https://www.educative.io/edpresso/copying-data-using-the-memset-function-in-c
        memset((char*) input, '\0', inputsize);

        // synchronization for concurrent execution exploration for lock/unlock syntax
        // locks the inputoutputmutex so that it wont read a line that hasnt been printed yet
        pthread_mutex_lock(&fourthmutex);

        // if the buffer is not full, continue the function
        if (fourthbuffercount != 1)
        {
            // increases the count variable in order to signal a new line
            fourthbuffercount = fourthbuffercount + 1;
        }
        // if the buffer is full, wait for the output thread to signal there is room
        else
        {
            // condition variables exploration for signaling and waiting syntax
            pthread_cond_wait(&fourthemptycond, &fourthmutex);
            // increases the count variable in order to signal a new line
            fourthbuffercount = fourthbuffercount + 1;
        }

        // condition variables exploration for signaling and waiting syntax
        // signals that the variable contains items
        pthread_cond_signal(&fourthfullcond);

        // synchronization for concurrent execution exploration for lock/unlock syntax
        // unlocks the mutex that was just locked above
        pthread_mutex_unlock(&fourthmutex);

        // reads in the input and stores it into the variable
        finished = fgets(input, MAXINPUT, stdin);

 
        // if the end of the input has not been reached
        if (finished != NULL)
        {
            // calculates the amount of bytes that were in the input and stores it into variable
            int inputbytes = strlen(input);
            // adds the input bytes into the overall total bytes variable to keep track

            // if the STOP input has been read in, the input has finished
            if ((strcmp(input, "STOP\n") == 0))
            {
                // sets the buffer checker and program ender functions to true
                buffercheck, endprogram = true;
            }
        }
        // if the end of the file or input has been found
        else
        {
            // sets the buffer checker and program ender functions to true
            buffercheck, endprogram = true;
        }

        // synchronization for concurrent execution exploration for lock/unlock syntax
        // locks the first buffer
        pthread_mutex_lock(&firstmutex);

        // if the buffer is not full, continue the function
        if (firstbuffercount != 1)
        {
            // passes the current line to the buffer
            firstbuffer[firstputindex] = strdup(input);
        }
        // if the buffer is full, wait for the output thread to signal there is room
        else
        {
            // condition variables exploration for signaling and waiting syntax
            pthread_cond_wait(&firstemptycond, &firstmutex);
            // passes the current line to the buffer
            firstbuffer[firstputindex] = strdup(input);
        }

        // increase the count of the buffer
        firstputindex = firstputindex + 1;
        firstbuffercount = firstbuffercount + 1;
        firstputindex = firstputindex % BUFFERSIZE;

        // condition variables exploration for signaling and waiting syntax
        // signals that the buffer is no longer empty
        pthread_cond_signal(&firstfullcond);

        // synchronization for concurrent execution exploration for lock/unlock syntax
        // unlocks the first mutex
        pthread_mutex_unlock(&firstmutex);
        
    }

    // reset global buffer check variable
    buffercheck = false;
    pthread_exit(&firstmutex);
}


// second thread function - also called the line separator thread 
// replaces every line separator in the input by a space
void* secondThreadFunction(void* args)
{
    // while the buffer is still not finished
    while (buffercheck == false)
    {
        // variable to store the buffer input with max size of 1010
        // memory allocation for a character pointer
        // https://stackoverflow.com/questions/51657540/memory-allocation-for-char-pointer
        char* input = (char*)malloc((INPUTANDBUFFER) * sizeof(char));
        // copies the data into the variable using memset()
        // https://www.educative.io/edpresso/copying-data-using-the-memset-function-in-c
        memset(input, '\0', INPUTANDBUFFER); 

        // variable to store the buffer input with max size of 1010
        // memory allocation for a character pointer
        // https://stackoverflow.com/questions/51657540/memory-allocation-for-char-pointer
        char* spacedinput = (char*)malloc((INPUTANDBUFFER) * sizeof(char));
        // copies the data into the variable using memset()
        // https://www.educative.io/edpresso/copying-data-using-the-memset-function-in-c
        memset(spacedinput, '\0', INPUTANDBUFFER);

        // synchronization for concurrent execution exploration for lock/unlock syntax
        // locks the first mutex
        pthread_mutex_lock(&firstmutex);

        // if the buffer is not full, continue the function
        if (firstbuffercount != 0)
        {
            // decreases the count variable
            firstbuffercount = firstbuffercount - 1;

            // if the STOP input has been read in, the input has finished
            if (strcmp(firstbuffer[firstpickindex], "STOP\n") == 0)
            {
                // sets the buffer check variable to true
                buffercheck = true;
            }
        }
        // if the buffer is empty, waits for a signal to proceed again
        else
        {
            // decreases the count variable
            firstbuffercount = firstbuffercount - 1;
            // condition variables exploration for signaling and waiting syntax
            pthread_cond_wait(&firstfullcond, &firstmutex);

            // if the STOP input has been read in, the input has finished
            if (strcmp(firstbuffer[firstpickindex], "STOP\n") == 0)
            {
                // sets the buffer check variable to true
                buffercheck = true;
            }
        }

        // copies the inputted line from the buffer onto the new line
        strcpy(spacedinput, firstbuffer[firstpickindex]);

        // increase the count of the buffer
        firstpickindex = firstpickindex + 1;
        firstpickindex = firstpickindex % BUFFERSIZE;  

        // loops through the input line and replaces all new lines with a space
        strcpy(spacedinput, handleSpaces(spacedinput));

        // condition variables exploration for signaling and waiting syntax
        // signals that the buffer variable contains values
        pthread_cond_signal(&firstemptycond);

        // synchronization for concurrent execution exploration for lock/unlock syntax
        // unlocks the first mutex
        pthread_mutex_unlock(&firstmutex);

        // locks the second mutex
        pthread_mutex_lock(&secondmutex);

        // if the buffer is not full, continue the function
        if (secondbuffercount != 1)
        {
            // moves the second buffer onto a new line
            secondbuffer[secondputindex] = strdup(spacedinput);
            // increments the counts of the second buffer
            secondbuffercount = secondbuffercount + 1;
        }
        // if the buffer is full, wait for the output thread to signal there is room
        else
        {
            // condition variables exploration for signaling and waiting syntax
            pthread_cond_wait(&secondemptycond, &secondmutex);
            // moves the second buffer onto a new line
            secondbuffer[secondputindex] = strdup(spacedinput);
            // increments the counts of the second buffer
            secondbuffercount = secondbuffercount + 1;
        }
        
        // finds the new second producer buffer index
        secondputindex = secondputindex + 1;
        secondputindex = secondputindex % BUFFERSIZE;

        // condition variables exploration for signaling and waiting syntax
        // signals that the buffer contains values
        pthread_cond_signal(&secondfullcond);

        // synchronization for concurrent execution exploration for lock/unlock syntax
        // unlocks the second butter mutex
        pthread_mutex_unlock(&secondmutex);

    }

    // reset global buffer check variable
    buffercheck = false;
    pthread_exit(&secondmutex);
}


// third thread function - also called the plus sign thread 
// replaces every pair of plus signs, i.e., "++", by a "^"
void* thirdThreadFunction(void* args)
{
    // while the buffer is still not finished
    while (buffercheck == false)
    {
        // variable to store the buffer input with max size of 1010
        // memory allocation for a character pointer
        // https://stackoverflow.com/questions/51657540/memory-allocation-for-char-pointer
        char* input = (char*)malloc((INPUTANDBUFFER) * sizeof(char));
        // copies the data into the variable using memset()
        // https://www.educative.io/edpresso/copying-data-using-the-memset-function-in-c
        memset(input, '\0', INPUTANDBUFFER);

        // synchronization for concurrent execution exploration for lock/unlock syntax
        // locks the second buffer
        pthread_mutex_lock(&secondmutex);

        // if the buffer is not full, continue the function
        if (secondbuffercount != 0)
        {
            // moves the second buffer at the index to the input
            strcpy(input, secondbuffer[secondpickindex]);
            // decreases the buffer count variable 
            secondbuffercount = secondbuffercount - 1;
        }
        // if the buffer is full, wait for the output thread to signal there is room
        else
        {
            // decreases the buffer count variable 
            secondbuffercount = secondbuffercount - 1;
            // condition variables exploration for signaling and waiting syntax
            pthread_cond_wait(&secondfullcond, &secondmutex);
            // moves the second buffer at the index to the input
            strcpy(input, secondbuffer[secondpickindex]);
        }

        // if the STOP phrase is found in the input
        if (strcmp(input, "STOP\n") == 0)
        {
            buffercheck = true;
            // finds the second buffer count for the consumer index
            secondpickindex = secondpickindex + 1;
            secondpickindex = secondpickindex % BUFFERSIZE;
        }
        else
        {
            // finds the second buffer count for the consumer index
            secondpickindex = secondpickindex + 1;
            secondpickindex = secondpickindex % BUFFERSIZE;
        }

        // condition variables exploration for signaling and waiting syntax
        // signals that the buffer variable contains values
        pthread_cond_signal(&secondemptycond);

        // synchronization for concurrent execution exploration for lock/unlock syntax
        // unlocks the second mutex
        pthread_mutex_unlock(&secondmutex);


        // variable to store the buffer input with max size of 1010
        // memory allocation for a character pointer
        // https://stackoverflow.com/questions/51657540/memory-allocation-for-char-pointer
        char* containscarrot = (char*)malloc((INPUTANDBUFFER) * sizeof(char)); 
        // copies the data into the variable using memset()
        // https://www.educative.io/edpresso/copying-data-using-the-memset-function-in-c
        memset(containscarrot, '\0', INPUTANDBUFFER); 

        // if there is a carrot in the input, the function is called to handle it
        strcpy(containscarrot, handleCarrot(input));

        // synchronization for concurrent execution exploration for lock/unlock syntax
        pthread_mutex_lock(&thirdmutex);

        // if the buffer is not full, continue the function
        if (thirdbuffercount != 1)
        {
            // increases the third buffer count variable
            thirdbuffercount = thirdbuffercount + 1;
            // puts the input onto the third buffer at the third producer index
            thirdbuffer[thirdputindex] = strdup(containscarrot);
        }
        // if the buffer is full, wait for the output thread to signal there is room
        else
        {
            // increases the third buffer count variable
            thirdbuffercount = thirdbuffercount + 1;
            // condition variables exploration for signaling and waiting syntax
            pthread_cond_wait(&thirdemptycond, &thirdmutex);
            // puts the input onto the third buffer at the third producer index
            thirdbuffer[thirdputindex] = strdup(containscarrot);
        }

        // finds the third buffer count for the producer index
        thirdputindex = thirdputindex + 1;
        thirdputindex = thirdputindex % BUFFERSIZE; 

        // condition variables exploration for signaling and waiting syntax
        pthread_cond_signal(&thirdfullcond);

        // synchronization for concurrent execution exploration for lock/unlock syntax
        pthread_mutex_unlock(&thirdmutex);
    }

    // reset global buffer check variable
    buffercheck = false;
    pthread_exit(&thirdmutex);
}

// fourth thread function - also called the output thread 
//  write this processed data to standard output as lines of exactly 80 characters
void* fourthThreadFunction(void* args)
{
    // variable to store the buffer input with max size of 1010
    // memory allocation for a character pointer
    // https://stackoverflow.com/questions/51657540/memory-allocation-for-char-pointer
    char* input = (char*)malloc((INPUTANDBUFFER) * sizeof(char)); 
    // copies the data into the variable using memset()
    // https://www.educative.io/edpresso/copying-data-using-the-memset-function-in-c
    memset(input, '\0', INPUTANDBUFFER); 

    int linecount = 0;

    // while the buffer is still not finished
    while (buffercheck == false)
    {
        // synchronization for concurrent execution exploration for lock/unlock syntax
        pthread_mutex_lock(&thirdmutex);

        // if the buffer is not full, continue the function
        if (thirdbuffercount != 0)
        {
            // decreases the third buffer count variable
            thirdbuffercount = thirdbuffercount - 1;
            // concatenates the input with the third buffer using strcat
            // https://www.tutorialspoint.com/c_standard_library/c_function_strcat.htm
            strcat(input, thirdbuffer[thirdpickindex]);

            // if the STOP input has been read in, the input has finished
            if (strcmp(thirdbuffer[thirdpickindex], "STOP\n") == 0)
            {
                // sets the buffer check variable to true
                buffercheck = true;
            }
        }
        // if the buffer is full, wait for the output thread to signal there is room
        else
        {
            // decreases the third buffer count variable
            thirdbuffercount = thirdbuffercount - 1;
            // condition variables exploration for signaling and waiting syntax
            pthread_cond_wait(&thirdfullcond, &thirdmutex);
            // concatenates the input with the third buffer using strcat
            // https://www.tutorialspoint.com/c_standard_library/c_function_strcat.htm
            strcat(input, thirdbuffer[thirdpickindex]);

            // if the STOP input has been read in, the input has finished
            if (strcmp(thirdbuffer[thirdpickindex], "STOP\n") == 0)
            {
                // sets the buffer check variable to true
                buffercheck = true;
            }
        }

        // calculates and stores the total bytes of the input into variable
        int bufferlength = strlen(thirdbuffer[thirdpickindex]);
        // subtracts this outputted byte length from the total byte count variable

        // calculates the new third consumer index
        thirdpickindex = thirdpickindex + 1;
        thirdpickindex = thirdpickindex % BUFFERSIZE; 

        // unlocks the third mutex
        pthread_mutex_unlock(&thirdmutex);

        // condition variables exploration for signaling and waiting syntax
        // signals that the buffer has some space on it
        pthread_cond_signal(&thirdemptycond);

        // locks the input and output mutex until a line can be read in
        pthread_mutex_lock(&fourthmutex);

        // if the buffer is not full, continue the function
        if (fourthbuffercount != 0)
        {
            // decreases the fourth buffer count
            fourthbuffercount = fourthbuffercount - 1;
        }
        // if the buffer is full, wait for the output thread to signal there is room
        else
        {
            // decreases the fourth buffer count
            fourthbuffercount = fourthbuffercount - 1;
            // condition variables exploration for signaling and waiting syntax
            pthread_cond_wait(&fourthfullcond, &fourthmutex);
        }

        int writtencount;
        // stores the length of the input into a variable
        int inputlength = strlen(input);

        // runs until the length of the input is less than 80
        while (inputlength >= MAXCHARACTERS)
        {
            writtencount = inputlength / MAXCHARACTERS;

            int x;
            int outputsize;

            // uses a postfix forloop to increment x prior to returning
            for (x = linecount; x < writtencount; ++x)
            {
                char output[MAXCHARACTERS + 1];
                // copies the data into the variable using memset()
                // https://www.educative.io/edpresso/copying-data-using-the-memset-function-in-c
                outputsize = sizeof(output);
                memset(output, '\0', outputsize);

                int y; 
                // creates an variable to store the index of the input array
                int index;

                // uses a postfix forloop to increment x prior to returning
                for (y = 0; y < MAXCHARACTERS; ++y)
                {
                    // calculates the index location
                    index = x * MAXCHARACTERS;
                    // stores values from input array at index into output array
                    output[y] = input[index + y];
                }

                // prints out the inputted line into the file
                printf("%s\n", output);
            }

            // cancels the input length variable to end the loop
            inputlength = 0;
            // updates the line count to the written count variable
            linecount = writtencount;
        }

        // condition variables exploration for signaling and waiting syntax
        // signals that the variable is no longer empty
        pthread_cond_signal(&fourthemptycond);

        // synchronization for concurrent execution exploration for lock/unlock syntax
        // this unlocks the input and output mutex because their is data
        pthread_mutex_unlock(&fourthmutex);

        // synchronization mechanisms beyond mutex exploration for terminating syntax
        // terminates the program once the endprogram variable is set to true
        if (endprogram == true)
        {
            exit(1);
        }
    }

    // reset global buffer check variable
    buffercheck = false;
    pthread_exit(&fourthmutex);
}


// this function is called by the second thread function to handle spaces
const char* handleSpaces(char* input)
{
    int x;
    // stores the length of the input into the variable
    int spacedlength = strlen(input);

    // runs from the beginning to the end of the input
    for (x = 0; x <= spacedlength; x++)
    {
        // if the character at the specified index of the input is a new line
        if (input[x] != '\n')
        {
            // continue the program
            continue;
        }
        // if the character at the specified index of the input is not a new line
        else
        {
            // insert a space at that index
            input[x] = space;
        }
    }

    // return the correctly spaced input back
    return input;
}

// this function is called by the third thread function to handle the plus signs
const char* handleCarrot(char *input)
{
    int inputlength = strlen(input);
    // variable to store the buffer input
    // memory allocation for a character pointer
    // https://stackoverflow.com/questions/51657540/memory-allocation-for-char-pointer
    char* copiedinput = (char*)malloc((inputlength + 1) * sizeof(char));
    // copies the data into the variable using memset()
    // https://www.educative.io/edpresso/copying-data-using-the-memset-function-in-c
    memset(copiedinput, '\0', inputlength + 1);
    // copies the number of characters from one memory block to another
    // https://www.educative.io/edpresso/c-copying-data-using-the-memcpy-function-in-c
    memcpy(copiedinput, input, inputlength);

    // creates variable to count the total instances of signs
    int signcount = 0;
    int x; 
    int copiedinputlength = strlen(copiedinput);

    // implemented syntax from the assignment 3 smallsh.c program
    // syntax similar to the removing the "$$" symbol, except modified to "++"
    // run from the begining of the input to the end of the input
    for (x = 0; x < copiedinputlength; x++)
    {
        // if two elements in a row of the input contain '+' signs
        if (copiedinput[x] == '+' && copiedinput[x+1] == '+')
        {
            // increment the sign count and x variable by one
            x = x + 1;
            signcount = signcount + 1;
        }
    }

    // variable to store the buffer input
    // memory allocation for a character pointer
    // https://stackoverflow.com/questions/51657540/memory-allocation-for-char-pointer
    char* carrotinput = (char*)malloc((strlen(input) - signcount) * sizeof(char));
    // copies the data into the variable using memset()
    // https://www.educative.io/edpresso/copying-data-using-the-memset-function-in-c
    int newlength = inputlength - signcount;
    memset(carrotinput, '\0', newlength);

    // if there are signs present in the input
    if (signcount > 0)
    {
        int i;
        int count; 

        // run for the total number of plus signs counted
        for (i = 1; i <= signcount; i++)
        {
            // creates a variable to keep track the for loop
            // resets at the end of the concatenated loops
            int startover = 0;
            int j;
            // runs until the tracker variable is set to true
            for (j = 0; startover == 0; j++)
            {
                // increments the count variable by sign byte size
                count = j + SIGNCOUNT;

                // implemented syntax from the assignment 3 smallsh.c program
                // syntax similar to the removing the "$$" symbol, except modified to "++"
                // run from the begining of the input to the end of the input
                if (copiedinput[j] == '+' && copiedinput[j + 1] == '+')
                {
                    // sets the tracker variable to true
                    startover = 1;
                    // concatenates the two character strings
                    strncat(carrotinput, copiedinput, j);
                    // inserts the carrot sign in place of the plus signs
                    strcat(carrotinput, "^");
                    // increases the overall count of the variable
                    copiedinput = copiedinput + count;
                }
                // if there was no plus sign in the input
                else
                {
                    // sets the tracker variable to false
                    startover = 0;
                }

            }
        }
    }

    // moves a copy of the source string into the desination string
    strcat(carrotinput, copiedinput);
    // returns this newly created input with modified + and ^ signs
    return carrotinput;
}


int main(void)
{
    // synchronization for concurrent execution exploration for syntax
    // initializes the first mutex by using initializing function
    pthread_mutex_init(&firstmutex, NULL);
    pthread_mutex_init(&secondmutex, NULL);
    pthread_mutex_init(&thirdmutex, NULL);
    pthread_mutex_init(&fourthmutex, NULL);
    
    // defines and creates our first thread
    // threads - concepts and api exploration for syntax
    pthread_t firstthread;
    pthread_create(&firstthread, NULL, firstThreadFunction, NULL);

    // defines and creates our second thread
    // threads - concepts and api exploration for syntax
    pthread_t secondthread;
    pthread_create(&secondthread, NULL, secondThreadFunction, NULL);

    // defines and creates our third thread
    // threads - concepts and api exploration for syntax
    pthread_t thirdthread;
    pthread_create(&thirdthread, NULL, thirdThreadFunction, NULL);

    // defines and creates our fourth thread
    // threads - concepts and api exploration for syntax
    pthread_t fourththread;
    pthread_create(&fourththread, NULL, fourthThreadFunction, NULL);


    // joins our four threads and waits for them to finish
    // threads - concepts and api exploration for syntax
    pthread_join(firstthread, NULL);
    pthread_join(secondthread, NULL);
    pthread_join(thirdthread, NULL);
    pthread_join(fourththread, NULL);


    // destroys our created mutexes
    // threads - concepts and api exploration for syntax
    pthread_mutex_destroy(&firstmutex);
    pthread_mutex_destroy(&secondmutex);
    pthread_mutex_destroy(&thirdmutex);
    pthread_mutex_destroy(&fourthmutex);

    
    // terminates the calling thread of our four threads
    pthread_exit(&firstthread);
    pthread_exit(&secondthread);
    pthread_exit(&thirdthread);
    pthread_exit(&fourththread);

    // exits the program and returns zero
    exit(EXIT_SUCCESS);
    return 0;
}

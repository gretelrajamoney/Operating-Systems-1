#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>


/* struct for movie information */
struct movie 
{
	char *title;
	int year;
	char *language;
	float rating;
	struct movie *next;
};

/* parse the current line which is space delimited and create a
 * movie struct with the data in this line
 */

struct movie *createMovie(char *currLine)
{
	struct movie *currMovie = malloc(sizeof(struct movie));

	// for use with strtok_r
	char *saveptr;

	// the first token is the movie title
	char *token = strtok_r(currLine, ",", &saveptr);
	currMovie->title = calloc(strlen(token) + 1, sizeof(char));
	strcpy(currMovie->title, token);

	// the next token is the release year
	token = strtok_r(NULL, ",", &saveptr);
	currMovie->year = atoi(token);

	// the next token is the movie language
	token = strtok_r(NULL, ",", &saveptr);
	currMovie->language = calloc(strlen(token) + 1, sizeof(char));
	strcpy(currMovie->language, token);

	// the last token is the movie rating
	token = strtok_r(NULL, " ", &saveptr);
	currMovie->rating = atof(token);

	// set the next node to NULL in the newly created movie entry
	currMovie->next = NULL;

	return currMovie;
}	

/* creates a directory using my onid and a random number
 */
int createDirectory()
{
	// in order to generate a random number each time
	srand(time(0));
	// generates a random number between the range 0 and 99999
	int num = (rand() % (99999 - 0 + 1)) + 0;
	// creates a character array to move the number into
	char numString[10];
	
	// creates character strings for our directory names
	const char* onid = "rajamong.";
	const char* movies = "movies.";
	
	// created a buffer using the length of our onid string
	char* combined = malloc(strlen(onid) + 1 + 4);
	
	// combines both our onid with our movie character string
	strcpy(combined, onid);
	strcat(combined, movies);
	
	// converts out number into a string array
	snprintf(numString, 10, "%d", num);
	
	char *directoryName;
	// concatenates our string and int inputs together
	directoryName = strcat(combined, numString);
	// creates the directory using our name
	mkdir(directoryName, 0750);	
	printf("created a directory named: %s\n", directoryName);

	int ch = chdir(directoryName);
}

/* finds the movies for each year and stores it in a newly created
 * file with is added into our newly created repository
 */
void findMoviesForEveryYear(struct movie* list)
{
	// creates variable to keep track of the years	
        int maxYear = 0;	
	// calls our create directory function
	createDirectory();

	// runs for a wide range of years to ensure none are missed
    	for (int x = 1000; x <= 2021; x++)
    	{
		// creates a new struct carrying our list
        	struct movie* current = list;

		// runs until our list of movies is empty
        	while (current != NULL)
        	{
			// if the years are the same it stores the movie
            		if (current->year == x)
            		{
				// if the year matches the year of our tracker variable
				
				if (current->year == maxYear)
				{
					// creates array to convert year to a string format
				        char yearString[10];
                                        const char* txt = ".txt";
					// moves the string of characters for the year into an array
					snprintf(yearString, 10, "%d", x);
					// concatenates our year with out .txt ending
					DIR* currDir = opendir(".");
                                        char* newFile = strcat(yearString, txt);
					FILE *fptr;
					// opens our file so that we can write in it
                                        fptr = fopen(newFile,"a");
					// prints out the year and the title for the movie
                                        fprintf(fptr,"%d %s\n", current->year, current->title);
					// closes our opened file
                                        fclose(fptr);	
					// moves to next element on our list
					current = current->next;					
				}
				
				else
				{
					// creates array to convert year to a string format
					char yearString[10];
					const char* txt = ".txt";
					// opens our directory in order to create a text file
					DIR* currDir = opendir(".");
					int file;
					// moves the string of characters for the year into an array
					snprintf(yearString, 10, "%d", x);
					char* newFile = strcat(yearString, txt);
					// opens our newly created file in the directory
					file = open(newFile, O_RDWR | O_CREAT | O_TRUNC, 0750);
					FILE *fptr;
					// opens our file so that we can write in it
					fptr = fopen(newFile,"a");
					// prints out the year and movie title for the movie
					fprintf(fptr,"%d %s\n", current->year, current->title);
					// closes our opened file
   					fclose(fptr);
					// sets out tracker value to the current year
					maxYear = x;
					// moves to the next element in the list
					current = current->next;	
				}
                		//printf("%d %s\n", current->year, current->title);
                		//current = current->next;
            		}		

            		else
            		{
				// moves to the next element in the list
                		current = current->next;
           		}
        	}
    	}
}


struct movie *processFile(char *filePath)
{
	// open the specified file for reading only
	FILE *movieFile = fopen(filePath, "r");
	char *currLine = NULL;
	size_t len = 0;
	ssize_t nread;
	char *token;

	// the head of the linked list
	struct movie *head = NULL;
	// the tail of the linked list
	struct movie *tail = NULL;

	// read the file line by line
	while ((nread = getline(&currLine, &len, movieFile)) != -1)
	{
		// get a new movie node corresponding to the current line
		struct movie *newNode = createMovie(currLine);

		// is this the first node in the linked list?
		if (head == NULL)
		{
			// this is the first node in the linked list
			// set the head and the tail to this node
			head = newNode;
			tail = newNode;
		}
		else
		{
			// this is not the first node
			// add this node to the list and advance the tail
			tail->next = newNode;
			tail = newNode;
		}
	}

	free(currLine);
	fclose(movieFile);
	return head;
}




int main()
{

	int choice;

	// prints out prompts and reads in inputs
	while(1)
	{
		printf("1. select file to process\n");
		printf("2. exit the program\n");
		printf("\n");

		printf("please enter what option you would like: ");
		scanf("%d", &choice);
		printf("\n");

		if (choice == 1)
		{
			printf("which file do you want to process ?\n");
			printf("\n");
			printf("1. the largest file\n");
			printf("2. the smallest file\n");
			printf("3. specify the name of the file\n");
			printf("\n");
			
			printf("please enter what option you would like: ");
			scanf("%d", &choice);
			printf("\n");

			if (choice == 1)
			{
				// used modules in this function to find the size of files
				// using statbuf, statbuffer, dirent, entry, etc.
				char *dir = ".";
				DIR *dp;
				struct dirent *entry;
				struct stat statbuf;
				struct stat statbuffer;
				int i = 0;
				char *name;
				char *csv;
				char *movies_;
			
				if ((dp = opendir(dir)) == NULL)
				{
					fprintf(stderr, "cannot open directory: %s\n", dir);
				}

				chdir(dir);
				
				while ((entry = readdir(dp)) != NULL)
				{
					stat(entry->d_name, &statbuf);
			
					if(S_ISREG(statbuf.st_mode))
					{
						// checks for files of correct name and format
						lstat(entry->d_name, &statbuf);
						csv = strstr(entry->d_name, ".csv");
						movies_ = strstr(entry->d_name, "movies_");
						
						// if they are correct, checks for size in bytes
						if(csv && movies_)
						{
							// locates the biggest file in directory	
							if(statbuf.st_size > i)
							{
								i = (statbuf.st_size);
								name = (entry->d_name);
							}	
						}
					}
				}
				printf("now processing the largest file in the directory named:  %s\n", name);
				printf("\n");
				// processes the selected file
				struct movie *list = processFile(name);
				// prints out the movies/creates directory/writes into files
				findMoviesForEveryYear(list);
				// closes our directory once finished
				closedir(dp);
				
			}

			else if (choice == 2)
			{
				// used modules in this function to find the size of files
				// using statbuf, statbuffer, dirent, entry, etc.
				char *dir = ".";
				DIR *dp;
				struct dirent *entry;
				struct stat statbuf;
				struct stat statbuffer;
				int i = 10000;
				char *name;
				char *csv;
				char *movies_;


				if ((dp = opendir(dir)) == NULL)
				{
					fprintf(stderr, "cannot open directory: %s\n", dir);
				}
				
				chdir(dir);

                                while ((entry = readdir(dp)) != NULL)
                                {
                                        stat(entry->d_name, &statbuf);

                                        if(S_ISREG(statbuf.st_mode))
                                        {
						// checks for files of correct name and format
                                                lstat(entry->d_name, &statbuf);
                                                csv = strstr(entry->d_name, ".csv");
						movies_ = strstr(entry->d_name, "movies_" );				
		
						// if they are correct, checks for size in bytes
                                                if(csv && movies_)
                                                {
							// locates the smaller file in the directory
                                                        if(statbuf.st_size <= i)
                                                        {
                                                                i = (statbuf.st_size);
                                                                name = (entry->d_name);
							}
						}
					}
				}

				printf("now processing the smallest file in the directory named: %s\n", name);
				printf("\n");
				// stores movies from the file onto list
				struct movie *list = processFile(name);
				// finds the movies, creates the directory, writes into the files
                                findMoviesForEveryYear(list);
				// closes the newly created directory
                                closedir(dp);
			}

			else if (choice == 3)
			{
				// creates character string to hold user input
				char fileInput[100];
		
				printf("please enter your file name: ");
				// scans in the input
				scanf("%s", &fileInput);
				printf("\n");
		
				// if the file is found in the directory
				if (access(fileInput, F_OK) == 0)
				{
					printf("now processing your file named: %s\n", fileInput);
					printf("\n");
					// creates list to hold all of the data from the file
					struct movie *list = processFile(fileInput);
					// creates directory, finds all movies, and writes into txt files
					findMoviesForEveryYear(list);
					printf("\n");
				}
				
				else
				{
					// if the file doesnt exist, this error message is printed
					printf("the file you requested does not exist :(\n");
					printf("\n");
				}
			
			}                                
			
			else
			{
				// program exits if user inputs incorrect choice
				exit(0);	
			}
		}
		
		else if (choice == 2)
		{
			// program exits if the user requests it
			exit(0);
		}
		
		else
		{
			// program exits by default
			exit(0);	
		}
	}
}

// If you are not compiling with the gcc option --std=gnu99, then
// // uncomment the following line or you might get a compiler warning
// //#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* struct for movie information */
struct movie {
    char *title;
    int year;
    char *language;
    float rating;
    struct movie *next;
};

/* Parse the current line which is space delimited and create a
 * *  movie struct with the data in this line
 * */
struct movie *createMovie(char *currLine)
{
    struct movie *currMovie = malloc(sizeof(struct movie));

    // For use with strtok_r
    char *saveptr;

    // The first token is the movie title
    char *token = strtok_r(currLine, ",", &saveptr);
    currMovie->title = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->title, token);

    // The next token is the release year
    token = strtok_r(NULL, ",", &saveptr);
    currMovie->year = atoi(token);

    // The next token is the movie language
    token = strtok_r(NULL, ",", &saveptr);
    currMovie->language = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->language, token);

    // The last token is the movie rating
    token = strtok_r(NULL, " ", &saveptr);
    currMovie->rating = atof(token);

    // Set the next node to NULL in the newly created movie entry
    currMovie->next = NULL;

    return currMovie;
}

/*
 * * Return a linked list of movies by parsing data from
 * * each line of the specified file.
 * */
struct movie *processFile(char *filePath)
{
    // Open the specified file for reading only
    FILE *movieFile = fopen(filePath, "r");
    char *currLine = NULL;
    size_t len = 0;
    ssize_t nread;
    char *token;

    // The head of the linked list
    struct movie *head = NULL;
    // The tail of the linked list
    struct movie *tail = NULL;

    // Read the file line by line
    while ((nread = getline(&currLine, &len, movieFile)) != -1)
    {
        // Get a new movie node corresponding to the current line
        struct movie *newNode = createMovie(currLine);

        // Is this the first node in the linked list?
        if (head == NULL)
        {
            // This is the first node in the linked link
            // Set the head and the tail to this node
            head = newNode;
            tail = newNode;
        }
        else
        {
            // This is not the first node.
            // Add this node to the list and advance the tail
            tail->next = newNode;
            tail = newNode;
        }
    }
    free(currLine);
    fclose(movieFile);
    return head;
}

/*
 * * Print data for the given movie
 * */
void printMovie(struct movie* aMovie){
  printf("%s, %d, %s, %0.1f\n", aMovie->title,
               aMovie->year,
               aMovie->language,
               aMovie->rating);
}
/*
 * * Print the linked list of movies
 * */
int printMovieList(struct movie *list)
{
    int movieCount = 0;

    while (list != NULL)
    {
	movieCount = movieCount + 1;
        //printMovie(list);
        list = list->next;
    }

    return movieCount;
}
/*
 * * Finds the movies corresponding to the inputted year
 * */
void findMoviesForYear(struct movie *list, int yearRequest, int movieCount)
{
	// creates variable to check if there were any movies for the year
	int valid = 0;
	
	// runs until list of movies is empty
	while (list != NULL)
	{
		// checks if the movie year matches up to the user request
		if (list->year == yearRequest)
		{
			// prints out movie title and goes down to the next on the list
			printf("%s\n", list->title);
			list = list->next;
			// sets checker variable to one
			valid = 1;
		}
		
		// if the years do not match up, moves to next movie on the list
		else
		{
			list = list->next;		
		}
	}

	if (valid == 1)
	{
		printf("\n");
	}
	
	// prints out a statement if there were no movies of the year
	else
	{
		printf("there are no movies for the year of %d\n", yearRequest);
	}
}
/*
 * * Finds all of the movies on the list that match the user inputted language
 */
void findMoviesForLanguage(struct movie *list, char languageRequest[], int movieCount)
{
	char *test;
	// creates a variable to check whether any movies were found
	int valid = 0;

	// runs until there are no more movies on the list
	while (list != NULL)
	{	
		// compares the movie language to the user requested language
		test = strstr(list->language, languageRequest);

		// prints out the year and title if they are the same language
		if(test)
		{
			printf("%d %s", list->year, list->title);
			printf("\n");
			// moves to the next movie on the list
			list = list->next;
			// sets the checking variable to one
			valid = 1;
		}
		
		else
		{
			// moves to the next movie on the list
			list = list->next;
		}
	}

	if (valid == 1)
	{
		printf("\n");
	}
	
	// prints out statement if there were no movies in the requested language
	else
	{
		printf("there are no movies for the language of %s\n", languageRequest);
	}
}
/*
 * * Finds the top rated movie for every year on the list
 */
void findAllTopRatings(struct movie *list)
{
	//runs for a general range of years
	for (int year = 1000; year <= 2021; year++)
	{
		// creates a minimum ranking that is impossible
		double maxRating = -1.0;
		// creates a new movie struct
		struct movie *maxMovie = NULL;
		// moves our list of movies into current variable
		struct movie *current = list;
		
		// while the list is not empty it runs
		while (current != NULL)
		{
			// if the years are equal to eachother
			if (current->year == year)
			{
				// if the rating is greater than the prior maximum, it replaces it
				if (current->rating > maxRating)
				{
					maxRating = current->rating;
					maxMovie = current;
				}
			}
			
			// moves to the next movie in the list		
			current = current->next;
		}

		// prints out the year, title, and rating for the stored highest ranked movie
		if (maxMovie != NULL)
		{
			printf("%d %s %0.1f\n", maxMovie->year, maxMovie->title, maxMovie->rating);
		}
	}
}
		
	
/*
 * *   Process the file provided as an argument to the program to
 * *   create a linked list of student structs and print out the list.
 * *   Compile the program as follows:
 * *       gcc --std=gnu99 -o students main.c
 * */

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("You must provide the name of the file to process\n");
        printf("Example usage: ./students student_info1.txt\n");
        return EXIT_FAILURE;
    }

    // creates necessary variables to run
    struct movie *list = processFile(argv[1]);
    //printMovieList(list);
    char *fileName = argv[1];
    int movieCount = printMovieList(list);
    int choice;
    int yearRequest;
    char languageRequest[1000];	   

    // prints out statement that the data has been parsed
    printf("\n");
    printf("processed file %s and parsed data for %d movies !!", fileName, movieCount);
    printf("\n");
    printf("\n");	

    // repeatedly reprompts user until they exit the program	
    while(1)
    {
	// prints out the list of options to select from
    	printf("1. show movies released in the specific year \n");
    	printf("2. show highest rated movie for each year \n");
    	printf("3. show the title and year of all movies in a specific language \n");
    	printf("4. exit the program \n");
    	printf("\n");

    	printf("please enter what option you would like: ");
	// stores the user inputted choice
    	scanf("%d", &choice);
    	printf("\n");	

    	if (choice == 1)
   	{
		// asks user what year they want to find movies from and stores it
		printf("enter the year you would like to see movies from: ");
		scanf("%d", &yearRequest);
		printf("\n");
		// calls function to print out their requested movies
		findMoviesForYear(list,yearRequest,movieCount);
    	}
	
        else if (choice == 2)
    	{
		// calls function to print out the highest ranked movies of each year
		findAllTopRatings(list);
		printf("\n");	
    	}	
   
    	else if (choice == 3)
    	{
		// asks user what language they want to find movies from and stores it
  		printf("enter the language you would like to see movies from: ");
		scanf("%s", &languageRequest);
		printf("\n");
		// calls function to print out their requested movies
		findMoviesForLanguage(list,languageRequest,movieCount);
	}

    	else if (choice == 4)
    	{
		// exits the program
    		exit(0);
    	}

   	else
    	{
		// prints out error message prior to repromting the user
    		printf("you entered an incorrect choice, please try again \n");
		printf("\n");
    	}
    }		
}


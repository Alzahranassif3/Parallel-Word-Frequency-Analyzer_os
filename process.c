
#include <stdio.h> //For printf(), fscanf(), and fclose()
#include <stdlib.h>//For memory allocation(malloc),exit(),qsort() 
#include <string.h> //For strcpy(), strcasecmp(), snprintf()
#include <sys/time.h>  // For gettimeofday()
#include <sys/wait.h>  // For wait()
#include <unistd.h>    // For fork()
#include <sys/mman.h> // For mmap() , munmap()
#include <semaphore.h>  // Semaphore

#define MAX_WORDS 18000000  // Maximum number of words
#define MAX_WORD_LENGTH 100  // Maximum word length
#define NUM_PROCESS 2  // Number of process

// Structure to hold a word and its count
struct singleword {
    char word[MAX_WORD_LENGTH];
    int count;
};

// Shared memory
struct singleword *words = NULL;
struct singleword *wordcounted = NULL;
int *realnum ;  // Number of unique words after counting

sem_t *semaphore;  // Semaphore for synchronization between processes

int counter = 0;

// Function prototypes
double getTimeInSeconds();// Function to get current time in seconds
void readDataFile();  // Function to read the file
void FindFrequentWords(int);  // Function to be executed by each process
int compare_words(const void *a, const void *b);
void sortWords(struct singleword *words, int size);  // Function to sort words using quicksort
void printTopWords();  // Function to print the top 10 words


int main() {

	realnum = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (realnum == MAP_FAILED) {
    		printf("mmap failed for realnum");
    		exit(1);
	}
	*realnum = 0;  // Initialize to 0 at the start


	words =mmap(NULL, MAX_WORDS * sizeof(struct singleword), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    	if (words == MAP_FAILED) {
        	printf("mmap failed for words");
        	exit(1);
    	}


	wordcounted = mmap(NULL, MAX_WORDS * sizeof(struct singleword), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
		if (wordcounted == MAP_FAILED) {
        	printf("mmap failed for wordcounted");
        	exit(1);
    	}


	semaphore=mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    	if (semaphore == MAP_FAILED) {
        	printf("mmap failed for semsphore");
        	exit(1);
    	}
	if (sem_init(semaphore, 1, 1) != 0) {
        	printf("sem_init error");
        	exit(1);
    	}

    	// Start measuring time
    	double start_time = getTimeInSeconds();  // Start wall clock time

    	// Read the data file
    	readDataFile();
	 double end_timeread = getTimeInSeconds();
        double time_takenread = end_timeread - start_time;

        double start_timeproc = getTimeInSeconds();

    	int pid[ NUM_PROCESS];

    	// Loop to create multiple child processes
    	for (int i = 0; i < NUM_PROCESS; i++) {

        	pid[i] = fork();  // Create a new child process

        	if (pid[i] < 0) {  // Error handling
            		printf("An error occurred with fork!\n");
            		return 1;
        	}
        	else if (pid[i] == 0) {  // Child process
            		FindFrequentWords(i);  // Call the function for processing
            		exit(0);  // Exit child process after completing task
        	}
    	}

    	// Parent process waits for all child processes to complete
    	for (int i = 0; i < NUM_PROCESS; i++) {
        	wait(&pid[i]);  // Wait for any child process to finish
    	}
	double end_timeproc = getTimeInSeconds();
        double time_takenproc = end_timeproc - start_timeproc;


        double start_timesort = getTimeInSeconds();

    	// Sort the words by frequency
    	sortWords(wordcounted, *realnum);
	double end_timesort = getTimeInSeconds();
        double time_takensort = end_timesort - start_timesort;


    	// Print the top 10 most frequent words
    	printTopWords();

    	// End measuring time
    	double end_time = getTimeInSeconds();  // End wall clock time
    	double time_taken = end_time - start_time;  // Calculate elapsed time
        printf("In Multiprocessing approach with %d processess \n",NUM_PROCESS);
	printf("\nTime taken to read Data file: %.6f seconds\n", time_takenread);
        printf("\nTime taken to find the frequency of each word : %.6f seconds\n", time_takenproc);
        printf("\nTime taken to sort the top 10: %.6f seconds\n", time_takensort);
    	printf("\nTime taken to execute: %.6f seconds\n", time_taken);

	// Unmap the shared memory regions
	munmap(words, MAX_WORDS * sizeof(struct singleword));
	munmap(wordcounted, MAX_WORDS * sizeof(struct singleword));
	munmap(realnum, sizeof(int));
	munmap(semaphore, sizeof(sem_t));

	// Destroy the semaphore
	sem_destroy(semaphore);
	return 0;

}

// Function to get current time in seconds
double getTimeInSeconds() {
    	struct timeval tv;
    	gettimeofday(&tv, NULL);
    	return (double)(tv.tv_sec) + (double)(tv.tv_usec) / 1000000.0;
}

void readDataFile() {
    	FILE *data = fopen("/home/alzahra/enwik8.txt.txt", "r");
    	if (data == NULL) {
        	printf("Error opening file, the path you entered is wrong ! \n");
        	return;  // Exit the program if the file cannot be opened
    	}

    	char word[MAX_WORD_LENGTH];

    	while (fscanf(data, "%99s", word) == 1) {
        	snprintf(words[counter].word, MAX_WORD_LENGTH, "%s", word);
        	words[counter].count = 1;
        	counter++;
    	}
    	fclose(data);
}

void FindFrequentWords(int number) {

    	int sizePerProcess = counter / NUM_PROCESS;
    	int startingIndex = number * sizePerProcess;
    	int endingIndex = (number + 1) * sizePerProcess;

    	if (number == (NUM_PROCESS - 1)) {
        	endingIndex = counter;
    	}

    	char wordToFound[MAX_WORD_LENGTH];

    	for (int i = startingIndex; i < endingIndex; i++) {
        	strcpy(wordToFound, words[i].word);
        	int found = 0;

        	// Try to find the word in wordcounted array
        	for (int j = 0; j < (*realnum); j++) {
            		if (strcasecmp(wordcounted[j].word, wordToFound) == 0) {
                		// If found, update count
                		sem_wait(semaphore);  // Wait (lock the semaphore)
                		wordcounted[j].count++;
                		sem_post(semaphore);  // Signal (unlock the semaphore)
                		found = 1;
                		break;
            		}
        	}

		// If word is not found, add it to wordcounted
        	if (!found && (*realnum) < counter) {
			sem_wait(semaphore);  // Wait (lock the semaphore)
            		snprintf(wordcounted[(*realnum)].word, MAX_WORD_LENGTH, "%s", wordToFound);
            		wordcounted[(*realnum)].count = 1;
            		(*realnum)++;
            		sem_post(semaphore);  // Signal (unlock the semaphore)
        	}

    }
}

int compare_words(const void *a, const void *b) {
    	struct singleword *wordA = (struct singleword *)a;
    	struct singleword *wordB = (struct singleword *)b;
    	return wordB->count - wordA->count;
}

void sortWords(struct singleword *words, int size) {
    	qsort(words, size, sizeof(struct singleword), compare_words);
}

void printTopWords() {
	printf("Top 10 most frequent words:\n");
    	for (int i = 0; i < 10 && i < (*realnum); i++) {
        	printf("%d)  %s - %d\n",(i+1), wordcounted[i].word, wordcounted[i].count);

	}
}


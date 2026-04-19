#include <stdio.h> // For printf(), fscanf(), fopen(), fclose()
#include <stdlib.h> // For memory allocation (malloc(), free()),qsort()
#include <string.h> // For strcpy(), strcasecmp(), snprintf()
#include <pthread.h>//For pthread_create(), pthread_join(), pthread_mutex_lock(), and pthread_mutex_unlock().
#include <sys/time.h>  // For gettimeofday()

#define MAX_WORDS 18000000  // Maximum number of words
#define MAX_WORD_LENGTH 100  // Maximum word length
#define NUM_THREADS 2  // Number of threads

// Structure to hold a word and its count
struct singleword {
    char word[MAX_WORD_LENGTH];
    int count;
};

// Global variables
struct singleword *words = NULL;  // array for words
struct singleword *wordcounted = NULL;  // array for counted words
int counter = 0;
int realnum = 0;  // Number of unique words after counting

pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;  // Mutex for thread-safe updates


// Function prototypes
double getTimeInSeconds();// Function to get current time in seconds
void readDataFile();  // Function to read the file
void*  FindFrequentWords(void* arg);  // Function to be executed by each thread
int compare_words(const void *a, const void *b);
void sortWords(struct singleword *words, int size);  // Function to sort words using quicksort
void printTopWords();  // Function to print the top 10 words

int main() {

	// Dynamically allocate memory for the arrays
    	words = (struct singleword *)malloc(MAX_WORDS * sizeof(struct singleword));
    	wordcounted = (struct singleword *)malloc(MAX_WORDS * sizeof(struct singleword));
    	if (words == NULL || wordcounted == NULL) {
        	printf("Memory allocation failed");
        	return 1;  // Exit if memory allocation fails
    	}

    	// Start measuring time
    	double start_time = getTimeInSeconds();

    	readDataFile();
	double end_timeread = getTimeInSeconds();
        double time_takenread = end_timeread - start_time;

	double start_timethread = getTimeInSeconds();
    	pthread_t threads[NUM_THREADS];

    	// Create threads
    	for (int i = 0; i < NUM_THREADS; i++) {
        	int *threadNumber = malloc(sizeof(int));
        	*threadNumber = i;
        	pthread_create(&threads[i], NULL,  FindFrequentWords,  threadNumber);
    	}

    	// Wait for all threads to finish
    	for (int i = 0; i < NUM_THREADS; i++) {
        	pthread_join(threads[i], NULL);
    	}

	double end_timethread = getTimeInSeconds();
        double time_takenthread = end_timethread - start_timethread;


	double start_timesort = getTimeInSeconds();

    	// Sort the words by frequency
    	sortWords(wordcounted, realnum);

 	double end_timesort = getTimeInSeconds();
        double time_takensort = end_timesort - start_timesort;

    	// Print the top 10 most frequent words
    	printTopWords();

    	// End measuring time
    	double end_time = getTimeInSeconds();  // End wall clock time
    	double time_taken = end_time - start_time;
	printf("In Multithreading approach with %d threads \n",NUM_THREADS);
        printf("\nTime taken to read Data file: %.6f seconds\n", time_takenread);
        printf("\nTime taken to find the frequency of each word : %.6f seconds\n", time_takenthread);
        printf("\nTime taken to sort the top 10: %.6f seconds\n", time_takensort);
    	printf("\nTime taken to execute: %.6f seconds\n", time_taken);


	// Free the dynamically allocated memory
    	free(words);
    	free(wordcounted);

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
        	printf("Error opening file");
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

void*  FindFrequentWords(void* arg) {

	int number = *(int*)arg;  // Thread number

    	// Divide the workload among threads
    	int sizePerThread = counter / NUM_THREADS;
    	int startingIndex = number * sizePerThread;
    	int endingIndex = (number + 1) * sizePerThread;

    	// Handle the last thread to cover any remaining words
    	if (number == (NUM_THREADS - 1)) {
        	endingIndex = counter;
    	}

    	char wordToFound[MAX_WORD_LENGTH];


   	for (int i = startingIndex; i < endingIndex; i++) {
        	strcpy(wordToFound, words[i].word);
        	int found = 0;

        	// Try to find the word in wordcounted array
        	for (int j = 0; j < realnum; j++) {
            		if (strcasecmp(wordcounted[j].word, wordToFound) == 0) {
                		// If found, update count
               			pthread_mutex_lock(&counter_mutex);  // Lock to safely update shared data
                		wordcounted[j].count++;
                		pthread_mutex_unlock(&counter_mutex);  // Unlock after updating shared data
                		found = 1;
                		break;
            		}
        	}

        	// If word is not found, add it to wordcounted
        	if (!found && realnum < counter) {
            		pthread_mutex_lock(&counter_mutex);  // Lock to safely update shared data
            		snprintf(wordcounted[realnum].word, MAX_WORD_LENGTH, "%s", wordToFound);
            		wordcounted[realnum].count = 1;
           	 	realnum++;
            		pthread_mutex_unlock(&counter_mutex);  // Unlock after updating shared data
        	}
	}


    	free(arg);  // Free allocated memory for thread number
    	return NULL;
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
    	for (int i = 0 ; i < 10  && i < realnum ; i++) {
                printf("%d)  %s - %d\n",(i+1), wordcounted[i].word, wordcounted[i].count);

        }
}

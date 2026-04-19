#include <stdlib.h>// For malloc, free, exit
#include <stdio.h>//For printf, fscanf, fopen, and fclose
#include <string.h>//For strcpy, strcasecmp, snprintf
#include <sys/time.h>  // For gettimeofday()

#define MAX_WORDS 18000000  // Maximum number of words
#define MAX_WORD_LENGTH 100  // Maximum word length

struct singleword {
    char word[MAX_WORD_LENGTH];
    int count;
};


// Global variables
struct singleword *words = NULL;  // array for words
struct singleword *wordcounted = NULL;  // array for counted words
int counter = 0;
int realnum = 0;  // Number of unique words after counting


// Function prototypes
double getTimeInSeconds();// Function to get current time in seconds
void readDataFile();  // Function to read the file
void FindFrequentWords();  // Function to find the frequency for each word
int compare_words(const void *a, const void *b);
void sortWords(struct singleword *words, int size);  // Function to sort words using quicksort
void printTopWords();  // Function to print the top 10 words


int main() {
	// Dynamically allocate memory for the arrays
    	words = (struct singleword *)malloc(MAX_WORDS * sizeof(struct singleword));
    	wordcounted = (struct singleword *)malloc(MAX_WORDS * sizeof(struct singleword));
    	if (words == NULL || wordcounted == NULL) {
        	printf("Memory allocation failed\n");
        	return 1;  // Exit if memory allocation fails
    	}
        // Start measuring time
	double start_time = getTimeInSeconds();  // Start wall clock time

	// start reading the input file
	readDataFile();
	double end_timeread = getTimeInSeconds();
        double time_takenread = end_timeread - start_time;

	double start_timefreq = getTimeInSeconds();
	FindFrequentWords();
	double end_timefreq = getTimeInSeconds();
        double time_takenfreq = end_timefreq - start_timefreq;


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
	printf("In Naive system \n");
	printf("\nTime taken to read Data file: %.6f seconds\n", time_takenread);
        printf("\nTime taken to find the frequency of each word : %.6f seconds\n", time_takenfreq);
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

void FindFrequentWords() {

  	char wordToFound[MAX_WORD_LENGTH];

	for (int i = 0 ; i < counter; i++) {
        	strcpy(wordToFound, words[i].word);
        	int found = 0;

        	// Try to find the word in wordcounted array
        	for (int j = 0; j < realnum; j++) {
            		if (strcasecmp(wordcounted[j].word, wordToFound) == 0) {
                		// If found, update count
                		wordcounted[j].count++;
                		found = 1;
                		break;
            		}
        	}

        	// If word is not found, add it to wordcounted
        	if (!found && realnum < counter) {
            		snprintf(wordcounted[realnum].word, MAX_WORD_LENGTH, "%s", wordToFound);
            		wordcounted[realnum].count = 1;
            		realnum++;

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
        for (int i = 0 ; i < 10  && i < realnum ; i++) {
                printf("%d)  %s - %d\n",(i+1), wordcounted[i].word, wordcounted[i].count);

        }
}

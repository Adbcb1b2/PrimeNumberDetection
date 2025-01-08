#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <pthread.h> //POSIX thread library

struct structVariables{
    int start;
    int end;
    int *fileNums;
    int * primes;
    int numberOfPrimes;
};

void * DeterminePrimes(void * args){
    int count = 0;
    
    // Cast void type back to struct
    struct structVariables * threadStruct = (struct structVariables*) args;
    
    // Extract variabes from struct
    int start = threadStruct -> start;
    int end = threadStruct -> end;
    
    // Count primes in slice list to allocate memory later
    for(int i = start; i <= end; i++){
    	int num = threadStruct-> fileNums[i]; // take each number in slice
    	bool isPrime = true;
    	
    	if (num <= 1) {
            isPrime = false;
        }
        if (num == 2) {
            isPrime = true;
        }
        for (int j = 2; j <= sqrt(num); j++) {
            if (num % j == 0) {
                isPrime = false;
                break;
            }
        }
        if (isPrime) {
            count++;
        }
    }
	
	// Allocate memory for the array storing prime numbers from the file
	threadStruct -> primes = (int*)malloc(count * sizeof(int));
	threadStruct -> numberOfPrimes = count;
    
    // Reset count for the second loop
    count = 0;

    // Iterate through the list again, adding the number to the list if it is a prime
    for (int i = start; i <= end; i++) {
        bool isPrime = true;
        int num = threadStruct-> fileNums[i];

        if (num <= 1) {
            isPrime = false;
        }
        if (num == 2) {
            isPrime = true;
        }
        for (int j = 2; j <= sqrt(num); j++) {
            if (num % j == 0) {
                isPrime = false;
                break;
            }
        }
        if (isPrime) {
            threadStruct -> primes[count] = num;
            count++;
        }
    }
    

    pthread_exit(NULL); 
    free(threadStruct -> primes);
}
void main(int argc, char** argv){
    const char* filename = argv[2];
    FILE *fptrread;
    int compCount = 0; // 
    int number;
    
  	// Check there are 3 command line arguments (./a.out, number of terms and thread count)
 	if(argc < 3){
 		printf("Either file name or thread count has been omitted. Please try again.\n");
 		exit(-1);
 	}
 	
    // Open the file in read mode
    fptrread = fopen(filename, "r");
    
    // Error check
	if(fptrread == NULL){
		printf("Error opening the file %s\n", filename);
		exit(-1); //Exit program if file can't be opened
    }else{
        printf("File opened successfully\n");
    }
    
    // Count the number of lines in the file
    while(fscanf(fptrread, "%d", &number)==1){
    	compCount++;
    }
    
    // Go back to the beginning of the file
    rewind(fptrread);
    
    // Allocate memory to store data from file
    int * numbers = malloc(sizeof(int) * compCount);
    
    // Store numbers from file in the list
    for(int i = 0; i < compCount; i++){
        fscanf(fptrread, "%d", numbers + i);
    }
    
    fclose(fptrread);
    
    // Thread count determined by command line argument
    int threadCount = atoi(argv[1]);
    
    // Check thread count is positive
    if(threadCount < 1){
    	printf("Invalid thread count entered. Please try again.\n");
    	exit(-1);
    }
    
   
    int sliceList[threadCount]; // slices needed determined by threadCount
    
    int remainder = compCount % threadCount;
    
    // Standard slices wihout remainder
    for (int i = 0; i < threadCount; i++){
        sliceList[i] = compCount / threadCount;
    }
    
    // Add extra computation (remainder) to slices
    for (int j = 0; j < remainder; j++){
        sliceList[j] = sliceList[j] + 1;
    }
    
    // Work out start and end of each slice
    int startList[threadCount];
    int endList[threadCount];
    
    for (int k = 0; k < threadCount; k++){
        if(k == 0){
            startList[k] = 0;
            endList[k] = startList[k] + sliceList[k] - 1;
        }else{
            startList[k] = endList[k-1] + 1;
            endList[k] = startList[k] + sliceList[k] - 1;
        }
    }
    
	// Array to store results from each thread
	struct structVariables results[threadCount];
	
	// Send slice
	pthread_t threadIDs[threadCount];
	
	for(int i = 0; i < threadCount; i++){
		// Set starts and end for each unique thread
		results[i].start = startList[i];
		results[i].end = endList[i];
		// Send whole array to thread - memory already generated
		results[i].fileNums = numbers;
		
		// Initialise thread
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		
		// Launch 
		pthread_create(&threadIDs[i], &attr, DeterminePrimes, &results[i]);
	}
	
	// Join threads and collect results
	int totalPrimes = 0;
	for(int i = 0; i < threadCount; i++){
		pthread_join(threadIDs[i], NULL);

		totalPrimes += results[i].numberOfPrimes;
	}
	
	
	// Array to hold all of the primes from all threads
	int * allPrimes = (int *)malloc(totalPrimes * sizeof(int));
	int currentIndex = 0;
	
	// Loop through each thread's results and add primes from each thread to the new array
	for (int i = 0; i < threadCount; i++){
		for(int j = 0; j < results[i].numberOfPrimes; j++){
			allPrimes[currentIndex] = results[i].primes[j];
			currentIndex++;
		}
		free(results[i].primes);
	}

	FILE * fptrwrite;
	
	// Open file for writing
	fptrwrite = fopen("allPrimes.txt", "w");
	
	// Check if the file has been created
	if(fptrwrite == NULL){
		printf("The file has not been created\n");
	}else{
		printf("The file 'allPrimes.txt' has successfully been created\n");
	}
	
	//Write the total prime numbers to the newly created file
	fprintf(fptrwrite, "Number of primes = %d\n", totalPrimes);
	
	//Then write the prime numbers themselves
	for(int i = 0; i < totalPrimes; i++){
		fprintf(fptrwrite, "%d\n", allPrimes[i]);
	}
	
	// Close the file
	fclose(fptrwrite);
	
	free(numbers);
	
}

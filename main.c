#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

double *array;
int numbersOfThreads;
int elementsPerThread;
int offset;
int arraySize;

struct timeRegistry {
  long int seconds;
  long int nanoseconds;
};
 
 

/**
 * @brief format the difference of time between two given timespecs in a timeRegistry
 * 
 * @param initial: timespec containing the initial time
 * @param final: timespec containing the final time
 * @return struct timeRegistry containing the difference of time.
 */
struct timeRegistry getTimeRegistry(struct timespec initial, struct timespec final) {
  struct timeRegistry registry;

  if (final.tv_nsec < initial.tv_nsec) {
    final.tv_nsec += 1000000000;
    final.tv_sec--;
  }

  registry.seconds = (long)(final.tv_sec - initial.tv_sec);
  registry.nanoseconds = final.tv_nsec - initial.tv_nsec;

  return registry;
}



/**
 * @brief display difference of time between two given timespecs
 * 
 * @param initialTime: timespec containing the initial time
 * @param finalTime: timespec containing the final time
 */
void printTime(struct timespec initialTime, struct timespec finalTime) {
  struct timeRegistry searchRegistry;
  struct timeRegistry sortRegistry;

  searchRegistry = getTimeRegistry(initialTime, finalTime);

  printf("Tempo de busca: %ld.%09ld\n", searchRegistry.seconds, searchRegistry.nanoseconds);
}



/**
 * @brief merge different arrays in just one
 * 
 * @param lowerIndex: lower index of array
 * @param middleIndex: middle index of array
 * @param higherIndex: higher index of array
 */
void merge(int lowerIndex, int middleIndex, int higherIndex) {
    int leftSegmentIndex = 0;
    int rightSegmentIndex = 0;
    int auxiliaryIndex = 0;

    int leftSegmentSize = middleIndex - lowerIndex + 1;
    int rightSegmentSize = higherIndex - middleIndex;
    
    double leftSegment[leftSegmentSize];
    double rightSegment[rightSegmentSize];

    for (int leftSegmentIndex = 0; leftSegmentIndex < leftSegmentSize; leftSegmentIndex++)
        leftSegment[leftSegmentIndex] = array[lowerIndex + leftSegmentIndex];
    
    for (int rightSegmentIndex = 0; rightSegmentIndex < rightSegmentSize; rightSegmentIndex ++)
        rightSegment[rightSegmentIndex] = array[middleIndex + 1 + rightSegmentIndex];
    
    leftSegmentIndex = 0;
    rightSegmentIndex = 0;

    while (leftSegmentIndex < leftSegmentSize && rightSegmentIndex < rightSegmentSize) {
        if (leftSegment[leftSegmentIndex] <= rightSegment[rightSegmentIndex]) {
            array[lowerIndex + auxiliaryIndex] = leftSegment[leftSegmentIndex];

            leftSegmentIndex++;
        } else {
            array[lowerIndex + auxiliaryIndex] = rightSegment[rightSegmentIndex];
            
            rightSegmentIndex++;
        }

        auxiliaryIndex ++;
    }
    
    while (leftSegmentIndex < leftSegmentSize) {
        array[lowerIndex + auxiliaryIndex] = leftSegment[leftSegmentIndex];

        auxiliaryIndex++;
        
        leftSegmentIndex ++;
    }

    while (rightSegmentIndex < rightSegmentSize) {
        array[lowerIndex + auxiliaryIndex] = rightSegment[rightSegmentIndex];
        
        auxiliaryIndex++;
        
        rightSegmentIndex ++;
    }
}



/**
 * @brief merges the previously ordered array segments
 * 
 * @param number: number of segments
 * @param aggregation: factor of aggregation, used to find the boundaries
 */
void mergeFinalArray(int numberOfSegments, int aggregation) {
    for(int index = 0; index < numberOfSegments; index = index + 2) {
        int lowerIndex = index * (elementsPerThread * aggregation);
        int rightIndex = ((index + 2) * elementsPerThread * aggregation) - 1;
        int middleIndex = lowerIndex + (elementsPerThread * aggregation) - 1;
        
        if (rightIndex >= arraySize)
            rightIndex = arraySize - 1;
        
        merge(lowerIndex, middleIndex, rightIndex);
    }
    if (numberOfSegments / 2 >= 1)
        mergeFinalArray(numberOfSegments / 2, aggregation * 2);
}



/**
 * @brief classic merge sort that will sort each part of array (divided in threads)
 * 
 * @param lowerIndex: lower index of given array section (one section per thread)
 * @param higherIndex: higher index of given array section (one section per thread)
 */
void mergeSort(int lowerIndex, int higherIndex) {
    if (lowerIndex < higherIndex) {
        int middleIndex = lowerIndex + (higherIndex - lowerIndex) / 2;
        
        mergeSort(lowerIndex, middleIndex);
        mergeSort(middleIndex + 1, higherIndex);
        
        merge(lowerIndex, middleIndex, higherIndex);
    }
}



/**
 * @brief configure the bounds of the array applies merge sort in each thread
 * 
 * @param arguments: pointer containing the thread id
 */
void *multiThreadMergeSort(void* arguments) {
    int threadID = (long)arguments;

    int lowerIndex = threadID * (elementsPerThread);
    int higherIndex = (threadID + 1) * (elementsPerThread) - 1;
    
    if (threadID == numbersOfThreads - 1) higherIndex += offset;
    
    int middleIndex = lowerIndex + (higherIndex - lowerIndex) / 2;
    
    if (lowerIndex < higherIndex) {
        mergeSort(lowerIndex, higherIndex);
        mergeSort(lowerIndex + 1, higherIndex);
        merge(lowerIndex, middleIndex, higherIndex);
    }

    return NULL;
}


/**
 * @brief splits the array in different segments and delegate each segment to one thread
 * 
 */
void mergeSegments() {
    pthread_t threads[numbersOfThreads];

    for (long i = 0; i < numbersOfThreads; i ++) {
        int createdThread = pthread_create(&threads[i], NULL, multiThreadMergeSort, (void *)i);

        if(createdThread) printf("An error has ocurred.");
    }
    
    for(long i = 0; i < numbersOfThreads; i++)
        pthread_join(threads[i], NULL);
}



/**
 * @brief read input values form vetor.dat and allocate them in the array
 * 
 */
void generateInput() {
    FILE *input = fopen("./vetor.dat", "r");

    double readedNumber = 0;

    array = (double *) malloc(0 * sizeof(double));

    while(fscanf(input, "%lf", &readedNumber) != EOF) {
        arraySize++;

        array = realloc(array, arraySize * sizeof(double));

        array[arraySize-1] = readedNumber;
    }

    fclose(input);
}



int main() {
    struct timespec initialTime, finalTime;

    numbersOfThreads = 8;

    generateInput();

    elementsPerThread = arraySize / numbersOfThreads;
    offset = arraySize % numbersOfThreads;

    clock_gettime(CLOCK_REALTIME, &initialTime);

    mergeSegments();

    mergeFinalArray(numbersOfThreads, 1);

    clock_gettime(CLOCK_REALTIME, &finalTime);

    printTime(initialTime, finalTime);

    return 0;
}
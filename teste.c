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

void test_array_is_in_order() {
    double max = 0;
    for (int i = 1; i < arraySize; i ++) {
        if (array[i] >= array[i - 1]) {
            max = array[i];
        } else {
            printf("Error. Out of order sequence: %lf found\n", array[i]);
            return;
        }
    }
    printf("Array is in sorted order\n");
}


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



void printTime(struct timespec initialTime, struct timespec finalTime) {
  struct timeRegistry searchRegistry;
  struct timeRegistry sortRegistry;

  searchRegistry = getTimeRegistry(initialTime, finalTime);

  printf("Tempo de busca: %ld.%09ld\n", searchRegistry.seconds, searchRegistry.nanoseconds);
}

void merge(int left, int middle, int right) {
    int i = 0;
    int j = 0;
    int k = 0;

    int left_length = middle - left + 1;
    int right_length = right - middle;
    
    double left_array[left_length];
    double right_array[right_length];

    for (int i = 0; i < left_length; i ++)
        left_array[i] = array[left + i];
    
    for (int j = 0; j < right_length; j ++)
        right_array[j] = array[middle + 1 + j];
    
    i = 0;
    j = 0;

    while (i < left_length && j < right_length) {
        if (left_array[i] <= right_array[j]) {
            array[left + k] = left_array[i];
            i ++;
        } else {
            array[left + k] = right_array[j];
            j ++;
        }
        k ++;
    }
    
    while (i < left_length) {
        array[left + k] = left_array[i];
        k ++;
        i ++;
    }

    while (j < right_length) {
        array[left + k] = right_array[j];
        k ++;
        j ++;
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

    pthread_t threads[numbersOfThreads];

    clock_gettime(CLOCK_REALTIME, &initialTime);

    for (long i = 0; i < numbersOfThreads; i ++) {
        int createdThread = pthread_create(&threads[i], NULL, multiThreadMergeSort, (void *)i);
    }
    
    for(long i = 0; i < numbersOfThreads; i++)
        pthread_join(threads[i], NULL);

    mergeFinalArray(numbersOfThreads, 1);

    clock_gettime(CLOCK_REALTIME, &finalTime);

    printTime(initialTime, finalTime);

    test_array_is_in_order();

    return 0;
}
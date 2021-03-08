#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>


double *array;
int arraySize;


struct thread {
    int number;
    int lowerIndex;
    int higherIndex;
} typedef ThreadData;

struct timeRegistry {
  long int seconds;
  long int nanoseconds;
};



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



void merge(int lowerIndex, int middleIndex, int higherIndex) {
    int leftIndex = 0; 
    int rightIndex = 0;
    
    int leftSize = middleIndex - lowerIndex + 1;
    int rightSize = higherIndex - middleIndex;

    int arrayIndex = lowerIndex;

    double *left = malloc(leftSize * sizeof(double));
    double *right = malloc(rightSize * sizeof(double));

    for (int index = 0; index < leftSize; index++)
        left[index] = array[index + lowerIndex];

    for (int index = 0; index < rightSize; index++)
        right[index] = array[index + middleIndex + 1];

    while (leftIndex < leftSize && rightIndex < rightSize) {
        if (left[leftIndex] <= right[rightIndex])
            array[arrayIndex++] = left[leftIndex++];
        else
            array[arrayIndex++] = right[rightIndex++];
    }

    while (leftIndex < leftSize)
        array[arrayIndex++] = left[leftIndex++];

    while (rightIndex < rightSize)
        array[arrayIndex++] = right[rightIndex++];

    free(left);
    free(right);
}



void mergeSort(int lowerIndex, int higherIndex) {
    int middleIndex = lowerIndex + (higherIndex - lowerIndex) / 2;

    if (lowerIndex < higherIndex) {
        mergeSort(lowerIndex, middleIndex);
        mergeSort(middleIndex + 1, higherIndex);

        merge(lowerIndex, middleIndex, higherIndex);
    }
}


void *multipleThreadsMergeSort(void *argument) {
    ThreadData *thread = argument;

    int lowerIndex = thread->lowerIndex;
    int higherIndex = thread->higherIndex;

    int middleIndex = lowerIndex + (higherIndex - lowerIndex) / 2;

    if (lowerIndex < higherIndex) {
        mergeSort(lowerIndex, middleIndex);
        mergeSort(middleIndex + 1, higherIndex);
        merge(lowerIndex, middleIndex, higherIndex);
    }

    return 0;
}



int main() {
    int threadsNumber = 8;

    struct timespec initialTime, finalTime;

    generateInput();

    pthread_t threads[threadsNumber];

    ThreadData threadsData[threadsNumber];
    ThreadData *threadData;

    int length = arraySize / threadsNumber;

    int lowerIndex = 0;

    for (int index = 0; index < threadsNumber; index++, lowerIndex += length) {
        threadData = &threadsData[index];
        threadData->number = index;

        threadData->lowerIndex = lowerIndex;
        threadData->higherIndex = lowerIndex + length - 1;
        
        if (index == (threadsNumber - 1)) threadData->higherIndex = arraySize - 1;
    }

    clock_gettime(CLOCK_REALTIME, &initialTime);

    for (int index = 0; index < threadsNumber; index++) {
        threadData = &threadsData[index];

        pthread_create(&threads[index], NULL, multipleThreadsMergeSort, threadData);
    }

    for (int index = 0; index < threadsNumber; index++) pthread_join(threads[index], NULL);
    
    ThreadData *threadMerge = &threadsData[0];

    for (int index = 1; index < threadsNumber; index++) {
        ThreadData *threadData = &threadsData[index];

        merge(threadMerge->lowerIndex, threadData->lowerIndex - 1, threadData->higherIndex);
    }

    clock_gettime(CLOCK_REALTIME, &finalTime);
    

    printTime(initialTime, finalTime);

    return 0;
}
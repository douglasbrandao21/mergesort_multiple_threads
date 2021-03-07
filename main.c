#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

// number of elements in array
//#define MAX 15
//#define MAX 16
int MAX;

// number of threads
//#define THREAD_MAX 4
int THREAD_MAX;

//using namespace std;

// array of size MAX
double *array;
int arraySize;

struct thread {
    int number;
    int lowerIndex;
    int higherIndex;
} typedef Thread;


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
    Thread *thread = argument;

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

// Driver Code
int main() {
    MAX = 10;
    THREAD_MAX = 8;

    generateInput();

    pthread_t threads[THREAD_MAX];

    Thread threadsList[THREAD_MAX];
    Thread *thread;

    int length = MAX / THREAD_MAX;

    int lowerIndex = 0;

    for (int i = 0; i < THREAD_MAX; i++, lowerIndex += length) {
        thread = &threadsList[i];
        thread->number = i;

        thread->lowerIndex = lowerIndex;
        thread->higherIndex = lowerIndex + length - 1;
        
        if (i == (THREAD_MAX - 1)) thread->higherIndex = MAX - 1;
    }

    // creating 4 threads
    for (int i = 0; i < THREAD_MAX; i++) {
        thread = &threadsList[i];

        pthread_create(&threads[i], NULL, multipleThreadsMergeSort, thread);
    }

    // joining all 4 threads
    for (int i = 0; i < THREAD_MAX; i++) pthread_join(threads[i], NULL);
    
    Thread *threadMerge = &threadsList[0];

    for (int i = 1; i < THREAD_MAX; i++) {
        Thread *thread = &threadsList[i];

        merge(threadMerge->lowerIndex, thread->lowerIndex - 1, thread->higherIndex);
    }
    

    // displaying sorted array
    printf("\n\nSorted array:\n");

    for (int i = 0; i < MAX; i++)
        printf("%lf\n", array[i]);
    
    printf("\n");

    return 0;
}
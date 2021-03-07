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
double *a;
int arraySize;

struct thread {
    int number;
    int lowerIndex;
    int higherIndex;
} typedef Thread;

void generateInput() {
    FILE *input = fopen("./vetor.dat", "r");

    double readedNumber = 0;

    a = (double *) malloc(0 * sizeof(double));

    while(fscanf(input, "%lf", &readedNumber) != EOF) {
        arraySize++;

        a = realloc(a, arraySize * sizeof(double));

        a[arraySize-1] = readedNumber;
    }

    fclose(input);
}


void merge(int low, int mid, int high) {

    // n1 is size of left part and n2 is size of right part
    int n1 = mid - low + 1;
    int n2 = high - mid;

    double *left = malloc(n1 * sizeof(double));
    double *right = malloc(n2 * sizeof(double));

    int i;
    int j;

    // storing values in left part
    for (i = 0; i < n1; i++)
        left[i] = a[i + low];

    // storing values in right part
    for (i = 0; i < n2; i++)
        right[i] = a[i + mid + 1];

    int k = low;

    i = j = 0;

    // merge left and right in ascending order
    while (i < n1 && j < n2) {
        if (left[i] <= right[j])
            a[k++] = left[i++];
        else
            a[k++] = right[j++];
    }

    // insert remaining values from left
    while (i < n1)
        a[k++] = left[i++];

    // insert remaining values from right
    while (j < n2)
        a[k++] = right[j++];

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
    THREAD_MAX = 4;

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
        printf("%lf\n", a[i]);
    
    printf("\n");

    return 0;
}
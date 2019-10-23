#include "merge_sort.h"
#include <iostream>
#include <pthread.h>
#include <string>
#include <stdio.h>
#include <vector>

using namespace std;

const char* FILE_PATH = "merge-sort-readme.txt";
const int THREADS_BRANCHES = 2;

vector<int> unsorted_array;

merge_sort::merge_sort()
{

}

merge_sort::~merge_sort()
{

}

void* mergeSortThreads(void *arguments);
void merge_partitions(int low, int mid, int high);

void merge_sort::mergeSortInitialize() {

    freopen(FILE_PATH, "r", stdin);

    int arraySize;
    cin >> arraySize;

    unsorted_array.resize(arraySize);
    pair<int, int> range = make_pair(0, arraySize - 1);

    for (int i = 0; i < arraySize; i++) {
        cin >> unsorted_array[i];
    }

    pthread_t thread;
    pthread_create(&thread, NULL, mergeSortThreads, &range);
    pthread_join(thread, NULL);

    for (int i = 0; i < arraySize; i++) {
        cout << unsorted_array[i] << " ";
    }
    cout << endl;

    fclose(stdin);
}

void* mergeSortThreads(void *arguments) {

    pair<int, int> range = *((pair<int, int>*) arguments);
    int low = range.first;
    int high = range.second;
    int mid = (low + high) / 2;

    pthread_t lowThread, highThread;
    pair<int, int> leftHalf = make_pair(low, mid);
    pair<int, int> rightHalf = make_pair(mid + 1, high);

    if (low < high) {
        pthread_create(&lowThread, NULL, mergeSortThreads, &leftHalf);
        pthread_create(&highThread, NULL, mergeSortThreads, &rightHalf);

        pthread_join(lowThread, NULL);
        pthread_join(highThread, NULL);

        merge_partitions(low, mid, high);
    }

    return 0;
}

void merge_partitions(int low, int mid, int high) {
    vector<int> sorted_array(high - low + 1, 0);
    int index = 0;
    int lowOne = low, highOne = mid;
    int lowTwo = mid + 1, highTwo = high;
    while (lowOne <= highOne && lowTwo <= highTwo) {
        if (unsorted_array[lowOne] < unsorted_array[lowTwo]) {
            sorted_array[index++] = unsorted_array[lowOne++];
        } else {
            sorted_array[index++] = unsorted_array[lowTwo++];
        }
    }

    while (lowOne <= highOne) sorted_array[index++] = unsorted_array[lowOne++];
    while (lowTwo <= highTwo) sorted_array[index++] = unsorted_array[lowTwo++];

    for (int i = 0; i < high - low + 1; i++) {
        unsorted_array[low + i] = sorted_array[i];
    }
}

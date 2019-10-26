#include "matrix_multiplication.h"
#include <vector>
#include <iostream>
#include <time.h>

using namespace std;

const char* INPUT_FILE = "matrix-multiplication-input.txt";
const char* OUTPUT_FILE = "matrix-multiplication-output.txt";

vector< vector<int> > matrixA;
vector< vector<int> > matrixB;
vector< vector<int> > matrixC;

pair<int, int> sizeA;
pair<int, int> sizeB;

pair<int, int> readMatrix(vector <vector<int> > &);
void writeFile(pair<clock_t, clock_t>, bool);
void* elementMatrixMultiplicationThread(void*);
void* rowMatrixMultiplicationThread(void* arguments);
pair<clock_t, clock_t> elementMatrixMultiplication();
pair<clock_t, clock_t> rowMatrixMultiplication();

matrix_multiplication::matrix_multiplication()
{
}

matrix_multiplication::~matrix_multiplication()
{
}

/**
* mutliplicationInitialze is responsible for calling
* main functionalities, in which it reads from a file, computes the matrix
* multiplication & finally save the results in an output file.
*/
void matrix_multiplication::multiplicationInitialize() {

    freopen(INPUT_FILE, "r", stdin);

    sizeA = readMatrix(matrixA);
    sizeB = readMatrix(matrixB);

    if (sizeA.second != sizeB.first) {
        cout << "Number of columns of the first matrix should be equal\n"
                "to the number of rows of the second matrix" << endl;
        return;
    }

    pair<clock_t, clock_t> timeRange = elementMatrixMultiplication();
    writeFile(timeRange, true);

    timeRange = rowMatrixMultiplication();
    writeFile(timeRange, false);

    fclose(stdin);
}

/**
* Read from a file the values of a given matrix
* @param matrix to be written.
* @return number of rows and columns.
*/
pair<int, int> readMatrix(vector <vector<int> > &matrix) {
    int row, column;
    cin >> row >> column;

    for (int i = 0; i < row; i++) {
        vector<int> rowVector(column);
        for (int j = 0; j < column; j++) {
            cin >> rowVector[j];
        }
        matrix.push_back(rowVector);
    }
    return make_pair(row, column);
}

/**
* Write the output of the matrix multiplication in a file.
* @param timeRange is the time elapsed for each matrix multiplication
* algorithm.
* @param isRecreate; if true neglect file if exists and create(overwrite) a new file
* else append contents (might create a new file if there isn't a file).
*/
void writeFile(pair<clock_t, clock_t> timeRange, bool isRecreate) {
    freopen(OUTPUT_FILE, ((isRecreate) ? "w": "a+"), stdout);
    for (int i = 0; i < sizeA.first; i++) {
        for (int j = 0; j < sizeB.second; j++) {
            cout << matrixC[i][j] << " ";
        }
        cout << endl;
    }
    cout << (timeRange.second - timeRange.first) / (double) CLOCKS_PER_SEC << endl;
    fclose(stdout);
}

/**
* Computes matrix multiplication for every element of the
* output matrix concurrently using threads.
* @return the time elapsed to complete computation.
*/
pair<clock_t, clock_t> elementMatrixMultiplication() {

    pthread_t threads[sizeA.first][sizeB.second];
    matrixC.clear();
    vector<int> row(sizeB.second, 0);
    matrixC.resize(sizeA.first, row);

    clock_t timeBefore = clock();
    for (int i = 0; i < sizeA.first; i++) {
        for (int j = 0; j < sizeB.second; j++) {
            pair<int, int>* index = new pair<int, int>(i, j);
            pthread_create(&threads[i][j], NULL, elementMatrixMultiplicationThread, index);
        }
    }

    for (int i = 0; i < sizeA.first; i++) {
        for (int j = 0; j < sizeB.second; j++) {
            pthread_join(threads[i][j], NULL);
        }
    }
    clock_t timeAfter = clock();

    return make_pair(timeBefore, timeAfter);
}

/**
* elementMatrixMultiplicationThread is called as a thread that handles
* matrix multiplication for every element of the output matrix.
* @param arguments are pointers to the required indicies in matrix
* multiplication.
*/
void* elementMatrixMultiplicationThread(void* arguments) {

    pair<int, int> index = *((pair<int, int>*) arguments);
    for (int i = 0; i < sizeA.second; i++) {
        matrixC[index.first][index.second] += matrixA[index.first][i] * matrixB[i][index.second];
    }

    return 0;
}

/**
* Computes matrix multiplication for every row of the
* output matrix concurrently using threads.
* @return the time elapsed to complete computation.
*/
pair<clock_t, clock_t> rowMatrixMultiplication() {

    pthread_t threads[sizeA.first];
    matrixC.clear();
    vector<int> row(sizeB.second, 0);
    matrixC.resize(sizeA.first, row);

    clock_t timeBefore = clock();
    for (int i = 0; i < sizeA.first; i++) {
        int* index = new int;
        *index = i;
        pthread_create(&threads[i], NULL, rowMatrixMultiplicationThread, index);
    }

    for (int i = 0; i < sizeA.first; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_t timeAfter = clock();

    return make_pair(timeBefore, timeAfter);
}

/**
* rowMatrixMultiplicationThread is called as a thread that handles
* matrix multiplication for every row of the output matrix.
* @param arguments are pointers to the required indicies in matrix
* multiplication.
*/
void* rowMatrixMultiplicationThread(void* arguments) {

    int index = *((int*) arguments);

    for (int i = 0; i < sizeB.second; i++) {
        for (int j = 0; j < sizeA.second; j++) {
            matrixC[index][i] += matrixA[index][j] * matrixB[j][i];
        }
    }

    return 0;
}

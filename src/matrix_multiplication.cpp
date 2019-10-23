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
void* rowMatrixMultiplication(void* arguments);
pair<clock_t, clock_t> elementMatrixMultiplication();
pair<clock_t, clock_t> rowMatrixMultiplication();

matrix_multiplication::matrix_multiplication()
{
}

matrix_multiplication::~matrix_multiplication()
{
}

void matrix_multiplication::multiplicationInitialize() {

    freopen(INPUT_FILE, "r", stdin);

    sizeA = readMatrix(matrixA);
    sizeB = readMatrix(matrixB);

    pair<clock_t, clock_t> timeRange = elementMatrixMultiplication();
    writeFile(timeRange, true);

    timeRange = rowMatrixMultiplication();
    writeFile(timeRange, false);

    fclose(stdin);
}

pair<int, int> readMatrix(vector <vector<int> > &matrix) {
    int row, column;
    cin >> row >> column;

    for (int i = 0; i < row; i++) {
        vector<int> rowVector(row, 0);
        for (int j = 0; j < column; j++) {
            cin >> rowVector[j];
        }
        matrix.push_back(rowVector);
    }
    return make_pair(row, column);
}

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

void* elementMatrixMultiplicationThread(void* arguments) {

    pair<int, int> index = *((pair<int, int>*) arguments);
    for (int i = 0; i < sizeA.second; i++) {
        matrixC[index.first][index.second] += matrixA[index.first][i] * matrixB[i][index.second];
    }

    return 0;
}

pair<clock_t, clock_t> rowMatrixMultiplication() {

    pthread_t threads[sizeA.first];
    matrixC.clear();
    vector<int> row(sizeB.second, 0);
    matrixC.resize(sizeA.first, row);

    clock_t timeBefore = clock();
    for (int i = 0; i < sizeA.first; i++) {
        int* index = new int;
        *index = i;
        pthread_create(&threads[i], NULL, rowMatrixMultiplication, index);
    }

    for (int i = 0; i < sizeA.first; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_t timeAfter = clock();

    return make_pair(timeBefore, timeAfter);
}

void* rowMatrixMultiplication(void* arguments) {

    int index = *((int*) arguments);

    for (int i = 0; i < sizeB.second; i++) {
        for (int j = 0; j < sizeA.second; j++) {
            matrixC[index][i] += matrixA[index][j] * matrixB[j][i];
        }
    }

    return 0;
}

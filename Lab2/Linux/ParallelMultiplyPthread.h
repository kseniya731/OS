#pragma once
#include "MatrixUtils.h"
#include <pthread.h>

void parallelMultiplyPthread(const Matrix& A, const Matrix& B, Matrix& C, int block_size);

struct ThreadDataPthread {
    const Matrix* A;
    const Matrix* B;
    Matrix* C;
    int block_i;
    int block_j;
    int block_size;
    int matrix_size;
    pthread_mutex_t* cout_mutex;
};

void* computeBlockPthread(void* arg);

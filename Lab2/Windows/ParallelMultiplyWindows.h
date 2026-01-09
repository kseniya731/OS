#pragma once
#include "MatrixUtils.h"
#include <Windows.h>

void parallelMultiplyWindows(const Matrix& A, const Matrix& B, Matrix& C, int block_size);

struct ThreadDataWindows {
    const Matrix* A;
    const Matrix* B;
    Matrix* C;
    int block_i;
    int block_j;
    int block_size;
    int matrix_size;
    HANDLE cout_mutex;
};

DWORD WINAPI computeBlockWindows(LPVOID lpParam);
#include "ParallelMultiplyWindows.h"
#include <iostream>

void parallelMultiplyWindows(const Matrix& A, const Matrix& B, Matrix& C, int block_size) {
    int matrix_size = A.size();
    int blocks_number_i = calculateBlocksNumber(matrix_size, block_size);
    int blocks_number_j = calculateBlocksNumber(matrix_size, block_size);

    HANDLE cout_mutex = CreateMutex(NULL, FALSE, NULL);

    std::vector<HANDLE> threads;

    for (int i = 0; i < blocks_number_i; i++) {
        for (int j = 0; j < blocks_number_j; j++) {
            ThreadDataWindows* data = new ThreadDataWindows;
            data->A = &A;
            data->B = &B;
            data->C = &C;
            data->block_i = i;
            data->block_j = j;
            data->block_size = block_size;
            data->matrix_size = matrix_size;
            data->cout_mutex = cout_mutex;

            HANDLE thread = CreateThread(NULL, 0, computeBlockWindows, data, 0, NULL);
            if (thread == NULL) {
                std::cerr << "Error creating thread for block (" << i << ", " << j << ")" << std::endl;
                delete data;
                continue;
            }
            threads.push_back(thread);
        }
    }

    WaitForMultipleObjects(static_cast<DWORD>(threads.size()), threads.data(), TRUE, INFINITE);

    for (HANDLE thread : threads) {
        CloseHandle(thread);
    }
    if (cout_mutex != NULL) {
        CloseHandle(cout_mutex);
    }
}

DWORD WINAPI computeBlockWindows(LPVOID lpParam) {
    ThreadDataWindows* data = (ThreadDataWindows*)lpParam;

    const Matrix& A = *(data->A);
    const Matrix& B = *(data->B);
    Matrix& C = *(data->C);
    int block_i = data->block_i;
    int block_j = data->block_j;
    int block_size = data->block_size;
    int matrix_size = data->matrix_size;
    HANDLE cout_mutex = data->cout_mutex;

    int start_i = block_i * block_size;
    int end_i = (start_i + block_size < matrix_size) ? start_i + block_size : matrix_size;
    int start_j = block_j * block_size;
    int end_j = (start_j + block_size < matrix_size) ? start_j + block_size : matrix_size;

    std::vector<std::vector<int>> local_block(end_i - start_i, std::vector<int>(end_j - start_j, 0));

    for (int k_block = 0; k_block < calculateBlocksNumber(matrix_size, block_size); k_block++) {
        int k_start = k_block * block_size;
        int k_end = (k_start + block_size < matrix_size) ? k_start + block_size : matrix_size;
        for (int i = start_i; i < end_i; i++) {
            for (int j = start_j; j < end_j; j++) {
                int sum = 0;
                for (int k = k_start; k < k_end; k++) {
                    sum += A[i][k] * B[k][j];
                }
                local_block[i - start_i][j - start_j] += sum;
            }
        }
    }

    WaitForSingleObject(cout_mutex, INFINITE);
    for (int i = start_i; i < end_i; i++) {
        for (int j = start_j; j < end_j; j++) {
            C[i][j] = local_block[i - start_i][j - start_j];
        }
    }
    ReleaseMutex(cout_mutex);

    delete data;
    return 0;
}
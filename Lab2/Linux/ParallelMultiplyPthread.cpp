#include "../include/ParallelMultiplyPthread.h"
#include <iostream>
#include <vector>

void parallelMultiplyPthread(const Matrix& A, const Matrix& B, Matrix& C, int block_size) {
    int matrix_size = static_cast<int>(A.size());
    int blocks_number = calculateBlocksNumber(matrix_size, block_size);

    pthread_mutex_t cout_mutex;
    pthread_mutex_init(&cout_mutex, NULL);

    std::vector<pthread_t> threads;
    std::vector<ThreadDataPthread*> thread_data;

    for (int i = 0; i < matrix_size; i++) {
        for (int j = 0; j < matrix_size; j++) {
            C[i][j] = 0;
        }
    }

    for (int i = 0; i < blocks_number; i++) {
        for (int j = 0; j < blocks_number; j++) {
            ThreadDataPthread* data = new ThreadDataPthread;
            data->A = &A;
            data->B = &B;
            data->C = &C;
            data->block_i = i;
            data->block_j = j;
            data->block_size = block_size;
            data->matrix_size = matrix_size;
            data->cout_mutex = &cout_mutex;

            pthread_t thread;
            pthread_create(&thread, NULL, computeBlockPthread, data);
            threads.push_back(thread);
            thread_data.push_back(data);
        }
    }

    for (pthread_t thread : threads) {
        pthread_join(thread, NULL);
    }

    for (ThreadDataPthread* data : thread_data) {
        delete data;
    }
    pthread_mutex_destroy(&cout_mutex);
}

void* computeBlockPthread(void* arg) {
    ThreadDataPthread* data = static_cast<ThreadDataPthread*>(arg);

    const Matrix& A = *(data->A);
    const Matrix& B = *(data->B);
    Matrix& C = *(data->C);
    int block_i = data->block_i;
    int block_j = data->block_j;
    int block_size = data->block_size;
    int matrix_size = data->matrix_size;
    pthread_mutex_t* cout_mutex = data->cout_mutex;

    int start_i = block_i * block_size;
    int end_i = std::min(start_i + block_size, matrix_size);
    int start_j = block_j * block_size;
    int end_j = std::min(start_j + block_size, matrix_size);

    pthread_mutex_lock(cout_mutex);
    pthread_mutex_unlock(cout_mutex);

    int num_k_blocks = calculateBlocksNumber(matrix_size, block_size);
    for (int k_block = 0; k_block < num_k_blocks; k_block++) {
        int k_start = k_block * block_size;
        int k_end = std::min(k_start + block_size, matrix_size);
        
        for (int i = start_i; i < end_i; i++) {
            for (int j = start_j; j < end_j; j++) {
                int sum = 0;
                for (int k = k_start; k < k_end; k++) {
                    sum += A[i][k] * B[k][j];
                }
                C[i][j] += sum;
            }
        }
    }

    return nullptr;
}

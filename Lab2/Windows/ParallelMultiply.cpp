#include "ParallelMultiply.h"
#include <iostream>

void parallelMultiplyThread(const Matrix& A, const Matrix& B, Matrix& C, int block_size) {
	int matrix_size = A.size();
	int blocks_number = calculateBlocksNumber(matrix_size, block_size);

	std::mutex cout_mutex;

	std::vector<std::thread> threads; // vector for streams

	for (int i = 0; i < blocks_number; i++) {
		for (int j = 0; j < blocks_number; j++) {
			threads.emplace_back(computeBlock,
				std::cref(A), std::cref(B), std::ref(C),
				i, j, block_size, matrix_size,
				std::ref(cout_mutex));
		}
	}

	for (auto& thread : threads) {
		thread.join();
	}
}

void computeBlock(const Matrix& A, const Matrix& B, Matrix& C,
    int block_i, int block_j, int block_size,
    int matrix_size, std::mutex& cout_mutex) {
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

    std::lock_guard<std::mutex> lock(cout_mutex);
    for (int i = start_i; i < end_i; i++) {
        for (int j = start_j; j < end_j; j++) {
            C[i][j] = local_block[i - start_i][j - start_j];
        }
    }
}
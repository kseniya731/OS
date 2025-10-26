#pragma once
#include "MatrixUtils.h"
#include <thread>
#include <mutex>

void parallelMultiplyThread(const Matrix& A, const Matrix& B, Matrix& C, int block_size);
void computeBlock(const Matrix& A, const Matrix& B, Matrix& C,
	int block_i, int block_j, int block_size,
	int matrix_size, std::mutex& cout_mutex);
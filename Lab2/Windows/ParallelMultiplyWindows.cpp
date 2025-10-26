#include "ParallelMultiplyWindows.h"
#include <iostream>

void parallelMultiplyWindows(const Matrix& A, const Matrix& B, Matrix& C, int block_size) {
	int matrix_size = A.size();
	int blocks_number = calculateBlocksNumber(matrix_size, block_size);

	HANDLE cout_mutex = CreateMutex(NULL, FALSE, NULL);
	/*if (cout_mutex == NULL) {
		std::cerr << "Error creating mutex" << std::endl;
		return;
	}*/

	std::vector<HANDLE> threads;

	for (int i = 0; i < blocks_number; i++) {
		ThreadDataWindows* data = new ThreadDataWindows;
		data->A = &A;
		data->B = &B;
		data->C = &C;
		data->block_i = i;
		data->matrix_size = matrix_size;
		data->block_size = block_size;
		data->cout_mutex = cout_mutex;

		HANDLE thread = CreateThread(NULL, 0, computeBlockWindows, data, 0, NULL);
		if (thread == NULL) {
			std::cerr << "Error creating thread for block " << i << std::endl;
			delete data;
			continue;
		}
		threads.push_back(thread);
	}

	WaitForMultipleObjects(static_cast<DWORD>(threads.size()), threads.data(), TRUE, INFINITE);

	for (HANDLE thread : threads) {
		CloseHandle(thread);
	}
	if (cout_mutex != 0) {
		CloseHandle(cout_mutex);
	}
}

DWORD WINAPI computeBlockWindows(LPVOID lpParam) {
	ThreadDataWindows* data = (ThreadDataWindows*)lpParam;

	const Matrix& A = *(data->A);
	const Matrix& B = *(data->B);
	Matrix& C = *(data->C);
	int block_i = data->block_i;
	int block_size = data->block_size;
	int matrix_size = data->matrix_size;
	HANDLE cout_mutex = data->cout_mutex;

	int start_i = block_i * block_size;
	int end_i = (start_i + block_size < matrix_size) ? start_i + block_size : matrix_size;

	/*if (start_i >= matrix_size) {
		delete data;
		return 1;
	}*/

	{
		WaitForSingleObject(cout_mutex, INFINITE);
		ReleaseMutex(cout_mutex);
	}

	for (int i = start_i; i < end_i; i++) {
		for (int j = 0; j < matrix_size; j++) {
			int sum = 0;
			for (int k = 0; k < matrix_size; k++) {
				sum += A[i][k] * B[k][j];
			}
			C[i][j] = sum;
		}
	}

	delete data;
	return 0;
}
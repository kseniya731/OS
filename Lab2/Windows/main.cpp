#include <iostream>
#include <chrono>

#include "ParallelMultiply.h"
#include "ParallelMultiplyWindows.h"

void countSpeed();

int main() {
	int N = 5;
	int block_size = 2;

	Matrix A = createMatrix(N);
	Matrix B = createMatrix(N);
	Matrix C_seq = createMatrix(N);
	Matrix C_par = createMatrix(N);
	Matrix C_parWin = createMatrix(N);

	initializeMatrix(A);
	initializeMatrix(B);

	std::cout << "Matrix A:" << std::endl;
	printMatrix(A);
	std::cout << "Matrix B:" << std::endl;
	printMatrix(B);

	sequenceMultiply(A, B, C_seq);
	std::cout << "Matrix C_seq:" << std::endl;
	printMatrix(C_seq);

	parallelMultiplyThread(A, B, C_par, block_size);
	std::cout << "Matrix C_par:" << std::endl;
	printMatrix(C_par);

	parallelMultiplyWindows(A, B, C_parWin, block_size);
	std::cout << "Matrix C_parWin:" << std::endl;
	printMatrix(C_parWin);

	std::cout << "C_seq equals C_par equals C_parWin (1~True, 0~False): " << (areMatricesEqual(C_seq, C_par) && areMatricesEqual(C_seq, C_parWin)) << std::endl;

	countSpeed();

	return 0;
}

void countSpeed() {
	int N = 100;
	std::vector<int> block_sizes = { 1, 2, 5, 10, 25, 50, 75, 100 };

	Matrix A = createMatrix(N);
	Matrix B = createMatrix(N);
	initializeMatrix(A);
	initializeMatrix(B);

	std::cout << "BlockSize | StreamsCount | Seq(ms) | Thread(ms) | Windows.h(ms) " << std::endl;

	for (int block_size : block_sizes) {
		int blocks = calculateBlocksNumber(N, block_size);
		int streams_count = blocks * blocks;

		Matrix C1 = createMatrix(N), C2 = createMatrix(N), C3 = createMatrix(N);

		auto start = std::chrono::high_resolution_clock::now();
		sequenceMultiply(A, B, C1);
		auto seq_time = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start).count();

		start = std::chrono::high_resolution_clock::now();
		parallelMultiplyThread(A, B, C2, block_size);
		auto thread_time = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start).count();

		start = std::chrono::high_resolution_clock::now();
		parallelMultiplyWindows(A, B, C3, block_size);
		auto windows_time = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start).count();

		std::cout << block_size << " | " << streams_count << " | " << seq_time << " | " << thread_time << " | " << windows_time << std::endl;
	}
}
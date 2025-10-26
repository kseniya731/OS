#include "MatrixUtils.h"
#include <iostream>
#include <random>

Matrix createMatrix(int n) {
	Matrix matrix(n, std::vector<int>(n));
	return matrix;
}

void initializeMatrix(Matrix& matrix) {
	for (int i = 0; i < matrix.size(); i++) {
		for (int j = 0; j < matrix[i].size(); j++) {
			matrix[i][j] = rand() % 30 - 15;
		}
	}
}

void printMatrix(const Matrix& matrix) {
	for (std::vector<int> vect : matrix) {
		for (int i : vect) {
			std::cout << i << "\t";
		}
		std::cout << std::endl;
	}
}

void sequenceMultiply(const Matrix& A, const Matrix& B, Matrix& C) {
	int n = A.size();
	for (int k = 0; k < n; k++) {
		for (int i = 0; i < n; i++) {
			int el = 0;
			for (int j = 0; j < A[i].size(); j++) {
				el += A[i][j] * B[j][k];
			}
			C[i][k] = el;
		}
	}
}

bool areMatricesEqual(const Matrix& A, const Matrix& B) {
	if (A.size() != B.size()) { return false; }

	for (int i = 0; i < A.size(); i++) {
		if (A[i].size() != B[i].size()) { return false; }
		for (int j = 0; j < A[i].size(); j++) {
			if (A[i][j] != B[i][j]) { return false; }
		}
	}

	return true;
}

int calculateBlocksNumber(int matrix_size, int block_size) {
	return (matrix_size + block_size - 1) / block_size;
}
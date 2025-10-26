#pragma once
#include <vector>

using Matrix = std::vector<std::vector<int>>;

Matrix createMatrix(int n);
void initializeMatrix(Matrix& matrix);
void printMatrix(const Matrix& matrix);
void sequenceMultiply(const Matrix& A, const Matrix& B, Matrix& C);
bool areMatricesEqual(const Matrix& A, const Matrix& B);
int calculateBlocksNumber(int matrix_size, int block_size);
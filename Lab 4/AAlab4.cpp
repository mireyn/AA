#include <iostream>
#include <ctime>
#include <thread>
#include <algorithm>
#include <chrono> 


#define OK 0
#define FAIL -1

int createMatrix(int*** mtrx, int n, int m)
{
	(*mtrx) = new int* [n];
	if (!(*mtrx))
		return FAIL;

	int f = 0;
	int i = 0;
	for (; i < n && !f; i++)
	{
		(*mtrx)[i] = new int[m];
		if (!(*mtrx)[i])
			f = 1;
	}

	if (f)
	{
		for (int j = 0; j < i; j++)
			delete[](*mtrx)[i];
		delete[](*mtrx);
	}

	return OK;
}

void deleteMatrix(int** mtrx, int n)
{
	for (int i = 0; i < n; i++)
		delete[] mtrx[i];
	delete[] mtrx;
}

void inputMatrix(int** mtrx, int n, int m)
{
	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++)
			std::cin >> mtrx[i][j];
}

void outputMatrix(int** mtrx, int n, int m)
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
			std::cout << mtrx[i][j] << " ";
		std::cout << std::endl;
	}
}

void fillMatrix(int** mtrx, int n, int m)
{
	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++)
			mtrx[i][j] = (i + j + 17) * (i + j - 13) - 13;
}


int vinogradMulti(int** A, int** B, int** result, int N_a, int M_a, int M_b, int dummy)
{
	int* mulH = new int[N_a];
	int* mulV = new int[M_b];

	int buf;
	for (int i = 0; i < N_a; i++)
	{
		buf = 0;
		for (int k = 1; k < M_a; k += 2)
			buf -= A[i][k - 1] * A[i][k];
		mulH[i] = buf;
	}

	for (int j = 0; j < M_b; j++)
	{
		buf = 0;
		for (int k = 1; k < M_a; k += 2)
			buf -= B[k - 1][j] * B[k][j];
		mulV[j] = buf;
	}

	for (int i = 0; i < N_a; i++)
		for (int j = 0; j < M_b; j++)
		{
			buf = mulH[i] + mulV[j];
			for (int k = 1; k < M_a; k += 2)
				buf += (A[i][k - 1] + B[k][j]) * (A[i][k] + B[k - 1][j]);

			if (M_a % 2)
				buf += A[i][M_a - 1] * B[M_a - 1][j];

			result[i][j] = buf;
		}

	return OK;
}

// Вспомогательное 1 для первого распараллеливания.
void vinogradMultiParallel11(int** A, int N_a, int M_a, int* mulH, int start, int end)
{
	int buf;
	for (int i = start; i < end; i++)
	{
		buf = 0;
		for (int k = 1; k < M_a; k += 2)
			buf -= A[i][k - 1] * A[i][k];
		mulH[i] = buf;
	}
}

// Вспомогательное 2 для первого распараллеливания.
void vinogradMultiParallel12(int** B, int M_a, int M_b, int* mulV, int start, int end)
{
	int buf;
	for (int j = start; j < end; j++)
	{
		buf = 0;
		for (int k = 1; k < M_a; k += 2)
			buf -= B[k - 1][j] * B[k][j];
		mulV[j] = buf;
	}
}

// Вспомогательное для второго распараллеливания.
void vinogradMultiParallel21(int** A, int** B, int** result, int N_a, int M_a, int M_b, int* mulH, int* mulV, int start_row, int end_row)
{
	int buf;
	for (int i = start_row; i < end_row; i++)
		for (int j = 0; j < M_b; j++)
		{
			buf = mulH[i] + mulV[j];
			for (int k = 1; k < M_a; k += 2)
				buf += (A[i][k - 1] + B[k][j]) * (A[i][k] + B[k - 1][j]);

			if (M_a % 2)
				buf += A[i][M_a - 1] * B[M_a - 1][j];

			result[i][j] = buf;
		}
}

// Первое распараллеливание.
int vinogradMultiParallel1(int** A, int** B, int** result, int N_a, int M_a, int M_b, int t_count)
{
	int* mulH = new int[N_a];
	int* mulV = new int[M_b];
	std::thread* threads = new std::thread[t_count];

	if (t_count > 1)
	{
		int proportion = t_count * N_a / (N_a + M_b);
		int rows_t = (proportion) ? proportion : 1;
		int cols_t = t_count - rows_t;

		int rows_per_t = N_a / rows_t;
		int cols_per_t = M_b / cols_t;

		int start_row = 0;
		for (int i = 0; i < rows_t; i++)
		{
			int end_row = (i == rows_t - 1) ? N_a : start_row + rows_per_t;
			threads[i] = std::thread(vinogradMultiParallel11, A, N_a, M_a, mulH, start_row, end_row);
			start_row = end_row;
		}

		int start_col = 0;
		for (int i = rows_t; i < t_count; i++)
		{
			int end_col = (i == t_count - 1) ? M_b : start_col + cols_per_t;
			threads[i] = std::thread(vinogradMultiParallel12, B, M_a, M_b, mulV, start_col, end_col);
			start_col = end_col;
		}

		for (int i = 0; i < t_count; i++)
		{
			threads[i].join();
		}
	}
	else
	{
		int buf;
		for (int i = 0; i < N_a; i++)
		{
			buf = 0;
			for (int k = 1; k < M_a; k += 2)
				buf -= A[i][k - 1] * A[i][k];
			mulH[i] = buf;
		}

		for (int j = 0; j < M_b; j++)
		{
			buf = 0;
			for (int k = 1; k < M_a; k += 2)
				buf -= B[k - 1][j] * B[k][j];
			mulV[j] = buf;
		}
	}

	int buf;
	for (int i = 0; i < N_a; i++)
		for (int j = 0; j < M_b; j++)
		{
			buf = mulH[i] + mulV[j];
			for (int k = 1; k < M_a; k += 2)
				buf += (A[i][k - 1] + B[k][j]) * (A[i][k] + B[k - 1][j]);

			if (M_a % 2)
				buf += A[i][M_a - 1] * B[M_a - 1][j];

			result[i][j] = buf;
		}

	return OK;
}


// Второе распараллеливание.
int vinogradMultiParallel2(int** A, int** B, int** result, int N_a, int M_a, int M_b, int t_count)
{
	int* mulH = new int[N_a];
	int* mulV = new int[M_b];
	std::thread* threads = new std::thread[t_count];

	int buf;
	for (int i = 0; i < N_a; i++)
	{
		buf = 0;
		for (int k = 1; k < M_a; k += 2)
			buf -= A[i][k - 1] * A[i][k];
		mulH[i] = buf;
	}

	for (int j = 0; j < M_b; j++)
	{
		buf = 0;
		for (int k = 1; k < M_a; k += 2)
			buf -= B[k - 1][j] * B[k][j];
		mulV[j] = buf;
	}

	int rows_per_t = N_a / t_count;
	int start_row = 0;
	for (int i = 0; i < t_count; i++)
	{
		int end_row = (i == t_count - 1) ? N_a : start_row + rows_per_t;
		threads[i] = std::thread(vinogradMultiParallel21, A, B, result, N_a, M_a, M_b, mulH, mulV, start_row, end_row);

		start_row = end_row;
	}
	for (int i = 0; i < t_count; i++)
	{
		threads[i].join();
	}

	return OK;
}

float getTime(int (*alg)(int**, int**, int**, int, int, int, int), int** A, int** B, int** result, int tcnt = 1, int size = 500, int itcnt = 100)
{
	fillMatrix(A, size, size);
	fillMatrix(B, size, size);

	auto start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < itcnt; i++)
		alg(A, B, result, size, size, size, tcnt);
	auto end = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	float time = duration.count() / 1000 / itcnt;
	return time;
}

void timeAnalysis()
{
	// 4 логических процессора. 
	int t_counts[6] = { 1, 2, 4, 8, 16, 32 };
	int sizes[6] = { 100, 200, 300, 400, 500 };

	float results[6][3];
	float results2[5][3];

	int** A = nullptr;
	int** B = nullptr;
	int** result = nullptr;
	int size = 500;
	int iterations = 100;
	if (createMatrix(&A, size, size) == OK)
	{
		if (createMatrix(&B, size, size) == OK)
		{
			if (createMatrix(&result, size, size) == OK)
			{
				int (*algs[3])(int**, int**, int**, int, int, int, int) = { vinogradMulti, vinogradMultiParallel1, vinogradMultiParallel2 };


				for (int si = 0; si < 6; si++)
					for (int ai = 0; ai < 3; ai++)
					{
						results[si][ai] = getTime(algs[ai], A, B, result, t_counts[si], size, iterations);
					}

				std::cout << "\nResults of the first experiment:\n";
				for (int i = 0; i < 6; i++)
				{
					for (int j = 0; j < 3; j++)
						std::cout << results[i][j] << " ";
					std::cout << std::endl;
				}

				for (int si = 0; si < 5; si++)
					for (int ai = 0; ai < 3; ai++)
					{
						results2[si][ai] = getTime(algs[ai], A, B, result, 4, sizes[si], iterations);
					}

				std::cout << "\nResults of the second experiment:\n";
				for (int i = 0; i < 5; i++)
				{
					for (int j = 0; j < 3; j++)
						std::cout << results2[i][j] << " ";
					std::cout << std::endl;
				}
			}
			else
			{
				deleteMatrix(A, size);
				deleteMatrix(B, size);
				std::cout << "\Memory error!" << std::endl;
			}
		}
		else
		{
			deleteMatrix(A, size);
			std::cout << "\Memory error!" << std::endl;
		}
	}
	else
	{
		std::cout << "\Memory error!" << std::endl;
	}
}

void taskPerformance()
{
	int n_a, m_a, n_b, m_b;

	std::cout << "Input sizes of first matrix (space separated): ";
	std::cin >> n_a >> m_a;

	std::cout << "Input sizes of second matrix (space separated): ";
	std::cin >> n_b >> m_b;

	if (m_a <= 0 || n_a <= 0 || m_b <= 0 || n_b <= 0)
	{
		std::cout << "Incorrect sizes of matricies: " << "(" << n_a \
			<< ", " << m_a << ") and (" << n_b << ", " << m_b << ")" << std::endl;
	}
	else if (m_a != n_b)
	{
		std::cout << "Can't multiply matricies with these sizes: " << "(" << n_a \
			<< ", " << m_a << ") and (" << n_b << ", " << m_b << ")" << std::endl;
	}
	else
	{
		int** A = nullptr;
		int** B = nullptr;
		int** result = nullptr;
		if (createMatrix(&A, n_a, m_a) == OK)
		{
			if (createMatrix(&B, n_b, m_b) == OK)
			{
				if (createMatrix(&result, n_a, m_b) == OK)
				{

					std::cout << "\nInput first matrix: " << std::endl;
					inputMatrix(A, n_a, m_a);

					std::cout << "\nInput second matrix: " << std::endl;
					inputMatrix(B, n_b, m_b);

					const char* alg_names[3] = { "Linear Vinograd algorithm", "Parallel Vinograd algorithm 1", "Parallel Vinograd algorithm 2" };
					int (*algs[3])(int**, int**, int**, int, int, int, int) = { vinogradMulti, vinogradMultiParallel1, vinogradMultiParallel2 };

					for (int it = 0; it < 3; it++)
					{
						if (algs[it](A, B, result, n_a, m_a, m_b, 2) == OK)
						{
							std::cout << "\nResult of multiplication with " << alg_names[it] << ":" << std::endl;
							outputMatrix(result, n_a, m_b);
						}
						else
						{
							std::cout << "\nMultiplication failed..." << std::endl;
						}
					}

					deleteMatrix(A, n_a);
					deleteMatrix(B, n_b);
					deleteMatrix(result, n_a);

				}
				else
				{
					deleteMatrix(A, n_a);
					deleteMatrix(B, n_b);
					std::cout << "\Memory error!" << std::endl;
				}
			}
			else
			{
				deleteMatrix(A, n_a);
				std::cout << "\Memory error!" << std::endl;
			}
		}
		else
		{
			std::cout << "\Memory error!" << std::endl;
		}
	}
}

int main()
{
	taskPerformance();
	timeAnalysis();
}

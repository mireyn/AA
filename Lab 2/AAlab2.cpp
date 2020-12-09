#include <iostream>
#include <ctime>

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


int standardMulti(int** A, int** B, int** res, int N_a, int M_a, int M_b)
{
	for (int i = 0; i < N_a; i++)
		for (int j = 0; j < M_b; j++)
		{
			res[i][j] = 0;
			for (int k = 0; k < M_a; k++)
				res[i][j] += A[i][k] * B[k][j];
		}

	return OK;
}

int vinogradMulti(int** A, int** B, int** result, int N_a, int M_a, int M_b)
{
	int* mulH = new int[N_a];
	int* mulV = new int[M_b];
	if (!mulH || !mulV)
	{
		return FAIL;
	}
	else
	{
		for (int i = 0; i < N_a; i++)
		{
			mulH[i] = 0;
			for (int k = 0; k < M_a / 2; k++)
				mulH[i] = mulH[i] + A[i][2 * k] * A[i][2 * k + 1];
		}

		for (int j = 0; j < M_b; j++)
		{
			mulV[j] = 0;
			for (int k = 0; k < M_a / 2; k++)
				mulV[j] = mulV[j] + B[2 * k][j] * B[2 * k + 1][j];
		}

		for (int i = 0; i < N_a; i++)
			for (int j = 0; j < M_b; j++)
			{
				result[i][j] = 0 - mulH[i] - mulV[j];
				for (int k = 0; k < M_a / 2; k++)
					result[i][j] = result[i][j] + \
					(A[i][2 * k] + B[2 * k + 1][j]) * \
					(A[i][2 * k + 1] + B[2 * k][j]);
			}

		if (M_a % 2 == 1)
			for (int i = 0; i < N_a; i++)
				for (int j = 0; j < M_b; j++)
					result[i][j] = result[i][j] + \
					A[i][M_a - 1] * B[M_a - 1][j];
	}

	return OK;
}

int vinogradOptiMulti(int** A, int** B, int** result, int N_a, int M_a, int M_b)
{
	int* mulH = new int[N_a];
	int* mulV = new int[M_b];
	if (!(mulH && mulV))
	{
		return FAIL;
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
	}

	return OK;
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

					const char* alg_names[3] = { "Standard algorithm", \
										 "Vinograd algorithm", \
										 "Optimized Vinograd algorithm" };
					int (*algs[3])(int**, int**, int**, int, int, int) = { standardMulti, vinogradMulti, vinogradOptiMulti };

					for (int it = 0; it < 3; it++)
					{
						if (algs[it](A, B, result, n_a, m_a, m_b) == OK)
						{
							std::cout << "\nResult of multiplication with " << alg_names[it] << ":" << std::endl;
							outputMatrix(result, n_a, m_b);
						}
						else
						{
							std::cout << "\nMultiplication failed" << std::endl;
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

float getTime(int (*alg)(int**, int**, int**, int, int, int), \
	int **A, int **B, int **result, int size, \
	int itcnt = 10)
{
	fillMatrix(A, size, size);
	fillMatrix(B, size, size);

	clock_t start = std::clock();
	for (int i = 0; i < itcnt; i++)
		alg(A, B, result, size, size, size);
	clock_t end = std::clock();

	float time = (float)(end - start) / (itcnt *  CLOCKS_PER_SEC); ;
	return time;
}

void timeAnalysis()
{
	int series_1[5] = { 100, 200, 300, 400, 500 };
	int series_2[5] = { 101, 201, 301, 401, 501 };

	float results_1[5][3];
	float results_2[5][3];

	int** A = nullptr;
	int** B = nullptr;
	int** result = nullptr;
	if (createMatrix(&A, series_2[4], series_2[4]) == OK)
	{
		if (createMatrix(&B, series_2[4], series_2[4]) == OK)
		{
			if (createMatrix(&result, series_2[4], series_2[4]) == OK)
			{
				const char* alg_names[3] = { "Standard algorithm", \
				 "Vinograd algorithm", \
				 "Optimized Vinograd algorithm" };
				int (*algs[3])(int**, int**, int**, int, int, int) = { standardMulti, vinogradMulti, vinogradOptiMulti };


				for (int si = 0; si < 5; si++)
					for (int ai = 0; ai < 3; ai++)
					{
						results_1[si][ai] = getTime(algs[ai], A, B, result, series_1[si]);
						results_2[si][ai] = getTime(algs[ai], A, B, result, series_2[si]);
					}

				std::cout << "\nResults of the first series:\n";
				for (int i = 0; i < 5; i++)
				{
					for (int j = 0; j < 3; j++)
						std::cout << results_1[i][j] << " ";
					std::cout << std::endl;
				}

				std::cout << "\nResults of the second series:\n";
				for (int i = 0; i < 5; i++)
				{
					for (int j = 0; j < 3; j++)
						std::cout << results_2[i][j] << " ";
					std::cout << std::endl;
				}
			}
			else
			{
				deleteMatrix(A, series_2[4]);
				deleteMatrix(B, series_2[4]);
				std::cout << "\Memory error!" << std::endl;
			}
		}
		else
		{
			deleteMatrix(A, series_2[4]);
			std::cout << "\Memory error!" << std::endl;
		}
	}
	else
	{
		std::cout << "\Memory error!" << std::endl;
	}
}

int main()
{
	taskPerformance();
	timeAnalysis();
}

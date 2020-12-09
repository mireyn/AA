#include <iostream>
#include <ctime>
#include <cstdlib>

#define OK 0
#define FAIL -1

int createArray(int** arr, int n)
{
	(*arr) = new int [n];
	if (!(*arr))
		return FAIL;

	return OK;
}

void deleteArray(int** arr)
{
	delete[] (*arr);
	(*arr) = nullptr;
}

void inputArray(int arr[], int n)
{
	for (int i = 0; i < n; i++)
		std::cin >> arr[i];
}

void outputArray(int arr[], int n)
{
	for (int i = 0; i < n; i++)
		std::cout << arr[i] << " ";
	std::cout << std::endl;
}

void fillAcsArray(int arr[], int n)
{
	for (int i = 0; i < n; i++)
		arr[i] = (i + 17) * (i + 13) - 31;
}

void fillDescArray(int arr[], int n)
{
	for (int i = 0; i < n; i++)
		arr[i] = n * n - (i + 17) * (i + 13) + 31;
}

void fillRandomArray(int arr[], int n)
{
	for (int i = 0; i < n; i++)
		arr[i] = rand();
}


void copyArray(int src[], int dest[], int n)
{
	for (int i = 0; i < n; i++)
		dest[i] = src[i];
}


void swap(int* a, int* b)
{
	int temp = *b; 
	*b = *a;       
	*a = temp;     
}

void bubbleSortFlag(int* arr, int n) {
	bool flag = true;
	while (flag) {
		--n;
		flag = false;
		for (int i = 0; i < n; ++i)
			if (arr[i] > arr[i + 1])
				swap(&arr[i], &arr[i + 1]), flag = true;
	}
}

void selectionSort(int arr[], int n)
{
	for (int i = 0; i < n - 1; i++)
	{
		int select = i;
		for (int j = i + 1; j < n; j++)
			if (arr[j] < arr[select])
				select = j;
		swap(&arr[select], &arr[i]);
	}
}

void insertionSort(int* arr, int n)
{
	for (int i = 1; i < n; i++)
	{
		int key = arr[i];
		int j = i - 1;
		for (; j >= 0 && arr[j] > key; j--)
			arr[j + 1] = arr[j];
		arr[j + 1] = key;
	}
}

void taskPerformance()
{
	int n;
	std::cout << "Input size of array: ";
	std::cin >> n;

	if (!std::cin.good())
	{
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "Incorrect size of array - not a number." << std::endl;
	}
	else if (n < 0)
	{
		std::cout << "Incorrect size of array - negative number: " << n << std::endl;
	}
	else
	{
		int* arr = nullptr;
		int* arr_cpy = nullptr;
		if (createArray(&arr, n) == OK)
		{
			if (createArray(&arr_cpy, n) == OK)
			{
				std::cout << "\nInput elements of array: " << std::endl;
				inputArray(arr, n);

				void (*algs[3])(int*, int) = { bubbleSortFlag, selectionSort, insertionSort };
				const char* alg_names[] = { "Bubble sort with flag",\
										  "Selection sort",\
										  "Insertion sort" };

				for (int ai = 0; ai < 3; ai++)
				{
					copyArray(arr, arr_cpy, n);

					std::cout << "\nBefore sorting: " << std::endl;
					outputArray(arr_cpy, n);

					algs[ai](arr_cpy, n);

					std::cout << "\nSorted with " << alg_names[ai] << ":" << std::endl;
					outputArray(arr_cpy, n);

				}

				deleteArray(&arr);
				deleteArray(&arr_cpy);
			}
			else
			{
				deleteArray(&arr);
				std::cout << "\Memory error!" << std::endl;
			}
		}
		else
		{
			std::cout << "\Memory error!" << std::endl;
		}
	}
}

float getTime(void (*alg)(int*, int), int* arr, int*arr_cpy, int n, int itcnt = 500)
{
	clock_t start = std::clock();
	for (int i = 0; i < itcnt; i++)
	{
		copyArray(arr, arr_cpy, n);
		alg(arr_cpy, n);
	}
	clock_t end = std::clock();

	float time = (float)(end - start) / (itcnt * CLOCKS_PER_SEC / 1000);
	return time;
}

void timeAnalysis()
{
	int series[10] = { 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000 };


	float results_asc[10][3];
	float results_desc[10][3];
	float results_rand[10][3];

	int* arr = nullptr;
	int* arr_cpy = nullptr;
	if (createArray(&arr, series[9]) == OK)
	{
		if (createArray(&arr_cpy, series[9]) == OK)
		{
			void (*algs[3])(int*, int) = { bubbleSortFlag, selectionSort, insertionSort };
			const char* alg_names[] = { "Bubble sort with flag",\
									  "Selection sort",\
									  "Insertion sort" };

			fillAcsArray(arr, series[9]);
			for (int si = 0; si < 10; si++)
				for (int ai = 0; ai < 3; ai++)
					results_asc[si][ai] = getTime(algs[ai], arr, arr_cpy, series[si], 1000);

			std::cout << "\nResults for acsending array:\n";
			for (int i = 0; i < 10; i++)
			{
				for (int j = 0; j < 3; j++)
					std::cout << results_asc[i][j] << " ";
				std::cout << std::endl;
			}

			fillDescArray(arr, series[9]);
			for (int si = 0; si < 10; si++)
				for (int ai = 0; ai < 3; ai++)
					results_desc[si][ai] = getTime(algs[ai], arr, arr_cpy, series[si], 1000);

			std::cout << "\nResults for descending array:\n";
			for (int i = 0; i < 10; i++)
			{
				for (int j = 0; j < 3; j++)
					std::cout << results_desc[i][j] << " ";
				std::cout << std::endl;
			}

			fillRandomArray(arr, series[9]);
			for (int si = 0; si < 10; si++)
				for (int ai = 0; ai < 3; ai++)
					results_rand[si][ai] = getTime(algs[ai], arr, arr_cpy, series[si], 1000);

			std::cout << "\nResults for random filled array:\n";
			for (int i = 0; i < 10; i++)
			{
				for (int j = 0; j < 3; j++)
					std::cout << results_rand[i][j] << " ";
				std::cout << std::endl;
			}

			deleteArray(&arr);
			deleteArray(&arr_cpy);
		}
		else
		{
			deleteArray(&arr);
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

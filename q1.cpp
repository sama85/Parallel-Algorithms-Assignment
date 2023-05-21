#include <iostream>
#include <omp.h>

using namespace std;

#define mx 3001

int matrixA[mx][mx];
int matrixB[mx][mx];
int matrixResultSerial[mx][mx];
int matrixResultParallel[mx][mx] ;

/*
A function to fill up matrices A,B with random numbers
*/
void fill_matrix(int size)
{
	int i, j;
	for (i = 0;i < size;i++)
	{
		for (j = 0;j < size;j++)
		{
			matrixA[i][j] = rand() % 100;
			matrixB[i][j] = rand() % 100;

		}
	}
}

/*
A function to verify the parallel multiplication result by comparing it to the serial multiplication result
*/
bool verify_result(int size)
{

	for (int i = 0;i < size;i++)
	{
		for (int j = 0;j < size;j++) {
if (matrixResultParallel[i][j] != matrixResultSerial[i][j]) return false;
		}
	}
	return true;
}

/*
A function to multiply matrices sequentially
*/
void multiply_serial(int size)
{
	memset(matrixResultSerial, 0, sizeof matrixResultSerial);
	double startTime = omp_get_wtime();

	for (int i = 0;i < size;i++)
	{
		for (int j = 0;j < size;j++)
		{
			for (int k = 0;k < size;k++)
			{
				matrixResultSerial[i][j] += matrixA[i][k] * matrixB[k][j];
			}
		}
	}
	double endTime = omp_get_wtime();
	cout << "Sereial multiplication time for square matices of size " << size <<"is :"<<endTime - startTime<<endl;
}

/*
A function to multiply matrices in parallel
*/
void multiply_parallel1(int size)
{
	
	memset(matrixResultParallel, 0, sizeof matrixResultParallel);
	double startTime = omp_get_wtime();
	int i, k, j;

	/* used the following directives for performance enhancement:
	* 1. static scheduler -> because the work done by iterations is roughly the same
	* 2. collapse -> because the two outermost loops are independent and could be collapsed
	* to one big iteration space that will be divided onto threads by the static scheduler
	* 3. private -> to make loop variables: i,j,k private for each thread to avoid race conditions
	* 4. defualt -> to make other variables shared by default
	*/
	#pragma omp parallel for schedule(static) collapse(2) private(i,j,k) default(shared)
	for (i = 0;i < size;i++) {
		for (j = 0;j < size;j++) {
			for (k = 0;k < size;k++) {
				matrixResultParallel[i][j] += matrixA[i][k] * matrixB[k][j];
			}
		}
	}
	double endTime = omp_get_wtime();
	cout << "Parallel multiplication time for square matices of size " << size << " is :"
		<< endTime - startTime << endl;
	
	if (verify_result(size) == true) 
		cout << "Parallel matrix multiplication result is correct - same as serial!" << endl;
	else
		cout << "Parallel matrix multiplication result is incorrect - different from serial!" << endl;
}


int main() {
	
	int size = 1000;
	fill_matrix(size);
	multiply_serial(size);
	multiply_parallel1(size);

	return 0;

}

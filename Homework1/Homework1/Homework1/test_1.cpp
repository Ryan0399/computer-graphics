#include<iostream>
#include"Matrix.h"
#include"test_1.h"
#include<random>
#include<chrono>


using namespace std;

void Matrix_1()
{
	std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();

	Matrix<double> A(12, 15), B(15, 4);

	for (int i = 0; i < A.getRow(); i++)
	{
		for (int j = 0; j < A.getCol(); j++)
		{
			A(i, j) = rand() * 1.0 / 1000 + 1.0;
		}
	}
	for (int i = 0; i < B.getRow(); i++)
	{
		for (int j = 0; j < B.getCol(); j++)
		{
			B(i, j) = rand() * 1.0 / 1000 + 1.0;
		}
	}

	Matrix<double> C = A * B;
	//Matrix<double> D = A.subMat(3, 5, 2, 4);
	A.print();
	B.print();
	C.print();
	//D.print();

	chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
	chrono::duration<double> time_used = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
	cout << " Matrix_1 solve time cost = " << time_used.count() << " seconds. " << endl << endl;
}
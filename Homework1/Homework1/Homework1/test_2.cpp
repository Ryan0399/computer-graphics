#include<iostream>
#include"Eigen/Dense"
#include"test_1.h"
#include<random>
#include<chrono>


using namespace Eigen;

void Matrix_2()
{
	std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();

	Matrix<double, 12, 15> A;
	Matrix<double, 15, 4> B;

	for (int i = 0; i < A.rows(); i++)
	{
		for (int j = 0; j < A.cols(); j++)
		{
			A(i, j) = rand() * 1.0 / 1000 + 1.0;
		}
	}
	for (int i = 0; i < B.rows(); i++)
	{
		for (int j = 0; j < B.cols(); j++)
		{
			B(i, j) = rand() * 1.0 / 1000 + 1.0;
		}
	}

	MatrixXd C = A * B;
	std::cout << A << std::endl << std::endl;
	std::cout << B << std::endl << std::endl;
	std::cout << C << std::endl << std::endl;
	//Matrix<double> D = A.subMat(3, 5, 2, 4);
	//D.print();

	std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
	std::chrono::duration<double> time_used = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
	std::cout << " Matrix_2 solve time cost = " << time_used.count() << " seconds. " << std::endl;
}
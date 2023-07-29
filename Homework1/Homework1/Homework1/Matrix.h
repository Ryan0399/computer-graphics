#pragma once
#include<iostream>

template<typename T>
class Matrix
{
private:
	int rows, cols;
	T** data;
private:
	void Init();//初始化矩阵
	void Free();//清除矩阵
public:
	Matrix();
	Matrix(int m, int n);
	Matrix(const Matrix& rhs);
	~Matrix();

	int getRow() const;
	int getCol() const;

	Matrix col(int n);
	Matrix row(int n);
	Matrix subMat(int m1, int m2, int n1, int n2);

	void print() const;

	//重载运算符
	T& operator()(int m, int n) const;
	T& operator[](int m)const;
	Matrix operator =(const Matrix& rhs);
	Matrix operator +(const Matrix& rhs);
	Matrix operator -(const Matrix& rhs);
	Matrix operator *(const Matrix& rhs);
	Matrix operator /(const Matrix& rhs);

	Matrix operator +=(const Matrix& rhs);
	Matrix operator -=(const Matrix& rhs);
	Matrix operator *=(const Matrix& rhs);
	Matrix operator /=(const Matrix& rhs);

	Matrix operator +(T v);
	Matrix operator -(T v);
	Matrix operator *(T v);
	Matrix operator /(T v);

	Matrix operator +=(T v);
	Matrix operator -=(T v);
	Matrix operator *=(T v);
	Matrix operator /=(T v);
};


using namespace std;


template<typename T>
void Matrix<T>::Init()
{
	data = nullptr;
	rows = 0;
	cols = 0;
}
template<typename T>
void Matrix<T>::Free()
{
	delete[] data;
	data = nullptr;
	rows = 0;
	cols = 0;
}

template<typename T>
Matrix<T>::Matrix()
{
	Init();
}
template<typename T>
Matrix<T>::Matrix(int m, int n)
{
	data = new T * [m];
	for (int i = 0; i < m; i++)
	{
		data[i] = new T[n];
	}
	rows = m;
	cols = n;
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			data[i][j] = 0;
		}
	}
}
template<typename T>
Matrix<T>::Matrix(const Matrix<T>& rhs)//拷贝成员函数
{
	data = new T * [rhs.rows];
	for (int i = 0; i < rhs.rows; i++)
	{
		data[i] = new T[rhs.cols];
	}
	rows = rhs.rows;
	cols = rhs.cols;
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			data[i][j] = rhs.data[i][j];
		}
	}
}
template<typename T>
Matrix<T>::~Matrix()
{
	Free();
}

template<typename T>
int Matrix<T>::getRow() const
{
	return(rows);
}
template<typename T>
int Matrix<T>::getCol() const
{
	return(cols);
}

template<typename T>
Matrix<T> Matrix<T>::col(int j)
{
	Matrix<T> rhs(rows, 1);
	for (int i = 0; i < rows; i++)
	{
		rhs.data[i][0] = data[i][j];
	}
	return(rhs);
}
template<typename T>
Matrix<T> Matrix<T>::row(int i)
{
	Matrix<T> rhs(1, cols);
	for (int j = 0; j < cols; j++)
	{
		rhs.data[0][j] = data[i][j];
	}
	return(rhs);
}
template<typename T>
Matrix<T> Matrix<T>::subMat(int m1, int m2, int n1, int n2)
{
	//此处的m1,m2,n1,n2均为在指针下的索引
	Matrix<T> rhs(m2 - m1 + 1, n2 - n1 + 1);
	for (int i = 0; i <= m2 - m1; i++)
	{
		for (int j = 0; j <= n2 - n1; j++)
		{
			rhs.data[i][j] = data[m1 + i][n1 + j];
		}
	}
	return(rhs);
}

template<typename T>
void Matrix<T>::print() const
{
	printf("this matrix has size (%d * %d)\n", rows, cols);
	printf("the entries are:\n");
	if (rows * cols == 0)
	{
		printf("NULL!\n");
	}
	else
	{
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < cols; j++)
			{
				cout << data[i][j] << '\t';
			}
			cout << endl;
		}
		cout << endl;
	}
}

template<typename T>
T& Matrix<T>::operator()(int m, int n) const
{
	return(data[m][n]);
}
template<typename T>
T& Matrix<T>::operator[](int m) const
{
	if (rows == 1)
	{
		return(data[0][m]);
	}
	if (cols == 1)
	{
		return(data[m][0]);
	}
	cout << "format error!" << endl;
	return(data[0][0]);
}
template<typename T>
Matrix<T> Matrix<T>::operator =(const Matrix<T>& rhs)
{
	rows = rhs.rows;
	cols = rhs.cols;
	T** tempData = new T * [rows];
	for (int i = 0; i < rows; i++)
	{
		tempData[i] = new T[cols];
	}
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			tempData[i][j] = rhs.data[i][j];
		}
	}
	delete[] data;
	data = tempData;
	return(*this);
}
template<typename T>
Matrix<T> Matrix<T>::operator +(const Matrix<T>& rhs)
{
	if (rows != rhs.rows || cols != rhs.cols)
	{
		Matrix<T> Null;
		cout << "format error!" << endl;
		return(Null);
	}
	Matrix<T> ret(rows, cols);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			ret.data[i][j] = data[i][j] + rhs.data[i][j];
		}
	}
	return(ret);
}
template<typename T>
Matrix<T> Matrix<T>::operator -(const Matrix<T>& rhs)
{
	if (rows != rhs.rows || cols != rhs.cols)
	{
		Matrix<T> Null;
		cout << "format error!" << endl;
		return(Null);
	}
	Matrix<T> ret(rows, cols);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			ret.data[i][j] = data[i][j] - rhs.data[i][j];
		}
	}
	return(ret);
}
template<typename T>
Matrix<T> Matrix<T>::operator *(const Matrix<T>& rhs)
{
	if (cols != rhs.rows)
	{
		Matrix<T> Null;
		cout << "format error!" << endl;
		return(Null);
	}
	Matrix<T> ret(rows, rhs.cols);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < rhs.cols; j++)
		{
			for (int k = 0; k < cols; k++)
			{
				ret.data[i][j] += data[i][k] * rhs.data[k][j];
			}
		}
	}
	return(ret);
}
template<typename T>
Matrix<T> Matrix<T>::operator /(const Matrix<T>& rhs)
{
	if (rows != rhs.rows || cols != rhs.cols)
	{
		Matrix<T> Null;
		cout << "format error!" << endl;
		return(Null);
	}
	Matrix<T> ret(rows, cols);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (rhs.data[i][j] == 0)
			{
				Matrix<T> Null;
				cout << "element error!" << endl;
				return(Null);
			}
			ret.data[i][j] = data[i][j] / rhs.data[i][j];
		}
	}
	return(ret);
}

template<typename T>
Matrix<T> Matrix<T>::operator +=(const Matrix<T>& rhs)
{
	if (rows != rhs.rows || cols != rhs.cols)
	{
		Matrix<T> Null;
		cout << "format error!" << endl;
		return(Null);
	}
	T** tempData = new T * [rows];
	for (int i = 0; i < rows; i++)
	{
		tempData[i] = new T[cols];
	}
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			tempData[i][j] = data[i][j] + rhs.data[i][j];
		}
	}
	delete[] data;
	data = tempData;
	return(*this);
}
template<typename T>
Matrix<T> Matrix<T>::operator -=(const Matrix<T>& rhs)
{
	if (rows != rhs.rows || cols != rhs.cols)
	{
		Matrix<T> Null;
		cout << "format error!" << endl;
		return(Null);
	}
	T** tempData = new T * [rows];
	for (int i = 0; i < rows; i++)
	{
		tempData[i] = new T[cols];
	}

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			tempData[i][j] = data[i][j] - rhs.data[i][j];
		}
	}
	delete[] data;
	data = tempData;
	return(*this);
}
template<typename T>
Matrix<T> Matrix<T>::operator *=(const Matrix<T>& rhs)
{
	if (cols != rhs.rows)
	{
		Matrix<T> Null;
		cout << "format error!" << endl;
		return(Null);
	}
	T** tempData = new T * [rows];
	for (int i = 0; i < rows; i++)
	{
		tempData[i] = new T[rhs.cols];
	}
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < rhs.cols; j++)
		{
			tempData[i][j] = 0.;
			for (int k = 0; k < cols; k++)
			{
				tempData[i][j] += data[i][k] * rhs.data[k][j];
			}
		}
	}
	delete[] data;
	data = tempData;
	return(*this);
}
template<typename T>
Matrix<T> Matrix<T>::operator /=(const Matrix<T>& rhs)
{
	if (rows != rhs.rows || cols != rhs.cols)
	{
		Matrix<T> Null;
		cout << "format error!" << endl;
		return(Null);
	}
	T** tempData = new T * [rows];
	for (int i = 0; i < rows; i++)
	{
		tempData[i] = new T[cols];
	}
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (rhs.data[i][j] == 0)
			{
				Matrix<T> Null;
				cout << "element error!" << endl;
				return(Null);
			}
			tempData[i][j] = data[i][j] / rhs.data[i][j];
		}
	}
	delete[] data;
	data = tempData;
	return(*this);
}

template<typename T>
Matrix<T> Matrix<T>::operator +(T v)
{
	Matrix<T> ret(rows, cols);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			ret.data[i][j] = data[i][j] + v;
		}
	}
	return(ret);
}
template<typename T>
Matrix<T> Matrix<T>::operator -(T v)
{
	Matrix<T> ret(rows, cols);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			ret.data[i][j] = data[i][j] - v;
		}
	}
	return(ret);
}
template<typename T>
Matrix<T> Matrix<T>::operator *(T v)
{
	Matrix<T> ret(rows, cols);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			ret.data[i][j] = data[i][j] * v;
		}
	}
	return(ret);
}
template<typename T>
Matrix<T> Matrix<T>::operator /(T v)
{
	if (v == 0)
	{
		Matrix<T> Null;
		cout << "element error!" << endl;
		return(Null);
	}
	Matrix<T> ret(rows, cols);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			ret.data[i][j] = data[i][j] / v;
		}
	}
	return(ret);
}

template<typename T>
Matrix<T> Matrix<T>::operator +=(T v)
{
	T** tempData = new T * [rows];
	for (int i = 0; i < rows; i++)
	{
		tempData[i] = new T[cols];
	}

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			tempData[i][j] = data[i][j] + v;
		}
	}
	delete[] data;
	data = tempData;
	return(*this);
}
template<typename T>
Matrix<T> Matrix<T>::operator -=(T v)
{
	T** tempData = new T * [rows];
	for (int i = 0; i < rows; i++)
	{
		tempData[i] = new T[cols];
	}

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			tempData[i][j] = data[i][j] - v;
		}
	}
	delete[] data;
	data = tempData;
	return(*this);
}
template<typename T>
Matrix<T> Matrix<T>::operator *=(T v)
{
	T** tempData = new T * [rows];
	for (int i = 0; i < rows; i++)
	{
		tempData[i] = new T[cols];
	}

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			tempData[i][j] = data[i][j] * v;
		}
	}
	delete[] data;
	data = tempData;
	return(*this);
}
template<typename T>
Matrix<T> Matrix<T>::operator /=(T v)
{
	if (v == 0)
	{
		Matrix<T> Null;
		cout << "element error!" << endl;
		return(Null);
	}
	T** tempData = new T * [rows];
	for (int i = 0; i < rows; i++)
	{
		tempData[i] = new T[cols];
	}

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			tempData[i][j] = data[i][j] / v;
		}
	}
	delete[] data;
	data = tempData;
	return(*this);
}
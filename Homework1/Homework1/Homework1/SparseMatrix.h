#pragma once
#include<iostream>
#include<assert.h>
using namespace std;
const int DefaultSize = 1000;
template<typename R>
class Trituple
{
public:
	int row;
	int col;
	R data;
	Trituple operator=(const Trituple& x)
	{
		row = x.row;//行
		col = x.col;//列
		data = x.data;//值
		return(*this);
	}
};

template<typename R>
class SparseMatrix
{
private:
	int rows, cols, terms;
	Trituple<R>* smArray;
	int maxTerms;
public:
	SparseMatrix(int maxSz = DefaultSize)//构造函数
	{
		maxTerms = maxSz;
		smArray = new Trituple<R>[maxTerms];
		assert(smArray != NULL);
		rows = cols = terms = 0;
	}
	SparseMatrix(SparseMatrix& SM)//复制构造函数
	{
		rows = SM.rows;
		cols = SM.cols;
		terms = SM.terms;
		maxTerms = SM.maxTerms;
		smArray = new Trituple<R>[maxTerms];
		assert(smArray != NULL);
		for (int i = 0; i < terms; i++)
		{
			smArray[i] = SM.smArray;
		}
	}
	~SparseMatrix()//析构函数
	{
		delete[] smArray;
		smArray = nullptr;
		rows = cols = terms = maxTerms = 0;
	}

	int getRows() const
	{
		return(rows);
	}
	int getCols() const
	{
		return(cols);
	}

	SparseMatrix operator=(const SparseMatrix& SM)
	{
		rows = SM.rows;      
		cols = SM.cols;
		terms = SM.terms;
		maxTerms = SM.maxTerms;
		for (int i = 0; i < terms; i++)      
			smArray[i] = SM.smArray[i];
		return *this;
	}
	SparseMatrix operator+(const SparseMatrix& SM)
	{
		SparseMatrix rhs(DefaultSize);
		rhs.rows = rows;
		rhs.cols = cols;
		rhs.terms = terms;
		for (int i = 0; i < rhs.terms; i++)
			rhs.smArray[i] = SM.smArray[i] + smArray[i];
		return(rhs);
	}
	SparseMatrix operator-(const SparseMatrix& SM)
	{
		SparseMatrix rhs(DefaultSize);
		rhs.rows = rows;
		rhs.cols = cols;
		rhs.terms = terms;
		for (int i = 0; i < rhs.terms; i++)
			rhs.smArray[i] = smArray[i] - SM.smArray[i];
		return(rhs);
	}
	SparseMatrix operator*(const SparseMatrix& SM)
	{
		SparseMatrix<R> ret(DefaultSize);
		ret.rows = rows;
		ret.cols = SM.cols;
		int l = 0;
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < rhs.cols; j++)
			{
				temp = 0;
				for (int k = 0; k < cols; k++)
				{
					temp += (*this)(i,j) * rhs(i,j);
					if (temp != 0)
					{
						ret.smArray[l].data = temp;
						ret.terms++;
					}
				}
			}
		}
		return(ret);
	}

	R& operator()(int m, int n) const
	{
		for (int i = 0; i < terms; i++)
		{
			if (smArray[i].row == m && smArray[i].col == n)
			{
				return(smArray[i].data);
			}
		}
		if (i == terms)
		{
			return(0);
		}
	}
	void print(const SparseMatrix& SM)
	{
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < cols; j++)
			{
				for (int k = 0; k < terms; k++)
				{
					if (smArray[k].row == i && smArray[k].col == j)
					{
						cout << smArray[k].data << '\t';
					}
					else
					{
						cout << 0 << '\t';
					}
				}
			}
			cout << endl;
		}
	}
};
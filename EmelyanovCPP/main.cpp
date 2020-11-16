#include <iostream>
#include <fstream>

using namespace std;

class Matrix
{
private:

	int msize;
	int** marr;

public:

	Matrix()
	{
		msize = 0;
		marr = 0;
	}

	Matrix(int size)
	{
		msize = size;
		marr = new int* [msize];
		for (int count = 0; count < msize; count++)
			marr[count] = new int[msize];
		for (int i = 0; i < msize; i++)
			for (int j = 0; j < msize; j++)
			{
				if (i == j) marr[i][j] = 1;
				else marr[i][j] = 0;
			}
	}

	Matrix(int size, int* data)
	{
		msize = size;
		marr = new int* [msize];
		for (int count = 0; count < msize; count++)
			marr[count] = new int[msize];
		for (int i = 0; i < msize; i++)
			for (int j = 0; j < msize; j++)
			{
				if (i == j) marr[i][j] = data[i];
				else marr[i][j] = 0;
			}
	}

	Matrix(const Matrix& A)
	{
		msize = A.msize;
		marr = new int* [msize];
		for (int count = 0; count < msize; count++)
			marr[count] = new int[msize];
		for (size_t i = 0; i < msize; i++)
			for (size_t j = 0; j < msize; j++)
				marr[i][j] = A.marr[i][j];
	}

	
	~Matrix()
	{
		for (int i = 0; i < msize; i++)
		{
			delete[] marr[i];
		}
		delete[] marr;
	}
	

	Matrix operator+ (const Matrix& matr2) const;
	Matrix operator- (const Matrix& matr2) const;
	Matrix operator* (const Matrix& matr2) const;
	Matrix operator()(const size_t n, const size_t m) const;
	Matrix operator! ();
	bool operator== (const Matrix& matr2) const;
	int* operator[](const int row);
	int const* operator[](const size_t row) const;
	

	
	friend ostream& operator<<(ostream& out, const Matrix& matr2)
	{
		for (int i = 0; i < matr2.msize; i++)
		{
			for (int j = 0; j < matr2.msize; j++)
			{
				out << matr2.marr[i][j] << " ";
			}
			out << endl;
		}
		return out;
	}

	friend istream& operator>>(istream& in, Matrix& matr2)
	{
		for (int i = 0; i < matr2.msize; i++)
		{
			for (int j = 0; j < matr2.msize; j++)
			{
				in >> matr2.marr[i][j];
			}
		}
		return in;
	}

};

Matrix Matrix :: operator+(const Matrix& matr2) const 
{
	if (msize == matr2.msize)
	{
		Matrix temp(msize);
		for(int i = 0; i < msize; i++)
			for (int j = 0; j < msize; j++)
			{
				temp.marr[i][j] = marr[i][j]+matr2.marr[i][j];
			}
		return temp;
	}

	else throw std::logic_error{ "Matrix::operator+" };

}

Matrix Matrix :: operator-(const Matrix& matr2) const
{
	

	if (msize == matr2.msize)
	{
		Matrix temp(msize);
		for (int i = 0; i < msize; i++)
			for (int j = 0; j < msize; j++)
			{
				temp.marr[i][j] = marr[i][j] - matr2.marr[i][j];
			}
		return temp;
	}

	else throw std::logic_error{ "Matrix::operator-" };
}

Matrix Matrix :: operator*(const Matrix& matr2) const
{
	Matrix result(msize);

	if (msize == matr2.msize)
	{

		for (size_t i = 0; i < msize; i++)
		{
			for (size_t j = 0; j < msize; j++)
			{
				int cell = 0;
				for (size_t k = 0; k < msize; k++)
				{
					cell += marr[i][k] * matr2.marr[k][j];
				}
				result.marr[i][j] = cell;
			}
		}
		return result;
	}
	else throw std::logic_error{ "Matrix::operator*" };
}

bool Matrix :: operator==(const Matrix& matr2) const
{
	if (msize != matr2.msize)
		return false;
	for (int i = 0; i < msize; i++)
		for (int j = 0; j < msize; j++)
			if (!(marr[i][j] == matr2.marr[i][j]))
				return false;
	return true;
}

Matrix Matrix :: operator!()
{
	Matrix temp(msize);

	for (int i = 0; i < msize; ++i)
		for (int j = 0; j < msize; ++j)
			temp.marr[j][i] = marr[i][j];
	return temp;
}

int* Matrix :: operator[](const int row)
{
	if (row < 0 || row >= msize)
	{
		throw std::out_of_range{ "Matrix::operator[]" };
	}
	return marr[row];
}

int const* Matrix ::  operator[](const size_t row) const
{
	if (row < 0 || row >= msize)
	{
		throw std::out_of_range{ "Matrix::operator[]" };
	}
	return marr[row];
}

Matrix Matrix :: operator()(const size_t n, const size_t m) const
{
	if (n >= msize || m >= msize)
	{
		throw std::out_of_range("Matrix::operator()");
	}
	Matrix result{ msize - 1 };
	for (size_t i = 0, i_temp = 0; i < msize; i++)
	{
		for (size_t j = 0, j_temp = 0; j < msize; j++)
		{
			if (i == n) continue;
			if (j == m) continue;
			result.marr[i_temp][j_temp] = marr[i][j];
			j_temp++;
		}
		if (i != n)
		{
			i_temp++;
		}
	}
	return result;
}

int main()
{
	ifstream fin{ "input.txt" };
	ofstream fout{ "output.txt" };

	if (!fin || !fout)
	{
		cout << "File error!" << endl;
		return 0;
	}

	int matrix_size;
	int k;

	fin >> matrix_size;

	Matrix A(matrix_size);
	Matrix B(matrix_size);
	Matrix C(matrix_size);
	Matrix D(matrix_size);

	fin >> k;
	fin >> A >> B >> C >> D;

	int* karray = new int[matrix_size];
	for (int i = 0; i < matrix_size; i++)
		karray[i] = k;
	Matrix K(matrix_size, karray);
	

	fout << (A + B * (!C) + K) * (!D);  

	return 0;
}
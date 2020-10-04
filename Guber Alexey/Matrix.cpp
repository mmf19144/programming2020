#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>

// Это костыль
template<typename T>
class Reference
{
	T & ref;
public:
	Reference(T & value) : ref{value} {  }

	operator T() const
	{
		return ref;
	}

	T operator=(const T & value)
	{
		ref = value;
		return ref;
	}

};

class Matrix
{
	long long ** arr;
	size_t sz;

	void memory_allocation(const size_t n)
	{
		arr = new long long* [n];
		for (size_t i = 0; i < n; i++)
		{
			arr[i] = new long long[n];
		}
	}

public:
	Matrix() : arr{nullptr}, sz{0} {	}

	explicit Matrix(const size_t sz) : sz{sz}
	{
		memory_allocation(sz);
		for (size_t i = 0; i < sz; i++)
		{
			for (size_t j = 0; j < sz; j++)
			{
				if (i == j) arr[i][i] = 1;
				else arr[i][j] = 0;
			}
		}
	}

	Matrix(const size_t sz, long long const * const elements) : sz{sz}
	{
		memory_allocation(sz);
		for (size_t i = 0; i < sz; i++)
		{
			for (size_t j = 0; j < sz; j++)
			{
				arr[i][j] = (i == j) ? elements[i] : 0;
			}
		}
	}

	Matrix(const Matrix & A) : sz{A.size()}
	{
		memory_allocation(sz);
		for (size_t i = 0; i < sz; i++)
			for (size_t j = 0; j < sz; j++)
				arr[i][j] = A.arr[i][j];
	}

	~Matrix()
	{
		for (size_t i = 0; i < sz; i++)
		{
			delete[] arr[i];
		}
		delete[] arr;
	}

	const size_t size() const
	{
		return sz;
	}

	long long const * operator[](const size_t row) const
	{
		if (row < 0 || row >= sz)
		{
			throw std::out_of_range{ "Matrix::operator[]" };
		}
		return arr[row];
	}

	long long * operator[](const size_t row)
	{
		if (row < 0 || row >= sz)
		{
			throw std::out_of_range{ "Matrix::operator[]" };
		}
		return arr[row];
	}

	std::vector<Reference<long long>> operator()(const size_t column)
	{
		std::vector<Reference<long long>> result;
		for (size_t i = 0; i < sz; i++)
		{
			result.push_back(arr[i][column]);
		}
		return result;
	}

	const Matrix operator+(const Matrix & A) const
	{
		if (sz != A.size())
		{
			throw std::logic_error{"Matrix::operator+"};
		}
		Matrix result{sz};
		for (size_t i = 0; i < sz; i++)
		{
			for (size_t j = 0; j < sz; j++)
			{
				result[i][j] = arr[i][j] + A[i][j];
			}
		}
		return result;
	}

	Matrix operator-(const Matrix & A)
	{
		if (sz != A.size())
		{
			throw std::logic_error{"Matrix::operator-"};
		}
		Matrix result{sz};
		for (size_t i = 0; i < sz; i++)
		{
			for (size_t j = 0; j < sz; j++)
			{
				result[i][j] = arr[i][j] - A[i][j];
			}
		}
		return result;
	}

	const Matrix operator*(const Matrix & A) const 
	{
		if (sz != A.size())
		{
			throw std::logic_error{"Matrix::operator*"};
		}

		Matrix result{ sz };
		for (size_t i = 0; i < sz; i++)
		{
			for (size_t j = 0; j < sz; j++)
			{
				int cell = 0;
				for (size_t k = 0; k < sz; k++)
				{
					cell += arr[i][k] * A[k][j];
				}
				result[i][j] = cell;
			}
		}
		return result;
	}

	Matrix & operator+=(const Matrix & A)
	{
		if (sz != A.size())
		{
			throw std::logic_error{"Matrix::operator+"};
		}

		for (size_t i = 0; i < sz; i++)
		{
			for (size_t j = 0; j < sz; j++)
			{
				arr[i][j] += A[i][j];
			}
		}
		return *this;
	}

	bool operator==(const Matrix & A) const
	{
		if (sz != A.size())	return false;

		for (size_t i = 0; i < sz; i++)
		{
			for (size_t j = 0; j < sz; j++)
			{
				if (arr[i][j] != A[i][j]) return false;
			}
		}
		return true;
	}

	bool operator!=(const Matrix & A) const
	{
		if (sz != A.size())	return true;

		for (size_t i = 0; i < sz; i++)
		{
			for (size_t j = 0; j < sz; j++)
			{
				if (arr[i][j] != A[i][j]) return true;
			}
		}
		return false;
	}

	Matrix operator=(const Matrix & A)
	{
		for (size_t i = 0; i < sz; i++)
		{
			delete[] arr[i];
		}
		delete[] arr;

		memory_allocation(A.size());
		sz = A.size();

		for (size_t i = 0; i < sz; i++)
			for (size_t j = 0; j < sz; j++)
				arr[i][j] = A.arr[i][j];
		return *this;
	}

	// Транспонирование матрицы
	Matrix operator~() const
	{
		Matrix result{sz};
		for (size_t i = 0; i < sz; i++)
		{
			for (size_t j = 0; j < sz; j++)
			{
				result[i][j] = arr[j][i];
			}
		}
		return result;
	}

	Matrix operator()(const size_t n, const size_t m) const
	{
		if (n >= sz || m >= sz)
		{
			throw std::out_of_range("Matrix::operator()");
		}
		Matrix result{sz - 1};
		for (size_t i = 0, i_temp = 0; i < sz; i++)
		{
			for (size_t j = 0, j_temp = 0; j < sz; j++)
			{
				if (i == n) continue;
				if (j == m) continue;
				result[i_temp][j_temp] = arr[i][j];
				j_temp++;
			}
			if (i != n)
			{
				i_temp++;
			}
		}
		return result;
	}

	friend std::ostream & operator<<(std::ostream & out, const Matrix & A)
	{
		for (size_t i = 0; i < A.size(); i++)
		{
			for (size_t j = 0; j < A.size(); j++)
			{
				out << A[i][j] << " ";
			}
			out << std::endl;
		}
		return out;
	}

	friend std::istream & operator>>(std::istream & in, Matrix & A)
	{
		for (size_t i = 0; i < A.size(); i++)
		{
			for (size_t j = 0; j < A.size(); j++)
			{
				in >> A[i][j];
			}
		}
		return in;
	}
};









int main()
{
	std::ifstream fin{"input.txt"};
	std::ofstream fout{"output.txt"};
	if (!fin || !fout)
	{
		std::cout << "Балбес! Файла нету!" << std::endl;
		return 0;
	}

	size_t matrix_size;
	fin >> matrix_size;

	long long k;
	fin >> k;

	Matrix A{matrix_size};
	Matrix B{matrix_size};
	Matrix C{matrix_size};
	Matrix D{matrix_size};
	fin >> A >> B >> C >> D;

	int a[] = {3, 3};
	Matrix K( matrix_size, std::vector<long long>(matrix_size, k).data());
	fout << (A + B * (~C) + K) * (~D) << std::endl;

	/*Matrix M(10);
	M[5][3] = 4;
	M(3)[5] = 12;
	std::cout << M;*/
	
}
#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <functional>

class Matrix
{
    int * data;
    size_t sz;

public:
	Matrix() : data{nullptr}, sz{0} {	}

	explicit Matrix(size_t sz) : sz{sz}
	{
		data = new int[sz * sz];
		for (size_t i = 0; i < sz; i++)
		{
			for (size_t j = 0; j < sz; j++)
			{
				(*this)[i][j] = i == j ? 1 : 0;
			}
		}
	}

	Matrix(size_t sz, int const * const elements) : sz{sz}
	{
		data = new int[sz * sz];
		for (size_t i = 0; i < sz; i++)
		{
			for (size_t j = 0; j < sz; j++)
			{
				(*this)[i][j] = i == j ? elements[i] : 0;
			}
		}
	}

	Matrix(const Matrix & A) : sz{A.size()}
	{
		data = new int[sz * sz];
		for (size_t i = 0; i < sz; i++)
		{
			for (size_t j = 0; j < sz; j++)
			{
				(*this)[i][j] = A[i][j];
			}
		}
	}

	~Matrix()
	{
		delete[] data;
	}

	Matrix operator+(const Matrix & A) const
	{
		if (A.size() != sz)
		{
			throw std::invalid_argument{"Matrix::operator+ - у матриц разные размеры"};
		}

		Matrix result{sz};
		for (size_t i = 0; i < sz; i++)
		{
			for (size_t j = 0; j < sz; j++)
			{
				result[i][j] = (*this)[i][j] + A[i][j];
			}
		}
		return result;
	}

	Matrix operator-(const Matrix & A)
	{
		if (sz != A.size())
		{
			throw std::invalid_argument{"Matrix::operator-"};
		}
		Matrix result{sz};
		for (size_t i = 0; i < sz; i++)
		{
			for (size_t j = 0; j < sz; j++)
			{
				result[i][j] = (*this)[i][j] - A[i][j];
			}
		}
		return result;
	}

	const Matrix operator*(const Matrix & A) const 
	{
		if (sz != A.size())
		{
			throw std::invalid_argument{"Matrix::operator*"};
		}

		Matrix result{ sz };
		for (size_t i = 0; i < sz; i++)
		{
			for (size_t j = 0; j < sz; j++)
			{
				int cell = 0;
				for (size_t k = 0; k < sz; k++)
				{
					cell += (*this)[i][k] * A[k][j];
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
			throw std::invalid_argument{"Matrix::operator+"};
		}

		for (size_t i = 0; i < sz; i++)
		{
			for (size_t j = 0; j < sz; j++)
			{
				(*this)[i][j] += A[i][j];
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
				if ((*this)[i][j] != A[i][j]) return false;
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
				if ((*this)[i][j] != A[i][j]) return true;
			}
		}
		return false;
	}

	Matrix & operator=(const Matrix & A)
	{
		delete[] data;
		data = nullptr;

		sz = A.size();
		data = new int[sz * sz];

		for (size_t i = 0; i < sz; i++)
			for (size_t j = 0; j < sz; j++)
				(*this)[i][j] = A[i][j];
		return *this;
	}

	Matrix operator~() const
	{
		Matrix result{sz};
		for (size_t i = 0; i < sz; i++)
		{
			for (size_t j = 0; j < sz; j++)
			{
				result[i][j] = (*this)[j][i];
			}
		}
		return result;
	}

    class Slice
    {
        Matrix & matrix;
        const size_t offset;
        const size_t stride;

    public:
        Slice(Matrix & m, size_t o, size_t s) : matrix{m}, offset{o}, stride{s} {   }

        int & operator[](size_t index)
        {
            return matrix.data[index * stride + offset];
        }
    };

	class const_Slice
	{
		const Matrix & matrix;
		const size_t offset;
        const size_t stride;

    public:
        const_Slice(const Matrix & m, size_t o, size_t s) : matrix{m}, offset{o}, stride{s} {   }

        const int & operator[](size_t index) const
        {
            return matrix.data[index * stride + offset];
        }
	};

	Slice operator[](size_t index)
    {
        return Slice{*this, index * sz, 1};
    }

	const const_Slice operator[](size_t index) const
	{
		return const_Slice{*this, index * sz, 1};
	}

    Slice operator()(size_t index)
    {
        return Slice{*this, index, sz};
    }

	const const_Slice operator()(size_t index) const
	{
		return const_Slice{*this, index, sz};
	}

	size_t size() const
	{
		return sz;
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
	Matrix K( matrix_size, std::vector<int>(matrix_size, k).data());
	fout << (A + B * (~C) + K) * (~D) << std::endl;

	/*Matrix M(10);
	M[5][3] = 4;
	M(3)[5] = 12;
	std::cout << M;*/
}

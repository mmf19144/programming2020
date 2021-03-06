#include <algorithm>
#include <fstream>
using namespace std;

class column
{
	int* matrix;
	const size_t dim;
	const size_t column_idx;

public:
	column(const size_t dim, int* m, const size_t col_id) : matrix(m), dim(dim), column_idx(col_id) {}

	int& operator[](const size_t row)//column element reference
	{
		if (row > dim)
		{
			throw invalid_argument("Row index is greater than dimension");
		}
		return matrix[dim * (row - 1) + column_idx - 1];
	}
};

class row
{
	int* matrix;
	const size_t dim;
	const size_t row_idx;

public:
	row(const size_t dim, int* m, const size_t _raw_id) : matrix(m), dim(dim), row_idx(_raw_id) {}

	int& operator[](const size_t column)//row element reference
	{
		if (column > dim)
		{
			throw invalid_argument("Column index is greater than dimension");
		}
		return matrix[dim * (column - 1) + row_idx - 1];
	}
	friend ostream& operator<< (ostream& os, const row& m) {
		for (size_t i = 0; i < m.dim; i++) {
			os << m.matrix[m.row_idx * m.dim + i];
			os << " ";
		}
		return os;
	}
};

class Matrix
{
	int* data; //matrix
	size_t dim; //dimension
	static int* allocate_matrix(int n) // memory allocation
	{
		int* L = new int[n * n];
		for (size_t i = 0; i < n * n; i++) {
			L[i] = 0;
		}
		return L;
	}
public:
	Matrix() : dim(0), data(nullptr) //default constructor
	{}
	Matrix(size_t n) : dim(n) //identity matrix constructor
	{
		data = allocate_matrix(n);
		for (size_t i = 0; i < n; i++)
		{
			data[i * dim + i] = 1;
		}
	}
	Matrix(size_t n, int m) : dim(n)
	{
		data = allocate_matrix(n);
		for (size_t i = 0; i < n; i++)
		{
			data[i * dim + i] = m;
		}
	}
	Matrix(size_t n, int* D) : dim(n) //diagonal matrix constructor
	{
		data = allocate_matrix(n);
		for (size_t i = 0; i < n; i++)
		{
			data[i * dim + i] = D[i];
		}
	}
	Matrix(const Matrix& that) : dim(that.dim) //copy constructor
	{
		this->data = allocate_matrix(dim);
		for (size_t i = 0; i < dim * dim; i++)
		{
			this->data[i] = that.data[i];
		}
	}
	void clear_matrix()
	{
		delete[] data;
	}
	int dimension()
	{
		return this->dim;
	}

	Matrix& operator=(const Matrix& that)
	{
		this->clear_matrix();
		this->data = nullptr;
		this->dim = that.dim;
		this->data = allocate_matrix(dim);
		for (size_t i = 0; i < dim; i++)
		{
			this->data[i] = that.data[i];
		}
		return *this;
	}
	Matrix operator+(const Matrix& that) const //matrix addition //function does not change this
	{
		if (this->dim != that.dim)
		{
			throw invalid_argument("wrong dimention");
		}
		int n1 = this->dim;
		Matrix temp(n1);

		for (size_t i = 0; i < n1 * n1; i++)
		{
			temp.data[i] = this->data[i] + that.data[i];
		}
		return temp;
	}
	void operator+=(const Matrix& that)
	{
		*this = *this + that;
	}

	void operator-=(const Matrix& that)
	{
		*this = *this - that;
	}

	void operator*=(const Matrix& that)
	{
		*this = *this * that;
	}

	Matrix operator-(const Matrix& that) const  //substraction of matrices
	{
		if (this->dim != that.dim)
		{
			throw invalid_argument("wrong dimention");
		}
		size_t n1 = this->dim;
		Matrix temp(n1);
		for (size_t i = 0; i < n1*n1; i++)
		{
			temp.data[i] = this->data[i] - that.data[i];
		}
		return temp;
	}
	Matrix operator*(const Matrix& that) const //matrix multiplication
	{
		if (this->dim != that.dim)
		{
			throw invalid_argument("wrong dimention");
		}
		size_t n1 = this->dim;
		Matrix temp(n1);
		for (size_t i = 1; i <= n1; i++)
		{
			for (size_t j = 1; j <= n1; j++)
			{
				temp[i][j] = 0;
				for (size_t k = 1; k <= n1; k++)
				{
					temp[i][j] += (*this)[k][j] * that[i][k];
				}
			}
		}
		return temp;
	}
	bool operator==(const Matrix& that) const //comparison of matrices
	{
		if (this->dim != that.dim)
		{
			return false;
		}
		for (size_t i = 0; i < dim; i++)
		{
			if (this->data[i] != that.data[i])
			{
				return false;
			}
		}
		return true;
	}
	bool operator!=(const Matrix& that) const
	{
		return !(*this == that);
	}
	Matrix operator!() const //matrix transpose
	{
		Matrix res(*this);
		for (size_t i = 1; i <= dim; i++)
		{
			for (size_t j = i + 1; j <= dim; j++)
			{
				swap(res[i][j], res[j][i]);
			}
		}
		return res;
	}
	Matrix operator()(size_t raw, size_t column) const //minor, obtained by deleting the specifier raw and column
	{
		if (raw > dim)
		{
			throw invalid_argument("row index is greater than dim");
		}
		if (column > dim)
		{
			throw invalid_argument("column index is greater than dim");
		}
		Matrix res(this->dim - 1);
		for (size_t i = 0; i < raw - 1; i++)
		{
			for (size_t j = 0; i < column - 1; j++)
			{
				res[i][j] = (*this)[i][j];
			}
		}
		for (size_t i = raw; i < dim - 1; i++)
		{
			for (size_t j = 0; i < column - 1; j++)
			{
				res[i][j] = (*this)[i + 1][j];
			}
		}
		for (size_t i = 0; i < raw - 1; i++)
		{
			for (size_t j = column; i < dim - 1; j++)
			{
				res[i][j] = (*this)[i][j + 1];
			}
		}
		for (size_t i = raw; i < dim - 1; i++)
		{
			for (size_t j = column; i < dim - 1; j++)
			{
				res[i][j] = (*this)[i + 1][j + 1];
			}
		}
		return res;
	}
	row operator[](size_t index) const//line access
	{
		if (index > dim)
		{
			throw invalid_argument("raw is  greater than dim");
		}
		return { dim, data, index };
	}

	column operator()(const size_t index)//column access
	{
		if (index > dim)
		{
			throw invalid_argument("column is  greater than dim");
		}

		return { dim, data, index };
	}

	friend ostream& operator<<(ostream& os, const Matrix& m) {
		for (size_t i = 0; i < m.dim; i++) {
			os << m[i];
			os << "\n";
		}
		return os;
	}

	friend istream& operator>>(istream& os, const Matrix& m) {
		for (size_t i = 0; i < m.dim * m.dim; i++) {
			os >> m.data[i];
		}
		return os;
	}

	~Matrix()
	{
		this->clear_matrix();
	}
};


int main()
{
	ifstream input("input.txt");
	ofstream output("output.txt");
	size_t dim, k;
	input >> dim >> k; //reading data
	Matrix A(dim), B(dim), C(dim), D(dim);
	input >> A >> B >> C >> D;
	input.close();
	Matrix K(dim, k);
	output << ((A + (B * (!C)) + K) * (!D));
	return 0;
}
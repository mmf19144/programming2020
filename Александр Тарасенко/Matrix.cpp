#include <iostream>
#include <fstream>

//using namespace std;


class Column {
private:
	const int size;
	const int num;
	int* arr;
public:
	Column(int a, int* matrix, int m_size) : size(m_size), num(a), arr(matrix) {}

	int& operator[](int row) {
		if (row > size || row < 0)
			throw ("The index is out of range");
		return arr[row * size + num];
	}
};

class Row {
private:
	const int size;
	const int num;
	int* arr;
public:
	Row(int a, int* matrix, int m_size) : size(m_size), num(a), arr(matrix) {}

	int& operator[](int col) {
		if (col > size || row < 0)
			throw ("The index is out of range");
		return arr[col + size * num];
	}
};

class Matrix {
private:
	int size;
	int* arr;
public:
	Matrix() {
		size = 0;
		int* arr = NULL;
	}

	Matrix(int A) {
		size = A;
		arr = new int[size * size]();
		int cnt = size + 1;
		for (int i = 0; i < size * size; i += cnt) {
			arr[i] = 1;
		}
	}

	Matrix(int A, int k) {
		size = A;
		arr = new int[size * size]();
		int cnt = size + 1;
		for (int i = 0; i < size * size; i += cnt) {
			arr[i] = k;
		}
	}

	Matrix(int A, int* elements) {
		size = A;
		arr = new int[size * size]();
		int cnt = size + 1;
		int cnt1 = 0;
		for (int i = 0; i < size * size; i += cnt) {
			arr[i] = elements[cnt1];
			cnt1++;
		}
	}

	const Matrix operator+(const Matrix& A) const {
		if (A.size != size)
			throw ("The index is out of range");
		
		Matrix result(A.size);
		
		for (int i = 0; i < result.size * result.size; i++) {
			result.arr[i] = this->arr[i] + A.arr[i];
		}
		return result;
	}

	const Matrix operator-(const Matrix& A) const {
		if (A.size != size)
			throw ("The index is out of range");
		
		Matrix result(A.size);
		for (int i = 0; i < result.size * result.size; i++) {
			result.arr[i] = this->arr[i] - A.arr[i];
		}
		return result;
	}

	const Matrix operator*(const Matrix& A) const {
		if (A.size != size)
			throw ("The index is out of range");
		
		Matrix result(A.size);
		for (int i = 0; i < result.size; i++) {
			for (int j = 0; j < result.size; j++) {
				result.arr[i * result.size + j] = 0;
				for (int k = 0; k < result.size; k++) {
					result.arr[i * result.size + j] += this->arr[i * result.size + k] * A.arr[k * result.size + j];
				}
			}

		}
		return result;
	}

	const Matrix operator()(int row, int col) {
		Matrix result(size - 1);
		int curr_index = 0;
		
		for (int i = 0; i < size * size; i++) {
			if (i % size != col - 1 && i / size != row - 1)
				result.arr[curr_index++] = arr[i];
		}

		return result;
	}

	Row operator[](const unsigned int a) const {
		return Row(a, arr, size);
	}

	Column operator()(const unsigned int a) const {
		return Column(a, arr, size);
	}

	const Matrix operator~() {
		Matrix result(size);
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++)
				result.arr[i * size + j] = this->arr[j * size + i];
		}
		return result;
	}


	friend std::ostream& operator<<(std::ostream& out, const Matrix& A) {
		for (size_t i = 0; i < A.size; i++) {
			for (size_t j = 0; j < A.size; j++) {
				out << A.arr[i * A.size + j] << " ";
			}
			out << std::endl;
		}
		return out;
	}

	friend std::ifstream& operator>>(std::ifstream& in, const Matrix& A) {
		for (size_t i = 0; i < A.size * A.size; i++) {
			in >> A.arr[i];
		}
		return in;
	}


};

int main() {
	std::ofstream fout("output.txt");
	std::ifstream fin("input.txt");

	int N, k;

	fin >> N >> k;

	Matrix A(N), B(N), C(N), K(N, k), D(N);

	fin >> A >> B >> C >> D;

	fout << (A + B * (~C) + K) * (~D);

	//fout << A(2, 1);

	return 0;
}

#include <iostream>
#include <fstream>
#include <cstring>


class MatrixColumn {
private:
    const size_t colIdx;
    const size_t matrixSize;
    int *matrix;
public:
    MatrixColumn() : colIdx(0), matrixSize(0), matrix(nullptr) {}

    MatrixColumn(size_t size, size_t row, int *m) : colIdx(row), matrixSize(size), matrix(m) {}

    int &operator[](size_t row) {
        return matrix[row * matrixSize + colIdx];
    }

    friend std::ostream &operator<<(std::ostream &os, const MatrixColumn &m) {
        for (size_t i = 0; i < m.matrixSize; i++) {
            os << m.matrix[i * m.matrixSize + m.colIdx];
            if (i + 1 != m.matrixSize)
                os << " ";
        }
        return os;
    }
};

class MatrixRow {
private:
    const size_t rowIdx;
    const size_t matrixSize;
    int *matrix;
public:
    MatrixRow() : rowIdx(0), matrixSize(0), matrix(nullptr) {}

    MatrixRow(size_t size, size_t row, int *m) : rowIdx(row), matrixSize(size), matrix(m) {}

    int &operator[](size_t col) {
        return matrix[rowIdx * matrixSize + col];
    }

    friend std::ostream &operator<<(std::ostream &os, const MatrixRow &m) {
        for (size_t i = 0; i < m.matrixSize; i++) {
            os << m.matrix[m.rowIdx * m.matrixSize + i];
            if (i + 1 != m.matrixSize)
                os << " ";
        }
        return os;
    }


};

class Matrix {
private:
    const size_t size;
    int *matrix;

    void init_empty(int default_value = 1) {
        memset(matrix, 0, size * size * sizeof(int));
        for (size_t i = 0; i < size; ++i)
            matrix[i * size + i] = default_value;
    }


    friend bool is_equal(const Matrix &m1, const Matrix &m2) {
        if (m1.getSize() != m2.getSize())
            throw std::invalid_argument("Matrices a and b must have the same dimensions");
        return memcmp(m1.matrix, m2.matrix, m1.size * m2.size * sizeof(int));
    }

public:
    Matrix() : size(0),
               matrix(nullptr) {
    }

    explicit Matrix(const size_t new_size) : size(new_size),
                                             matrix(new int[new_size * new_size]) {
        init_empty(1);
    }

    Matrix(const size_t new_size, const size_t k) : size(new_size),
                                                    matrix(new int[new_size * new_size]) {
        init_empty(k);
    }

    Matrix(size_t new_size, const int diagonal[]) : Matrix(new_size) {
        for (size_t i = 0; i < new_size; i++)
            (*this)[i][i] = diagonal[i];
    }

    ~Matrix() {
        delete[] matrix;
    }

    size_t getSize() const {
        return size;
    }

    Matrix getMinor(size_t row, size_t column) const {
        if (size <= 1 || size < row || size < column || row <= 0 || column <= 0)
            throw std::invalid_argument("Matrix has not valid dimension");

        Matrix minor(size - 1);
        bool new_row = false, new_column = false;
        for (size_t i = 0; i < size; i++) {
            if (i == row - 1) {
                new_row = true;
                continue;
            }
            for (size_t j = 0; j < size; j++) {
                if (j == column - 1) {
                    new_column = true;
                    continue;
                }
                minor[i - new_row][j - new_column] = (*this)[i][j];
            }
        }
        return minor;
    }

    Matrix operator+(const Matrix &m) const {
        if (m.getSize() != size)
            throw std::invalid_argument("Matrices a and b must have the same dimensions");

        Matrix sum(size);
        for (size_t i = 0; i < size; i++) {
            for (size_t j = 0; j < size; j++) {
                sum[i][j] = (*this)[i][j] + m[i][j];
            }
        }
        return sum;
    }

    Matrix operator-(const Matrix &m) const {
        if (m.getSize() != getSize())
            throw std::invalid_argument("Matrices a and b must have the same dimensions");
        Matrix sum(size);
        for (size_t i = 0; i < size; i++) {
            for (size_t j = 0; j < size; j++) {
                sum[i][j] = (*this)[i][j] - m[i][j];
            }
        }
        return sum;
    }

    Matrix operator*(const Matrix &m) const {
        if (m.getSize() != size)
            throw std::invalid_argument("Matrices a and b must have the same dimensions");
        Matrix multi(size);
        for (size_t i = 0; i < size; i++) {
            for (size_t j = 0; j < size; j++) {
                multi[i][j] = 0;
                for (size_t u = 0; u < size; u++)
                    multi[i][j] += (*this)[i][u] * m[u][j];
            }
        }
        return multi;
    }

    Matrix T() {
        Matrix trans(size);
        for (size_t i = 0; i < size; i++) {
            for (size_t j = 0; j < size; j++)
                trans[i][j] = (*this)[j][i];
        }
        return trans;
    }

    Matrix operator!() {
        return T();
    }

    Matrix operator-() const {
        Matrix negative(size);
        for (size_t i = 0; i < size; i++) {
            for (size_t j = 0; j < size; j++)
                negative[i][j] = -(*this)[i][j];
        }
        return negative;
    }

    friend std::ostream &operator<<(std::ostream &os, const Matrix &m) {
        for (size_t i = 0; i < m.size; i++) {
            os << m[i];
            os << "\n";
        }
        return os;
    }

    friend std::istream &operator>>(std::istream &os, const Matrix &m) {
        for (size_t i = 0; i < m.size * m.size; i++) {
            os >> m.matrix[i];
        }
        return os;
    }

    Matrix operator()(size_t row, size_t column) const {
        return getMinor(row, column);
    }

    MatrixRow operator[](size_t row) const {
        if (size == 0 || size <= row)
            throw std::invalid_argument("Matrix has not valid dimension");
        return {size, row, matrix};
    }


    MatrixColumn operator()(size_t column) const {
        if (size == 0 || size <= column)
            throw std::invalid_argument("Matrix has not valid dimension");
        return {size, column, matrix};
    }


    friend bool operator==(const Matrix &m1, const Matrix &m2) {
        return is_equal(m1, m2);
    }

    friend bool operator!=(const Matrix &m1, const Matrix &m2) {
        return !is_equal(m1, m2);
    }
};


int main() {
    std::ofstream fout("output.txt");
    std::ifstream fin("input.txt");

    int N, k;

    fin >> N >> k;

    Matrix A(N), B(N), C(N), K(N, k), D(N);

    fin >> A >> B >> C >> D;

    //fout << (A + B * (C.T()) + K) * (D.T());

    Matrix d = (A + B * (C.T()) + K) * (D.T());

//    fout << d << d(1) << d[1];
//
//    d[1][0] = 4;
//
//    fout<<d;
//
//    d(1)[0] =5;

    fout << d;
//    fout << d[0] << std::endl;
//    std::cout << d(0);
    return 0;
}
#include <iostream>
#include <fstream>


class MatrixRow {
private:
    size_t rowIdx;
    const size_t matrixSize;
    int *matrix;
public:
    MatrixRow() : rowIdx(0), matrixSize(0), matrix(nullptr) {}

    MatrixRow(size_t size, size_t row, int *m) : rowIdx(row), matrixSize(size), matrix(m) {}

    int &operator[](size_t col) {
        return matrix[rowIdx * matrixSize + col];
    }

    friend std::ofstream &operator<<(std::ofstream &os, const MatrixRow &m) {
        for (size_t i = 0; i < m.matrixSize; i++) {
            os << m.matrix[m.rowIdx * m.matrixSize + i];
            if (i + 1 != m.matrixSize)
                os << " ";
            os << "\n";
        }
        return os;
    }

    friend std::ostream &operator<<(std::ostream &os, const MatrixRow &m) {
        for (size_t i = 0; i < m.matrixSize; i++) {
            os << m.matrix[m.rowIdx * m.matrixSize + i];
            if (i + 1 != m.matrixSize)
                os << " ";
            os << "\n";
        }
        return os;
    }

};

class MatrixColumn {
private:
    size_t colIdx;
    const size_t matrixSize;
    int *matrix;
public:
    MatrixColumn() : colIdx(0), matrixSize(0), matrix(nullptr) {}

    MatrixColumn(size_t size, size_t row, int *m) : colIdx(row), matrixSize(size), matrix(m) {}

    int &operator[](size_t row) {
        return matrix[row * matrixSize + colIdx];
    }

    friend std::ofstream &operator<<(std::ofstream &os, const MatrixColumn &m) {
        for (size_t i = 0; i < m.matrixSize; i++) {
            os << m.matrix[i * m.matrixSize + m.colIdx];
            if (i + 1 != m.matrixSize)
                os << " ";
            os << "\n";
        }
        return os;
    }

    friend std::ostream &operator<<(std::ostream &os, const MatrixColumn &m) {
        for (size_t i = 0; i < m.matrixSize; i++) {
            os << m.matrix[i * m.matrixSize + m.colIdx];
            if (i + 1 != m.matrixSize)
                os << " ";
            os << "\n";
        }
        return os;
    }
};


class Matrix {
private:
    const size_t size;
    int *matrix;

    void init_empty(int default_value = 1) {
        for (size_t i = 0; i < size * size; ++i) {
            matrix[i] = 0;

        }
        for (size_t i = 0; i < size; ++i)
            matrix[i * size + i] = default_value;
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
            matrix[i * size + i] = diagonal[i];
    }

    ~Matrix() {
        delete[] matrix;
    }

    size_t getSize() const {
        return size;
    }

    Matrix getMinor(size_t row, size_t column) const {
        if (size == 0 || size < row || size < column)
            throw std::invalid_argument("Matrix has not valid dimension");

        Matrix minor(size - 1);
        bool new_row = false, new_column = false;
        for (size_t i = 0; i < size; i++) {
            if (i == row) {
                new_row = true;
                continue;
            }
            for (size_t j = 0; j < size; j++) {
                if (j == column) {
                    new_row = true;
                    continue;
                }
                minor.matrix[(i - new_row) * size + j - new_column] = matrix[i * size + j];
            }
        }
        return minor;
    }

    Matrix operator+(const Matrix &b) {
        if (b.getSize() != getSize())
            throw std::invalid_argument("Matrices a and b must have the same dimensions");
        size_t sum_size = getSize();
        Matrix sum(sum_size);
        for (size_t i = 0; i < sum_size; i++) {
            for (size_t j = 0; j < sum_size; j++) {
                sum.matrix[i * size + j] = matrix[i * size + j] + b.matrix[i * size + j];
            }
        }
        return sum;
    }

    Matrix operator-(const Matrix &b) {
        if (b.getSize() != getSize())
            throw std::invalid_argument("Matrices a and b must have the same dimensions");
        size_t sum_size = getSize();
        Matrix sum(sum_size);
        for (size_t i = 0; i < sum_size; i++) {
            for (size_t j = 0; j < sum_size; j++) {
                sum.matrix[i * size + j] = matrix[i * size + j] - b.matrix[i * size + j];
            }
        }
        return sum;
    }

    Matrix operator*(const Matrix &b) {
        if (b.getSize() != getSize())
            throw std::invalid_argument("Matrices a and b must have the same dimensions");

        size_t multi_size = getSize();
        Matrix multi(multi_size);
        for (size_t i = 0; i < multi_size; i++) {
            for (size_t j = 0; j < multi_size; j++) {
                multi.matrix[i * size + j] = 0;
                for (size_t u = 0; u < multi_size; u++)
                    multi.matrix[i * size + j] += matrix[i * size + u] * b.matrix[u * size + j];
            }
        }
        return multi;
    }

    Matrix T() {
        Matrix trans(size);
        for (size_t i = 0; i < size; i++) {
            for (size_t j = 0; j < size; j++)
                trans.matrix[i * size + j] = matrix[j * size + i];
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
                negative.matrix[i * size + j] = -matrix[i * size + j];
        }
        return negative;
    }

    friend std::ofstream &operator<<(std::ofstream &os, const Matrix &m) {
        for (size_t i = 0; i < m.size; i++) {
            for (size_t j = 0; j < m.size; j++) {
                os << m.matrix[i * m.size + j];
                if (j + 1 != m.size)
                    os << " ";
            }
            os << "\n";
        }
        return os;
    }

    friend std::ifstream &operator>>(std::ifstream &os, const Matrix &m) {
        for (size_t i = 0; i < m.size * m.size; i++) {
            os >> m.matrix[i];
        }
        return os;
    }


    friend std::ostream &operator<<(std::ostream &os, const Matrix &m) {
        for (size_t i = 0; i < m.size; i++) {
            for (size_t j = 0; j < m.size; j++) {
                os << m.matrix[i * m.size + j];
                if (j + 1 != m.size)
                    os << " ";
            }
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

    MatrixRow operator[](size_t row) {
        if (size == 0 || size <= row)
            throw std::invalid_argument("Matrix has not valid dimension");
        return {size, row, matrix};
    }


    MatrixColumn operator()(size_t column) {
        if (size == 0 || size <= column)
            throw std::invalid_argument("Matrix has not valid dimension");
        return {size, column, matrix};
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
//    fout << d(0,1);
    return 0;
}

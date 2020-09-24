#include <iostream>
#include <fstream>

class Matrix {
private:
    const size_t size;
    int **matrix;


public:
    Matrix() : size(0),
               matrix(nullptr) {
    }

    Matrix(const size_t new_size) : size(new_size),
                                    matrix(new int *[new_size]) {
        for (size_t i = 0; i < new_size; ++i) {
            matrix[i] = new int[new_size];
            for (size_t j = 0; j < new_size; ++j)
                matrix[i][j] = 0;
            matrix[i][i] = 1;
        }
    }

    Matrix(const size_t new_size, const size_t k) : size(new_size),
                                                    matrix(new int *[new_size]) {
        for (size_t i = 0; i < new_size; ++i) {
            matrix[i] = new int[new_size];
            for (size_t j = 0; j < new_size; ++j)
                matrix[i][j] = 0;
            matrix[i][i] = k;
        }
    }

    Matrix(size_t new_size, const int diagonal[]) : Matrix(new_size) {
        for (size_t i = 0; i < new_size; i++)
            matrix[i][i] = diagonal[i];
    }

    ~Matrix() {
        for (size_t i = 0; i < size; i++)
            delete[] matrix[i];
        delete[] matrix;
    }

    size_t getSize() const {
        return size;
    }

//    int **getMatrix() {
//        return matrix;
//    }

    Matrix getMinor(size_t row, size_t column) const {
        if (size == 0 || size < row || size < column)
            throw std::invalid_argument("Matrix has not valid dimension");

        Matrix minor(size - 1);
        bool new_row = false, new_column = false;
        for (size_t i = 0; i < size; i++) {
            if (i == row - 1) {
                new_row = true;
                continue;
            }
            for (size_t j = 0; j < size; j++) {
                if (i == column - 1) {
                    new_row = true;
                    continue;
                }
                minor.matrix[i - new_row][j - new_column] = matrix[i][j];
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
                sum.matrix[i][j] = matrix[i][j] + b.matrix[i][j];
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
                sum.matrix[i][j] = matrix[i][j] - b.matrix[i][j];
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
                multi.matrix[i][j] = 0;
                for (size_t u = 0; u < multi_size; u++)
                    multi.matrix[i][j] += matrix[i][u] * b.matrix[u][j];
            }
        }
        return multi;
    }

    Matrix T() {
        Matrix trans(size);
        for (size_t i = 0; i < size; i++) {
            for (size_t j = 0; j < size; j++)
                trans.matrix[i][j] = matrix[j][i];
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
                negative.matrix[i][j] = -matrix[i][j];
        }
        return negative;
    }

    friend std::ofstream &operator<<(std::ofstream &os, const Matrix &m) {
        for (size_t i = 0; i < m.size; i++) {
            for (size_t j = 0; j < m.size; j++) {
                os << m.matrix[i][j];
                if (j + 1 != m.size)
                    os << " ";
            }
            os << "\n";
        }
        return os;
    }

    friend std::ifstream &operator>>(std::ifstream &os, const Matrix &m) {
        for (size_t i = 0; i < m.size; i++) {
            for (size_t j = 0; j < m.size; j++) {
                os >> m.matrix[i][j];
            }
        }
        return os;
    }


    friend std::ostream &operator<<(std::ostream &os, const Matrix &m) {
        for (size_t i = 0; i < m.size; i++) {
            for (size_t j = 0; j < m.size; j++) {
                os << m.matrix[i][j];
                if (j + 1 != m.size)
                    os << " ";
            }
            os << "\n";
        }
        return os;
    }

    friend std::istream &operator>>(std::istream &os, const Matrix &m) {
        for (size_t i = 0; i < m.size; i++) {
            for (size_t j = 0; j < m.size; j++) {
                os >> m.matrix[i][j];
            }
        }
        return os;
    }

    Matrix operator()(size_t row, size_t column) const {
        return getMinor(row, column);
    }

    int *operator[](size_t row) {
        if (size == 0 || size <= row)
            throw std::invalid_argument("Matrix has not valid dimension");
        return matrix[row];
    }




    int * operator()(size_t column){
        if (size == 0 || size <= column)
            throw std::invalid_argument("Matrix has not valid dimension");
        return T().matrix[column]; // TODO: rewrite to save ref not new instant
    }

};


int main() {
    std::ofstream fout("output.txt");
    std::ifstream fin("input.txt");

    int N, k;

    fin >> N >> k;

    Matrix A(N), B(N), C(N), K(N, k), D(N);

    fin >> A >> B >> C >> D;

    fout << (A + B * (C.T()) + K) * (D.T());
    return 0;
}

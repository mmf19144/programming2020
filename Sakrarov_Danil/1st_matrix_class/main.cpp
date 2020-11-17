#include <iostream>
#include <fstream>
#include <cassert>
#include <cstring>

class Matrix_row {
private:
    const size_t size;
    const size_t row_index;
    int *matrix;

public:
    Matrix_row(const size_t row_) : size(0), matrix(nullptr), row_index(row_) {}

    Matrix_row(const size_t size_, const size_t row_, int *m) : row_index(row_), size(size_), matrix(m) {
    }

    int &operator[](size_t col) {
        return matrix[row_index * size + col];
    }


    friend std::ostream &operator<<(std::ostream &os, const Matrix_row &obj) {
        for (size_t i = 0; i < obj.size; i++) {
            os << obj.matrix[obj.row_index * obj.size + i];
            if (i + 1 != obj.size)
                os << " ";
        }
        return os;
    }

};

class Matrix_col {
private:
    const size_t size;
    const size_t col_index;
    int *matrix;
public:
    Matrix_col(const size_t row_) : size(0), matrix(nullptr), col_index(row_) {}

    Matrix_col(const size_t size_, const size_t row_, int *m) : col_index(row_), size(size_), matrix(m) {
    }

    int &operator[](size_t row) {
        return matrix[row * size + col_index];
    }

    friend std::ostream &operator<<(std::ostream &os, const Matrix_col &m) {
        for (size_t i = 0; i < m.size; i++) {
            os << m.matrix[i * m.size + m.col_index];
            if (i + 1 != m.size)
                os << " ";
        }
        return os;
    }

};

class Matrix {
private:
    const size_t size;
    int *matrix;


    Matrix_row operator[](const size_t row) const {
        if (size == 0 || size <= row)
            throw std::invalid_argument("not valid argument for matrix");
        return {size, row, matrix};
    }

    Matrix_col operator()(const size_t column) const {
        if (size == 0 || size <= column)
            throw std::invalid_argument("not valid argument for matrix");
        return {size, column, matrix};
    }

public:

    Matrix() : size(0), matrix(nullptr) {
    }

    Matrix(const size_t size_) : size(size_), matrix(new int[size * size]) {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (i == j)
                    matrix[i * size + i] = 1;
                else
                    matrix[i * size + j] = 0;
            }

        }
    }

    Matrix(size_t size_, int element) : size(size_), matrix(new int[size * size]) {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (i == j)
                    matrix[i * size + i] = element;
                else
                    matrix[i * size + j] = 0;
            }

        }
    }

    Matrix(const Matrix &obj) : size(obj.size), matrix(new int[size * size]) {
        assert(obj.matrix != nullptr);
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                (*this)[i][j] = obj[i][j];
            }
        }
    }

    Matrix &operator=(const Matrix &obj) {
        assert(obj.matrix != nullptr);
        if (obj.size != size)
            throw std::invalid_argument("Matrix dimensions must agree");
        if (&obj == this) {
            return *this;
        }
        for (int i = 0; i < obj.size; i++) {
            for (int j = 0; j < obj.size; j++) {
                (*this)[i][j] = obj[i][j];
            }
        }

        return *this;

    }


    Matrix_row operator[](const size_t row) {
        if (size == 0 || size <= row)
            throw std::invalid_argument("not valid argument for matrix");
        return {size, row, matrix};
    }


    Matrix_col operator()(const size_t column) {
        if (size == 0 || size <= column)
            throw std::invalid_argument("not valid argument for matrix");
        return {size, column, matrix};
    }

    ~Matrix() {
        delete[] matrix;
    }

    Matrix operator+(const Matrix &obj) {
        assert(matrix != nullptr);
        assert(obj.matrix != nullptr);
        if (obj.size != size)
            throw std::invalid_argument("Matrix dimensions must agree");
        Matrix tmp(size);

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++)
                tmp[i][j] = (*this)[i][j] + obj[i][j];
        }
        return tmp;
    }

    Matrix operator-(const Matrix &obj) {
        assert(matrix != nullptr);
        assert(obj.matrix != nullptr);
        if (obj.size != size)
            throw std::invalid_argument("Matrix dimensions must agree");
        Matrix tmp(size);
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++)
                tmp[i][j] = (*this)[i][j] - obj[i][j];
        }
        return tmp;
    }

    Matrix operator*(const Matrix &obj) {
        assert(matrix != nullptr);
        assert(obj.matrix != nullptr);
        if (obj.size != size)
            throw std::invalid_argument("Matrix dimensions must agree");
        Matrix tmp(size);

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                tmp.matrix[i * size + j] = 0;
                for (int k = 0; k < size; k++) {
                    tmp[i][j] += (*this)[i][k] * obj[k][j];
                }
            }
        }
        return tmp;
    }

    bool operator==(const Matrix &obj) {
        assert(matrix != nullptr);
        assert(obj.matrix != nullptr);
        if (obj.size != size)
            throw std::invalid_argument("Matrix dimensions must agree");
        return !memcmp(matrix, obj.matrix, size * size * sizeof(int));
    }

    friend bool operator!=(Matrix &obj1, Matrix &obj2) {
        return !(obj1 == obj2);
    }

    Matrix operator!() {
        assert(matrix != nullptr);
        Matrix tmp(size);
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                tmp[i][j] = (*this)[j][i];
            }
        }
        return tmp;
    }

    Matrix operator()(const size_t row, const size_t column) {
        Matrix tmp(size - 1);
        int was_i = 0;
        int was_j = 0;
        for (int i = 0; i < size; i++) {
            if (i == row) {
                was_i = 1;
                continue;
            }
            for (int j = 0; j < size; j++) {
                if (j == column) {
                    was_j = 1;
                    continue;
                }
                tmp[i - was_i][j - was_j] = (*this)[i][j];
            }
        }
        return tmp;
    }

    friend std::ostream &operator<<(std::ostream &os, const Matrix &obj) {
        for (int i = 0; i < obj.size; i++) {
            for (int j = 0; j < obj.size; j++) {
                os << obj.matrix[i * obj.size + j];
                if (j + 1 != obj.size) {
                    os << ' ';
                }
            }
            os << std::endl;
        }
        return os;
    }

    friend std::istream &operator>>(std::istream &os, Matrix &obj) {
        for (int i = 0; i < obj.size; i++) {
            for (int j = 0; j < obj.size; j++) {
                os >> obj[i][j];
            }
        }
        return os;
    }


};

int main() {
    std::ifstream fin("input.txt");
    std::ofstream fout("output.txt");
    int size, k_matr;
    fin >> size >> k_matr;
    Matrix a(size), b(size), c(size), k(size, k_matr), d(size);
    fin >> a >> b >> c >> d;
    Matrix p = (a + (b * (!c)) + k) * (!d);
    fout << (a + b * (!c) + k) * (!d);

    return 0;
}

#include <iostream>
#include <fstream>
#include <cstring>


class Row {
    const size_t row_id;
    const size_t matrix_size;
    int *const matrix_data;

public:
    Row(const size_t idx, const size_t size, int *const data)
            : row_id(idx), matrix_size(size), matrix_data(data) {
    }

    int &operator[](size_t col_id) {
        if (col_id > matrix_size || col_id < 1) {
            throw std::invalid_argument("Invalid column index");
        }
        col_id--;

        return matrix_data[row_id * matrix_size + col_id];
    }

    friend std::ostream &operator<<(std::ostream &stream, const Row &row) {
        for (size_t i = 0; i < row.matrix_size; i++) {
            stream << row.matrix_data[row.row_id * row.matrix_size + i];
            if (i + 1 == row.matrix_size)
                break;
            stream << " ";
        }
        return stream;
    }
};

class Column {
    const size_t col_id;
    const size_t matrix_size;
    int *const matrix_data;
public:
    Column(const size_t idx, const size_t size, int *const data)
            : col_id(idx), matrix_size(size), matrix_data(data) {
    }

    int &operator[](size_t row_id) {
        if (row_id > matrix_size || row_id < 1) {
            throw std::invalid_argument("Invalid row index");
        }
        row_id--;

        return matrix_data[row_id * matrix_size + col_id];
    }

    friend std::ostream &operator<<(std::ostream &stream, const Column &col) {
        for (size_t i = 0; i < col.matrix_size; i++) {
            stream << col.matrix_data[i * col.matrix_size + col.col_id];
            if (i + 1 == col.matrix_size)
                break;
            stream << " ";
        }
        return stream;
    }
};

class Matrix {
public:
    const size_t matrix_size;
private:
    int *const data;

public:

    Matrix()
            : matrix_size(0), data(nullptr) {
    }

    explicit Matrix(const size_t size)
            : matrix_size(size), data(new int[matrix_size * matrix_size]()) {
        memset(data, 0, matrix_size * matrix_size * sizeof(int));
        for (size_t i = 0; i < matrix_size * matrix_size; i += (matrix_size + 1)) {
            data[i] = 1;
        }
    }

    Matrix(const size_t size, const int *arr)
            : matrix_size(size), data(new int[matrix_size * matrix_size]()) {
        memset(data, 0, matrix_size * matrix_size * sizeof(int));
        for (size_t i = 0, j = 0; i < matrix_size * matrix_size; i += (matrix_size + 1), j++) {
            data[i] = arr[j];
        }
    }


    ~Matrix() {
        delete[] data;
    }

    Matrix operator+(const Matrix &other) {
        if (matrix_size != other.matrix_size) {
            throw std::invalid_argument("Invalid size of matrix ");
        }
        Matrix result_matrix(matrix_size);
        for (size_t i = 0; i < matrix_size * matrix_size; i++) {
            result_matrix.data[i] = data[i] + other.data[i];
        }
        return result_matrix;
    }

    Matrix operator-(const Matrix &other) {
        if (matrix_size != other.matrix_size) {
            throw std::invalid_argument("Invalid size of matrix ");
        }
        Matrix result_matrix(matrix_size);
        for (size_t i = 0; i < matrix_size * matrix_size; i++) {
            result_matrix.data[i] = data[i] - other.data[i];
        }
        return result_matrix;
    }

    Matrix operator*(const Matrix &other) {
        if (matrix_size != other.matrix_size) {
            throw std::invalid_argument("Invalid size of matrix ");
        }
        Matrix result_matrix(matrix_size);

        for (size_t row_id = 0; row_id < matrix_size; row_id++) {
            for (size_t col_id = 0; col_id < matrix_size; col_id++) {
                result_matrix[row_id + 1][col_id + 1] = 0;
                for (size_t k = 0; k < matrix_size; k++) {
                    result_matrix[row_id + 1][col_id + 1] +=
                            (*this)[row_id + 1][k + 1] * other[k + 1][col_id + 1];
                }
            }
        }
        return result_matrix;
    }

    bool operator==(const Matrix &other) {
        if (matrix_size != other.matrix_size) {
            throw std::invalid_argument("Invalid size of matrix ");
        }
        return memcmp(data, other.data, matrix_size * matrix_size * sizeof(int)) == 0;
    }

    bool operator!=(const Matrix &other) {
        return !(*this == other);
    }

    Matrix operator()(size_t row, size_t col) {
        if (col > matrix_size || row > matrix_size || col < 1 || row < 1) {
            throw std::invalid_argument("Invalid column and row indexes");
        }
        row--;
        col--;
        if (matrix_size < 1) {
            throw std::invalid_argument("Invalid matrix size");
        }
        Matrix result_matrix(matrix_size - 1);
        for (size_t i = 0; i < matrix_size; i++) {
            if (i == row) continue;
            for (size_t j = 0; j < matrix_size; j++) {
                if (j == col) continue;
                int row_shift = 0;
                int col_shift = 0;
                if (i > row) row_shift = -1;
                if (j > col) col_shift = -1;
                result_matrix[i + row_shift + 1][j + col_shift + 1] = (*this)[i+1][j+1];
            }
        }
        return result_matrix;
    }

    Column operator()(size_t col_id) const {
        if (col_id > matrix_size || col_id < 1) {
            throw std::invalid_argument("Invalid column index");
        }
        col_id--;
        return {col_id, matrix_size, data};
    }

    Row operator[](size_t row_id) const {
        if (row_id > matrix_size || row_id < 1) {
            throw std::invalid_argument("Invalid row index");
        }
        row_id--;
        return {row_id, matrix_size, data};
    }


    Matrix transposed() {
        Matrix result_matrix(matrix_size);
        for (size_t i = 0; i < matrix_size; i++) {
            for (size_t j = 0; j < matrix_size; j++) {
                result_matrix[i+1][j+1] = (*this)[j+1][i+1];
            }
        }
        return result_matrix;
    }


    friend std::ostream &operator<<(std::ostream &stream, const Matrix &matrix) {
        for (size_t i = 0; i < matrix.matrix_size; i++) {
            stream << matrix[i + 1];
            stream << "\n";
        }
        return stream;
    }

    friend std::istream &operator>>(std::istream &stream, const Matrix &matrix) {
        for (size_t i = 0; i < matrix.matrix_size * matrix.matrix_size; i++) {
            stream >> matrix.data[i];
        }
        return stream;
    }

};
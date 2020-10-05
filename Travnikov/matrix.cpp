#define CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <cstdio>
#include <stdio.h>
using namespace std;


class Matrix {
    int *data;
    int dim;

public:

    friend void readMatrix(std::ifstream &is, Matrix &m);

    friend void writeMatrix(std::ofstream &os, Matrix &m);


    Matrix() {
        this->dim = 0;
        this->data = nullptr;
    }
    ~Matrix() {
        delete[] data;

    }

    Matrix(int num, bool singular = true) {
        this->dim = num;

        this->data = new int[num * num];

        for (int i = 0; i < num; i++) {
            for (int j = 0; j < num; j++) {
                if (singular) if (i == j) { this->data[i * num + i] = 1; }
                this->data[i * num + j] = 0;
            }
        }
        //for (int i = 0; i < num; i++) { this->data[i][i] = 1; }
    }

    Matrix(int num, int const *arr) {
        this->dim = num;
        this->data = new int[num*num];

        for (int i = 0; i < num; i++) {
            for (int j = 0; j < num; j++) {
                this->data[i * num + j] = 0;
            }
        }
        for (int i = 0; i < num; i++) {
            this->data[i * num + i] = arr[i];
        }
    }

    Matrix(int num, int de) {
        this->dim = num;
        this->data = new int[num*num];

        for (int i = 0; i < num; i++) {
            for (int j = 0; j < num; j++) {
                this->data[i * num + j] = 0;
            }
        }
        for (int i = 0; i < num; i++) {
            this->data[i * num + i] = de;
        }
    }



    Matrix operator+(const Matrix &that) const {
        if (this->dim != that.dim) {
            throw std::invalid_argument("Matrices a and b must have the same dimensions");
        }
        Matrix result(this->dim);
        for (int i = 0; i < this->dim; i++) {
            for (int j = 0; j < this->dim; j++) {
                result.data[i * this->dim + j] = this->data[i * this->dim + j] + that.data[i * this->dim + j];
            }
        }
        return result;
    }

    Matrix operator-(const Matrix &that) const {
        if (this->dim != that.dim) {
            throw std::invalid_argument("Matrices a and b must have the same dimensions");
        }
        Matrix result(this->dim);
        for (int i = 0; i < this->dim; i++) {
            for (int j = 0; j < this->dim; j++) {
                result.data[i * this->dim + j] = this->data[i * this->dim + j] - that.data[i * this->dim + j];
            }
        }
        return result;
    }

    Matrix operator*(const Matrix &that) const {
        Matrix result(this->dim, false);
        if (this->dim != that.dim) {
            throw std::invalid_argument("Matrices a and b must have the same dimensions");
        }

        for (int i = 0; i < this->dim; i++) {
            for (int j = 0; j < this->dim; j++) {
                for (int k = 0; k < this->dim; k++) {
                    result.data[i * this->dim + j] += this->data[i * this->dim + k] * that.data[k * this->dim + j];
                }
            }
        }
        return result;
    }



    Matrix operator!() const {
        Matrix result(this->dim);
        for (int i = 0; i < this->dim; i++) {
            for (int j = 0; j < this->dim; j++) {
                result.data[i * this->dim + j] = this->data[j * this->dim + i];
            }
        }
        return result;
    }

    Matrix operator()(int r, int c) {
        Matrix result((this->dim) - 1, false);
        int minor_row, minor_col;

        for (int i = 0; i < 3; i++) {
            minor_row = i;
            if (i > r)
                minor_row--;
            for (int j = 0; j < 3; j++) {
                minor_col = j;
                if (j > c)
                    minor_col--;
                if (i != r && j != c)
                    result.data[minor_row * this->dim + minor_col] = this->data[i * this->dim + j];
            }
        }
        return result;
    }

    int *operator()(int col) {
        int *result = new int[this->dim];
        for (int n = 0; n < this->dim; n++) result[n] = this->data[n * this->dim + col];
        return result;
    }

    int *operator[](int row) {
        int *result = new int[this->dim];
        for (int n = 0; n < this->dim; n++) result[n] = this->data[row * this->dim + n];
        return result;
    }



};

void readMatrix(std::ifstream &is, Matrix &m) {
    for (int i = 0; i < m.dim; i++) {
        for (int j = 0; j < m.dim; j++) {
            is >> m.data[i * m.dim + j];
        }
    }
}

void writeMatrix(std::ofstream &os, Matrix &m) {
    for (int i = 0; i < m.dim; i++) {
        for (int j = 0; j < m.dim; j++) {
            os << m.data[i * m.dim + j]<< " " ;
        }
        os << endl;
    }
}


int main() {


    ifstream fin("input.txt");

    int msize, k;

    fin >> msize >> k;

    Matrix A(msize);
    Matrix B(msize);
    Matrix C(msize);
    Matrix D(msize);

    readMatrix(fin, A);
    readMatrix(fin, B);
    readMatrix(fin, C);
    readMatrix(fin, D);

    ofstream fout("output.txt");

    Matrix K(msize, k);


    Matrix R = (A + B * (!C) + K) * (!D);

    writeMatrix(fout, R);


    fin.close();
    fout.close();

    return 0;


}
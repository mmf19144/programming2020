#include <iostream>
#include <fstream>
#include <stdio.h>

using namespace std;


class Matrix {
    int *data;
    int dim;
    int len;
public:

    friend void readMatrix(std::ifstream &is, Matrix &m);

    friend void writeMatrix(std::ofstream &os, Matrix &m);


    Matrix() {}

    ~Matrix() {
        delete[] data;
    }

    Matrix(int num, bool singular = true) {
        this->dim = num;
        this->len = num * num;
        this->data = new int[num * num];

        for (int i = 0; i < num; i++) {
            for (int j = 0; j < num; j++) {
                if (singular) if (i == j) { this->data[i * num + i] = 1; }
                this->data[i * num + j] = 0;
            }
        }
        //for (int i = 0; i < num; i++) { this->data[i][i] = 1; }
    }

    Matrix(int num, int *arr) {
        this->dim = num;
        this->data = new int[num];

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
        this->data = new int[num];

        for (int i = 0; i < num; i++) {
            for (int j = 0; j < num; j++) {
                this->data[i * num + j] = 0;
            }
        }
        for (int i = 0; i < num; i++) {
            this->data[i * num + i] = de;
        }
    }

    int getElem(int a, int b) {
        return this->data[a * (this->dim) + b];
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

    Matrix operator^(const Matrix &that) const {
        for (int i = 0; i < this->dim; i++) {
            for (int j = 0; j < this->dim; j++) {
                if (this->data[i * this->dim + j] != that.data[i * this->dim + j]) {
                    //printf("F");
                    return false;
                }
            }
        }
        //printf("T");
        return true;
    }

    Matrix operator!() {
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

    /*Matrix operator[](int row){
        Matrix result(this->dim,false);
        or(int n=0; n<this->dim; n++) result[n] = this->data[row*this->dim+n]
    }*/
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
            os << m.data[i * m.dim + j] << " ";
        }
        os << endl;
    }
}


int main() {
    int a = 0;
    int tstarr1[4] = {1, 2, 3, 4};
    int tstarr2[4] = {5, 6, 7, 8};
    /*Matrix mtx1(4, tstarr1);
    Matrix mtx2(4, tstarr2);
    Matrix mtx3 =  mtx2 (1,3);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%d", mtx3.getElem(i, j));
        }
        printf("\n");

    }*/

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

    ofstream fout("result.txt");

    Matrix K(msize, k);
    Matrix R(msize, false);

    R = (A + B * !C + K) * !D;

    writeMatrix(fout, R);
    //fout <<R[1][1] << "\t"<< msize;

    fin.close();
    fout.close();






    //scanf("%d",&a);
    //std::cout << "Hello, World!" << std::endl;
    //printf("%d",a);
    return 0;


}

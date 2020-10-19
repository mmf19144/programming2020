
#include <iostream>
#include <fstream>


using namespace std;


class Matrix {
    int *data;
    int dim;

public:




    Matrix():
        dim(0), data(nullptr){
    }
    ~Matrix() {
        delete[] data;

    }

    Matrix(int num, bool singular = true) {
        this->dim = num;

        this->data = new int[num * num];

        for (int i = 0; i < num; i++) {
            for (int j = 0; j < num; j++) {
                if (singular) {
                    if (i == j) { this->data[i * num + i] = 1; }
                }
                this->data[i * num + j] = 0;
            }
        }

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

        for (int i = 0; i < this-> dim; i++) {
            minor_row = i;
            if (i > r)
                minor_row--;
            for (int j = 0; j < this->dim; j++) {
                minor_col = j;
                if (j > c)
                    minor_col--;
                if (i != r && j != c)
                    result.data[minor_row * this->dim + minor_col] = this->data[i * this->dim + j];
            }
        }
        return result;
    }

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
    friend std::ostream& operator<< (std::ostream &out, Matrix& m) {

        for (int i = 0; i < m.dim; i++) {
            for (int j = 0; j < m.dim; j++)
                out << m.data[i*m.dim + j] << " ";
            out << std::endl;
        }

        return out;
    }

    friend std::istream& operator>>(std::istream &in, Matrix& m) {

        for (int i = 0; i < m.dim; i++) {
            for (int j = 0; j < m.dim; j++)
                in >>m.data[i*m.dim+j];
        }

        return in;
    }

};




int main() {


    ifstream fin("input.txt");

    int msize, k;

    fin >> msize >> k;

    Matrix A(msize);
    Matrix B(msize);
    Matrix C(msize);
    Matrix D(msize);
    Matrix K(msize, k);


    ofstream fout("output.txt");




    Matrix R = (A + B * (!C) + K) * (!D);


    fin >> A >> B >> C >> D;

    Matrix example_result = ((A + B * (!C) + K) * (!D));

    fout << example_result;

    fin.close();
    fout.close();

    return 0;


}
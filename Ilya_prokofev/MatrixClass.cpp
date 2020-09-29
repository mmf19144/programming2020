#include <iostream>
#include <algorithm>
#include <fstream>
using namespace std;

class column
{
    int ** matrix ;
    const int dim;
    const unsigned int column_idx;

public:
    column(const int dim, int** m, const unsigned int col_id) : matrix(m), dim(dim), column_idx(col_id) {}

    int& operator[](const size_t index)//column element reference
    {
        if (index > this->dim)
        {
            throw "Column index is greater than dimension.";
        }
        return matrix[index-1][column_idx];
    }
};

class raw
{
    int** matrix;
    const int dim;
    const unsigned int raw_id;

public:
    raw(const int dim, int** m, const unsigned int _raw_id) : matrix(m), dim(dim), raw_id(_raw_id) {}

    int& operator[](const size_t index)//raw element reference
    {
        if (index > this->dim)
        {
            throw "Column index is greater than dimension.";
        }
        return matrix[raw_id][index-1];
    }
};

class Matrix
{
    int** data; //matrix
    int dim; //dimension
    int** allocate_matrix (int n) // memory allocation
    {
        int** L = new int* [n];
        for (int i = 0; i < n; i++)
        {
            L[i] = new int[n];
        }
        return L;
    }

    void clear_matrix()
    {
        for (int i = 0; i < dim; i++)
        {
            delete[] data[i];
        }
        delete[] data;
    }
public:
    Matrix(): dim(0), data(nullptr) //default constructor
    {}
    Matrix(int n) :dim(n) //identity matrix constructor
    {
        data = allocate_matrix(n);
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (i == j)
                {
                    data[i][j] = 1;
                }
                else
                {
                    data[i][j] = 0;
                }
            }
        }
    }
    Matrix(int n, int* D):dim(n) //diagonal matrix constructor
    {
        data = allocate_matrix(n);
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (i == j)
                {
                    data[i][j] = D[i];
                }
                else
                {
                    data[i][j] = 0;
                }
            }
        }
    }
    Matrix(const Matrix& that):dim (that.dim) //copy constructor
    {
        this->data = allocate_matrix(dim);
        for (int i = 0; i < dim; i++)
        {
            for (int j = 0; j < dim; j++)
            {
                this->data[i][j] = that.data[i][j];
            }
        }
    }
    Matrix(int dimension, ifstream& input):dim(dimension) //constructor reading from stream
    {
        this->data = allocate_matrix(dim);
        for (int i = 0; i < dim; i++)
        {
            for (int j = 0; j < dim; j++)
            {
                input >> this->data[i][j];
            }
        }
    }

    int dimension()
    {
        return this->dim;
    }

    Matrix& operator=(const Matrix& that)
    {
        if (*this != that)
        {
            this->clear_matrix();
            this->dim = that.dim;
            this->data = allocate_matrix(dim);
            for (int i = 0; i < dim; i++)
            {
                for (int j = 0; j < dim; j++)
                {
                    this->data[i][j] = that.data[i][j];
                }
            }
        }

        return *this;
    }
    Matrix operator+(const Matrix& that) const //matrix addition //function does not change this
    {
        if (this->dim != that.dim)
        {
            throw "wrong dimention";
        }
        int n1 = this->dim;
        Matrix temp(n1);

        for (int i = 0; i < n1; i++)
        {
            for (int j = 0; j < n1; j++)
            {
                temp.data[i][j] = this->data[i][j] + that.data[i][j];
            }
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
            throw "wrong dimention";
        }
        int n1 = this->dim;
        Matrix temp(n1);

        for (int i = 0; i < n1; i++)
        {
            for (int j = 0; j < n1; j++)
            {
                temp.data[i][j] = this->data[i][j] - that.data[i][j];
            }
        }
        return temp;
    }
    Matrix operator*(const Matrix& that) const //matrix multiplication
    {
        if (this->dim != that.dim)
        {
            throw "wrong dimention";
        }
        int n1 = this->dim;
        Matrix temp(n1);
        for (int i = 0; i < n1; i++)
        {
            for (int j = 0; j < n1; j++)
            {
                temp.data[i][j] = 0;
                for (int k = 0; k < n1; k++)
                {
                    temp.data[i][j] += this->data[i][k] * that.data[k][j];
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
        for (int i = 0; i < dim; i++)
        {
            for (int j = 0; j < dim; j++)
            {
                if (this->data[i][j] != that.data[i][j])
                {
                    return false;
                }
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
        for(int i = 0; i < dim; i++)
        {
            for (int j = i+1; j < dim; j++)
            {
                swap(res.data[i][j], res.data[j][i]);
            }
        }
        return res;
    }
    Matrix operator()(int raw, int column) const //minor, obtained by deleting the specifier raw and column
    {
        if (raw > dim)
        {
            throw "raw is  greater than dim";
        }
        if (column > dim)
        {
            throw "column is  greater than dim";
        }
        Matrix res(this -> dim-1);
        for (int i = 0; i < raw-1; i++)
        {
            for (int j = 0; i < column-1; j++)
            {
                res.data[i][j] = this->data[i][j];
            }
        }
        for (int i = raw; i < dim-1; i++)
        {
            for (int j = 0; i < column - 1; j++)
            {
                res.data[i][j] = this->data[i+1][j];
            }
        }
        for (int i = 0; i < raw - 1; i++)
        {
            for (int j = column; i < dim - 1; j++)
            {
                res.data[i][j] = this->data[i][j+1];
            }
        }
        for (int i = raw; i < dim - 1; i++)
        {
            for (int j = column; i < dim - 1; j++)
            {
                res.data[i][j] = this->data[i + 1][j+1];
            }
        }
        return res;
    }
    void print() //output to console
    {
        for (int i = 0; i < dim; i++)
        {
            cout << endl;
            for (int j = 0; j < dim; j++)
            {
                cout << this->data[i][j] << " ";

            }
        }
    }
    void print(ofstream& output)//output to file
    {
        for (int i = 0; i < dim; i++)
        {
            output << endl;
            for (int j = 0; j < dim; j++)
            {
                output << this->data[i][j] << " ";

            }
        }
    }
    raw operator[](unsigned int index)//line access
    {
        if (index > dim)
        {
            throw "raw is  greater than dim";
        }


        return { this->dim, data, index -1};
    }

    column operator()(const size_t index)//column access
    {
        if (index > dim)
        {
            throw "column is  greater than dim";
        }

        return column(this->dim, data, index-1);
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
    int dim, k;
    input >> dim; //reading data
    input >> k;
    Matrix A(dim, input), B(dim, input), C(dim, input), D(dim, input);
    input.close();
    int* arr = new int [dim];  //create matrix K
    for (int i = 0; i < dim; i++)
    {
        arr[i] = k;
    }
    Matrix K(dim, arr);
    delete[] arr;
    //A.print(); B.print(); C.print(); D.print(); K.print();
    //((A + (B * (!C)) + K) * (!D)).print(output);
    //A += B;
    //A.print();
    output.close();

    A[2][2] += 5;
    A.print();
    A(2)[2] -= 9;
    A.print();
    return 0;
}
#include <cassert>
#include <iostream>
#include <fstream>

using namespace std;

class Row {
    size_t row_id;
    size_t matrix_size;
    int * matrix_data;
public:
    Row(size_t idx, size_t size, int * data)
            : row_id(idx)
            , matrix_size(size)
            , matrix_data(data){
    }

    int & operator[](size_t col_id){
        col_id--;
        if (col_id > matrix_size || col_id <0){
            throw std::invalid_argument("Invalid column index");
        }
        return matrix_data[row_id*matrix_size+col_id];
    }
};

class Column{
    size_t col_id;
    size_t matrix_size;
    int * matrix_data;
public:
    Column(size_t idx, size_t size, int * data)
            : col_id(idx)
            , matrix_size(size)
            , matrix_data(data){
    }

    int & operator[](size_t row_id){
        row_id--;
        if (row_id > matrix_size || row_id <0){
            throw std::invalid_argument("Invalid row index");
        }
        return matrix_data[row_id*matrix_size+col_id];
    }
};

class Matrix{
    const size_t matrix_size;
    int * const data;
public:
    Matrix()
            : matrix_size(0)
            , data(nullptr)
    {
    }

    Matrix(size_t size)
            : matrix_size(size)
            , data(new int[matrix_size*matrix_size]())
    {
        for (size_t i = 0; i<matrix_size*matrix_size; i+=(matrix_size+1)){
            data[i]=1;
        }
    }

    Matrix (size_t size, const int * arr)
            : matrix_size(size)
            , data(new int[matrix_size*matrix_size]())
    {
        for (size_t i = 0, j = 0; i<matrix_size*matrix_size; i+=(matrix_size+1), j++ ){
            data[i]=arr[j];
        }
    }

    Matrix (size_t size, ifstream& stream)
            :matrix_size(size)
            , data(new int[matrix_size*matrix_size]())
    {
        for (int i = 0; i< matrix_size*matrix_size; i++){
            stream >> data[i];
        }
    }


    ~Matrix(){
        delete[] data;
    }

    Matrix operator+(const Matrix& other){
        if (matrix_size != other.matrix_size){
            throw std::invalid_argument("Invalid size of matrix ");
        }
        Matrix result_matrix(matrix_size);
        for (size_t i =0; i<matrix_size*matrix_size; i++){
            result_matrix.data[i]=data[i]+other.data[i];
        }
        return result_matrix;
    }

    Matrix operator-(const Matrix& other){
        if (matrix_size != other.matrix_size){
            throw std::invalid_argument("Invalid size of matrix ");
        }
        Matrix result_matrix (matrix_size);
        for (size_t i =0; i<matrix_size*matrix_size; i++){
            result_matrix.data[i]=data[i]-other.data[i];
        }
        return result_matrix;
    }

    Matrix operator*(const Matrix& other){
        if (matrix_size != other.matrix_size){
            throw std::invalid_argument("Invalid size of matrix ");
        }
        Matrix result_matrix(matrix_size);
        for (size_t row_id = 0; row_id< matrix_size; row_id++){
            for (size_t col_id = 0; col_id< matrix_size; col_id++){
                result_matrix.data[row_id*matrix_size+col_id] = 0;
                for (size_t k = 0; k< matrix_size; k++){
                    result_matrix.data[row_id*matrix_size+col_id] += data[row_id*matrix_size+k]*other.data[k*matrix_size+col_id];
                }
            }
        }
        return result_matrix;
    }

    bool operator==(const Matrix& other){
        if (matrix_size != other.matrix_size){
            throw std::invalid_argument("Invalid size of matrix ");
        }
        for (size_t i =0; i<matrix_size*matrix_size; i++){
            if (data[i]!=other.data[i]){
                return false;
            }
        }
        return true;
    }

    bool operator!=(const Matrix& other){
        return !(*this == other);
    }

    Matrix operator() (size_t row, size_t col){
        row--;
        col--;
        if (col > matrix_size || row > matrix_size || col <0 || row < 0){
            throw std::invalid_argument("Invalid column and row indexes");
        }
        if (matrix_size <1){
            throw std::invalid_argument("Invalid matrix size");
        }
        Matrix result_matrix(matrix_size-1);
        for (int i = 0; i < matrix_size; i++){
            if (i == row) continue;
            for (int j = 0; j< matrix_size; j++){
                if (j == col) continue;
                size_t row_shift = 0;
                size_t col_shift = 0;
                if (i > row) row_shift=-1;
                if (j > col) col_shift=-1;
                result_matrix.data[(i+row_shift)*(matrix_size-1)+(j+col_shift)] = data[i*matrix_size+j];
            }
        }
        return result_matrix;
    }

    Column operator()(size_t col_id){
        col_id--;
        if (col_id > matrix_size || col_id <0 ){
            throw std::invalid_argument("Invalid column index");
        }
        return {col_id, matrix_size, data};
    }

    Row operator[](size_t row_id){
        row_id--;
        if (row_id > matrix_size || row_id <0 ){
            throw std::invalid_argument("Invalid row index");
        }
        return {row_id, matrix_size, data};
    }


    Matrix transposed (){
        Matrix result_matrix(matrix_size);
        for (size_t i = 0; i<matrix_size; i++){
            for (size_t j = 0; j< matrix_size; j++){
                result_matrix.data[i*matrix_size+j] = data[j*matrix_size+i];
            }
        }
        return result_matrix;
    }

    void Print(ofstream& stream) const {
        for (size_t i = 0; i<matrix_size; i++){
            for (size_t j = 0; j<matrix_size; j++){
                stream << data[i*matrix_size+j] << " ";
            }
            stream << "\n";
        }
    }


};

int main(){
    ifstream input("input.txt");
    ofstream output("output.txt");
    int n, k;
    input >> n >> k;

    int * k_arr = new int[n];
    for (int i = 0; i<n;i++){
        k_arr[i]=k;
    }

    Matrix A(n, input);
    Matrix B(n, input);
    Matrix C(n, input);
    Matrix D(n, input);
    Matrix K(n, k_arr);

    ((A+(B*C.transposed()) + K) * D.transposed()).Print(output);
//    A[1][2]=5;
//    B(1)[2]=9;
//    A.Print(output);
//    B.Print(output);
    input.close();
    output.close();
    return 0;
}





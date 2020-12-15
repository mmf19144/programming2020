#include <iostream>
#include <fstream>

class RowOfMatrix {
protected:
    int* arr;
    int dimension;
public:
    int getDimension() const {
        return this->dimension;
    }
    RowOfMatrix() {}
    RowOfMatrix(int size, int* array) {
        dimension = size;
        arr = array;
    }
    ~RowOfMatrix() {
        delete[] arr;
    }
    int& operator[](int number) {
        number--;
        if (number < 0 || number >= dimension)
            throw std::string("Wrong number");

        return this->arr[number];
    }
};

class ColumnOfMatrix : public RowOfMatrix {
public:
    ColumnOfMatrix() {}
    ColumnOfMatrix(int size, int* array) {
        dimension = size;
        arr = array;
    }
};

class matrix {
private:
    int dimension;
    int* memory;
public:
    int getDimension() const {
        return this->dimension;
    }
    int getElement(int n) const {
        if (n >= 0 && n < dimension * dimension)
            return memory[n];
    }
    matrix() {
        this->dimension = 0;
        this->memory = nullptr;
    }
    matrix(int n) {
        if (n >= 0) {
            this->dimension = n;
            this->memory = new int[n * n]{ 0 };

            for (int i = 0; i < n; i++) {
                this->memory[i + i * n] = 1;
            }
        }
    }
    matrix(int n, int array[]) {
        if (n >= 0) {
            this->dimension = n;
            this->memory = new int[n * n]{ 0 };

            for (int i = 0; i < n; i++) {
                this->memory[i + i * n] = array[i];
            }
        }
    }
    ~matrix() {
        delete[] memory;
    }
    matrix(const matrix& c1) {
        this->dimension = c1.dimension;
        int n = this->dimension;
        int k = n * n;

        this->memory = new int[n * n];
        for (int i = 0; i < k; i++) {
            this->memory[i] = c1.memory[i];
        }
    }
    matrix operator()(int a, int b) {
        int size = this->getDimension();
        if ((size >= a) && (size >= b)) {
            matrix minorOfMatrix(size - 1);

            int i_m = 0;
            int j_m = 0;
            for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                    if (!((i == a - 1) || (j == b - 1))) {
                        minorOfMatrix.memory[i_m + minorOfMatrix.dimension * j_m] = this->memory[i + this->dimension * j];

                        if (j != b - 1)
                            j_m++;
                    }
                }
                j_m = 0;
                if (i != a - 1)
                    i_m++;
            }
            return minorOfMatrix;
        }
        else return 0;
    }
    RowOfMatrix& operator[](int n) {
        n--;
        int size = this->getDimension();
        if (n < 0 || n >= size) throw "Не существует строки с таким номером";

        int** arrayRow = new int* [dimension];
        for (int i = 0; i < dimension; i++)
            arrayRow[i] = &memory[dimension * n + i];

        ColumnOfMatrix* c = new ColumnOfMatrix(dimension, *arrayRow);
        return *c;
    }
    ColumnOfMatrix& operator()(int n) {
        n--;
        int size = this->getDimension();
        if (n < 0 || n >= size) throw "Не существует строки с таким номером";

        int** arrayCol = new int* [dimension];
        for (int i = 0; i < dimension; i++)
            arrayCol[i] = &memory[dimension * i + n];

        ColumnOfMatrix* c = new ColumnOfMatrix(dimension, *arrayCol);
        return *c;
    }
    matrix operator=(matrix c) {
        this->dimension = c.dimension;
        int n = c.dimension, k = n * n;

        for (int i = 0; i < k; i++) {
            this->memory[i] = c.memory[i];
        }
        return *this;
    }
    friend std::istream& operator>>(std::istream& in, matrix& c1);
    friend std::ostream& operator<<(std::ostream& out, matrix& c1);
    friend matrix operator+(matrix c1, matrix c2);
    friend matrix operator-(matrix c1, matrix c2);
    friend bool operator==(matrix c1, matrix c2);
    friend matrix operator*(matrix c1, matrix c2);
    friend matrix operator+(matrix c1);
};

std::ostream& operator<< (std::ostream& out, matrix& c1) {
    int size = c1.getDimension();
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++)
            out << c1.memory[i * c1.getDimension() + j] << " ";
        out << std::endl;
    }

    return out;
}

std::istream& operator>>(std::istream& in, matrix& c1) {
    int size = c1.getDimension();
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++)
            in >> c1.memory[i * c1.getDimension() + j];
    }

    return in;
}

matrix operator+(matrix c1, matrix c2) {
    if (c1.getDimension() == c2.getDimension()) {
        int size = c1.getDimension();
        int* a = new int[size] { 0 };
        matrix result(size, a);
        delete a[];

        for (int i = 0; i < size * size; i++) {
            result.memory[i] = c1.memory[i] + c2.memory[i];
        }
        return result;
    }
    throw std::string("Сложение матриц разного размера!");
}

matrix operator-(matrix c1, matrix c2) {
    if (c1.getDimension() == c2.getDimension()) {
        int size = c1.getDimension();
        int* a = new int[size] { 0 };
        matrix result(size, a);
        delete a[];

        for (int i = 0; i < size * size; i++) {
            result.memory[i] = c1.memory[i] - c2.memory[i];
        }
        return result;
    }
    throw std::string("Вычитание матриц разного размера!");
}

matrix operator*(matrix c1, matrix c2) {
    if (c1.getDimension() == c2.getDimension()) {
        int size = c1.getDimension();
        matrix result(size, new int[size] { 0 });

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                for (int k = 0; k < size; k++) {
                    result.memory[j + size * i] += c1.memory[k + size * i] * c2.memory[j + size * k];
                }
            }
        }
        return result;
    }
    throw std::string("Умножение матриц разного размера!");
}

bool operator==(matrix c1, matrix c2) {
    if (c1.getDimension() == c2.getDimension()) {
        int size = c1.getDimension();

        for (int i = 0; i < size * size; i++)
            if (c1.memory[i] != c2.memory[i])
                return false;

        return true;
    }
    throw std::string("Сравнение матриц разного размера!");
}

bool operator!=(matrix c1, matrix c2) {
    bool flag = (c1 == c2);
    return !flag;
}

matrix operator+(matrix c1) {
    int size = c1.getDimension();
    if (size == 0)
        throw std::string("Транспонирование матрицы нулевого размера!");

    int* a = new int[size] { 0 };
    matrix trans(size, a);
    delete a[];

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++)
            trans.memory[i * size + j] = c1.memory[i + j * size];
    }

    return trans;
}

int main() {
    std::ifstream fin("input.txt");
    std::ofstream fout("output.txt");

    int N;
    int k;

    fin >> N >> k;

    int* diag = new int[N] {k};
    for (int i = 0; i < N; i++)
        diag[i] = k;

    matrix A(N);
    matrix B(N);
    matrix C(N);
    matrix D(N);
    matrix K(N, diag);
    delete diag[];

    fin >> A >> B >> C >> D;

    matrix example_result = ((A + B * (+C) + K) * (+D));
    fout << example_result;
    return 0;
}
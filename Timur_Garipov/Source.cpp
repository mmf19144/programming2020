#include <iostream>
#include <fstream>

class row_m {
private:
    int dimension;
public:
    int** arr;

    int getDimension() {
        return this->dimension;
    }

    row_m() {}
    ~row_m() {
        delete[] arr;
    }
    row_m(row_m& c) {
        this->dimension = c.dimension;
        int n = c.dimension;
        this->arr = new int* [n];

        for (int i = 0; i < n; i++)
            this->arr[i] = c.arr[i];
    }
    row_m(const row_m& c) {
        this->dimension = c.dimension;
        int n = c.dimension;
        this->arr = new int* [n];

        for (int i = 0; i < n; i++)
            this->arr[i] = c.arr[i];
    }
    void buildCol(int dim, int* memory, int number) {
        if (dim > 0) {
            this->dimension = dim;
            this->arr = new int* [dim];

            for (int i = 0; i < dim; i++) {
                this->arr[i] = &memory[number + dim * i];
            }
        }
    }
    void buildRow(int dim, int* memory, int number) {
        if (dim > 0) {
            this->dimension = dim;
            this->arr = new int* [dim];

            for (int i = 0; i < dim; i++) {
                this->arr[i] = &memory[number * dim + i];
            }
        }
    }

    int operator[](int number) {
        number--;
        if (number < 0 || number >= dimension)
            throw std::string("Нет элемента с таким номером");

        return *this->arr[number];
    }

    friend std::istream& operator>>(std::istream& in, row_m& c1);
    friend std::ostream& operator<<(std::ostream& out, row_m& c1);
};

std::ostream& operator<<(std::ostream& out, row_m& row1) {
    int size = row1.getDimension();
    for (int i = 0; i < size; i++) {
        out << row1.arr[i] << "\t";
    }

    out << std::endl;
    return out;
}

std::istream& operator>>(std::istream& in, row_m& row1) {
    int size = row1.getDimension();
    for (int i = 0; i < size; i++) {
        in >> *row1.arr[i];
    }

    return in;
}

class matrix {
private:
    int dimension;
    int* memory;
public:
    row_m* row,
        * column;

    int getDimension() {
        return this->dimension;
    }
    matrix() {
        this->dimension = 0;
        this->column = nullptr;
        this->row = nullptr;
        this->memory = nullptr;
    }
    matrix(int n) {
        if (n >= 0) {
            this->dimension = n;
            this->memory = new int[n * n]{ 0 };

            this->column = new row_m[n];
            this->row = new row_m[n];

            for (int i = 0; i < n; i++) {
                this->memory[i + i * n] = 1;
            }

            for (int i = 0; i < n; i++) {
                this->column[i].buildCol(n, this->memory, i);
                this->row[i].buildRow(n, this->memory, i);
            }
        }
    }
    matrix(int n, int array[]) {
        if (n >= 0) {
            this->dimension = n;
            this->memory = new int[n * n]{ 0 };

            this->column = new row_m[n];
            this->row = new row_m[n];

            for (int i = 0; i < n; i++) {
                this->memory[i + i * n] = array[i];
            }

            for (int i = 0; i < n; i++) {
                this->column[i].buildCol(n, this->memory, i);
                this->row[i].buildRow(n, this->memory, i);
            }
        }
    }
    ~matrix() {
        delete[] column;
        delete[] row;
        delete[] memory;
    }
    matrix(matrix& c1) {
        this->dimension = c1.dimension;
        int n = this->dimension;
        int k = n * n;

        //this->memory = c1.memory;
        //this->column = c1.column;
        //this->row = c1.row;

        this->memory = new int[n * n];
        for (int i = 0; i < k; i++) {
            this->memory[i] = c1.memory[i];
        }

        this->column = new row_m[n];
        this->row = new row_m[n];

        for (int i = 0; i < n; i++) {
            this->column[i].buildCol(n, this->memory, i);
            this->row[i].buildRow(n, this->memory, i);
        }
    }
    matrix(const matrix& c1) {
        this->dimension = c1.dimension;
        int n = this->dimension;
        int k = n * n;

        //this->memory = c1.memory;
        //this->column = c1.column;
        //this->row = c1.row;

        this->memory = new int[n * n];
        for (int i = 0; i < k; i++) {
            this->memory[i] = c1.memory[i];
        }

        this->column = new row_m[n];
        this->row = new row_m[n];

        for (int i = 0; i < n; i++) {
            this->column[i].buildCol(n, this->memory, i);
            this->row[i].buildRow(n, this->memory, i);
        }
    }
    matrix operator()(int a, int b) {
        int size = this->getDimension();
        if ((size >= a) && (size >= b)) {
            matrix minor_m(size - 1);

            int i_m = 0;
            int j_m = 0;
            for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                    if (!((i == a - 1) || (j == b - 1))) {
                        *minor_m.row[i_m].arr[j_m] = *this->row[i].arr[j];
                        if (j != b - 1)
                            j_m++;
                    }
                }
                j_m = 0;
                if (i != a - 1)
                    i_m++;
            }
            return minor_m;
        }
        else return NULL;
    }
    friend std::istream& operator>>(std::istream& in, matrix& c1);
    friend std::ostream& operator<<(std::ostream& out, matrix& c1);
    row_m operator[](int n) {
        n--;
        int size = this->getDimension();
        if (n < 0 || n >= size) throw "Не существует строки с таким номером";

        return this->row[n];
    }

    row_m operator()(int n) {
        n--;
        int size = this->getDimension();
        if (n < 0 || n >= size)
            throw "Не существует столбца с таким номером";

        return this->column[n];
    }
    matrix operator=(matrix c) {
        this->dimension = c.dimension;
        int n = c.dimension, k = n * n;

        for (int i = 0; i < k; i++) {
            this->memory[i] = c.memory[i];
        }
        return *this;
    }
};

std::ostream& operator<< (std::ostream& out, matrix& c1) {
    int size = c1.getDimension();
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++)
            out << *c1.row[i].arr[j] << " ";
        out << std::endl;
    }

    return out;
}

std::istream& operator>>(std::istream& in, matrix& c1) {
    int size = c1.getDimension();
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++)
            in >> *c1.row[i].arr[j];
    }

    return in;
}

matrix operator+(matrix c1, matrix c2) {
    if (c1.getDimension() == c2.getDimension()) {
        int size = c1.getDimension();
        int* a = new int[size] { 0 };
        matrix result(size, a);
        delete a;

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                *result.column[i].arr[j] = *c1.column[i].arr[j] + *c2.column[i].arr[j];
            }
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
        delete a;

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                *result.column[i].arr[j] = *c1.column[i].arr[j] - *c2.column[i].arr[j];
            }
        }
        return result;
    }
    throw std::string("Вычитание матриц разного размера!");
}

matrix operator*(matrix c1, matrix c2) {
    if (c1.getDimension() == c2.getDimension()) {
        int size = c1.getDimension();
        int* a = new int[size] { 0 };
        matrix result(size, a);
        delete a;

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                for (int k = 0; k < size; k++) {
                    *result.row[i].arr[j] += *c1.row[i].arr[k] * *c2.column[j].arr[k];
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

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (*c1.column[i].arr[j] != *c2.column[i].arr[j])
                    return false;
            }
        }

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
    delete a;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++)
            *trans.row[i].arr[j] = *c1.column[i].arr[j];
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
    delete diag;

    fin >> A >> B >> C >> D;

    matrix example_result = ((A + B * (+C) + K) * (+D));

    fout << example_result;
    return 0;

}
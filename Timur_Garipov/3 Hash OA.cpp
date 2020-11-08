#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>

class RowOfMatrix {
protected:
    int* arr;
    int dimension;
public:
    int getDimension() {
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
    int operator[](int number) {
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
    ~ColumnOfMatrix() {
        delete[] arr;
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
    matrix(matrix& c1) {
        this->dimension = c1.dimension;
        int n = this->dimension;
        int k = n * n;


        this->memory = new int[n * n];
        for (int i = 0; i < k; i++) {
            this->memory[i] = c1.memory[i];
        }
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
        else return NULL;
    }
    RowOfMatrix operator[](int n) {
        n--;
        int size = this->getDimension();
        if (n < 0 || n >= size) throw "Не существует строки с таким номером";

        int* arrayRow = new int[dimension];
        for (int i = 0; i < dimension; i++)
            arrayRow[i] = memory[dimension * n + i];

        RowOfMatrix C(dimension, arrayRow);
        return C;
    }
    ColumnOfMatrix operator()(int n) {
        n--;
        int size = this->getDimension();
        if (n < 0 || n >= size) throw "Не существует строки с таким номером";

        int* arrayCol = new int[dimension];
        for (int i = 0; i < dimension; i++)
            arrayCol[i] = memory[dimension * i + n];

        ColumnOfMatrix C(dimension, arrayCol);
        return C;
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
        delete a;

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
        delete a;

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
        int* a = new int[size] { 0 };
        matrix result(size, a);
        delete a;

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
    delete a;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++)
            trans.memory[i * size + j] = c1.memory[i + j * size];
    }

    return trans;
}

template<> struct std::hash<matrix> {
    size_t operator()(const matrix& value) {
        size_t hash = 0;
        for (int i = 0; i < pow(value.getDimension(), 2); i++) {
            hash += pow(std::hash<int>()(value.getDimension()), 2);
        }
        return hash;
    }
};

template <typename K, typename V>
class Cell {
private:
    bool statFill;
    K key;
    V value;
public:
    Cell() {
        statFill = false;
    }
    bool getStatFill() {
        return statFill;
    }
    K getKey() {
        return key;
    }
    V getValue() {
        return value;
    }
    void setKey(const K& s) {
        key = s;
        statFill = true;
    }
    void setValue(const V& s) {
        value = s;
        statFill = true;
    }
    void delElement() {
        statFill = 0;
    }
};

template <typename K, typename V>
class HashMap {
protected:
    size_t countPairs;
    size_t size;
    Cell<K, V>* massive;
    V iterOfFind(size_t hash_val, const K& find_key) {
        if (hash_val < size) {
            if (massive[hash_val].getStatFill()) {
                if (find_key == massive[hash_val].getKey())
                    return massive[hash_val].getValue();
                else {
                    // В случае коллизии
                    hash_val++;
                    iterOfFind(hash_val, find_key);
                }
            }
            else
                return nullptr;
        }
        else
            return nullptr;
    }
    void realloc(const size_t &new_size) {
        // Выделили память
        Cell<K, V>* new_massive = new Cell<K, V>[new_size];

        // Переписываем значения
        for (size_t i = 0; i < size; i++)
            new_massive[i] = massive[i];

        // Очистим старую память
        delete[] massive;

        // Укажем на новую память и размер
        massive = new_massive;
        size = new_size;
    }
    void iterOfAdd(size_t hash_value, const K& addKey, const V& addValue) {
        if (hash_value >= size) {
            realloc(hash_value + 1);
        }

        if (massive[hash_value].getStatFill()) {
            // Коллизия
            if(massive[hash_value].getKey() != addKey)
                iterOfAdd(hash_value + 1, addKey, addValue);
            else
                massive[hash_value].setValue(addValue);
        }
        else {
            massive[hash_value].setKey(addKey);
            massive[hash_value].setValue(addValue);
            countPairs++;
        }
    }
    void iterOfDel(size_t hash_val, const K& delKey) {
        if (hash_val >= size) {
            return;
        }

        if (massive[hash_val].getStatFill()) {
            if (massive[hash_val].getKey() == delKey) {
                massive[hash_val].delElement();
                countPairs--;
                return;
            }
            else {
                iterOfDel(hash_val + 1, delKey);
            }
        }
    }
public:
    class Iterator {
    private:
        Cell<K, V>* point;
    public:
        Iterator(Cell<K, V>* point1) {
            point = point1;
        }

        Cell<K, V>& operator++() {
            for (auto c = this->point; c != end(); *c = *(c + 1)) {
                if (*c.getStatFill())
                    return c;
            }
        }
        Cell<K, V>& operator--() {
            for (auto c = this->point; c != begin(); *c = *(c - 1)) {
                if (*c.getStatFill())
                    return c;
            }
        }

        bool operator!=(const Iterator It) { return point != It.point; }
        bool operator==(const Iterator It) { return point == It.point; }
        Cell<K, V>& operator*() { return *point; }
    };
    Iterator begin() {
        for (int i = 0; i < size; i++) {
            if (massive[i].getStatFill() == true)
                return Iterator(&massive[i]);
        }
    }
    Iterator end() {
        for (int i = size - 1; i >= 0; i--) {
            if (massive[i].getStatFill() == true)
                return Iterator(&massive[i]);
        }
    }
    HashMap() {
        size = 1000000;
        massive = new Cell<K, V>[size];
        countPairs = 0;
    };
    ~HashMap() {
        delete[] massive;
    }
    V findElement(const K& find_key) {
        size_t hash_val = std::hash<K>()(find_key) % size;
        return iterOfFind(hash_val, find_key);
    };
    void addElement(const K &addKey, const V &addValue) {
        size_t hash_value = std::hash<K>()(addKey) % size;
        iterOfAdd(hash_value, addKey, addValue);
    };
    void delElement(const K &delKey) {
        size_t hash_val = std::hash<K>()(delKey) % size;
        iterOfDel(hash_val, delKey);
    };
    size_t* getMainInfo() {
        size_t* massInfo = new size_t[2];
        massInfo[0] = countPairs;
        size_t uniqueCount = 0;

        std::map<V, bool> mapOfValues;
        for (size_t i = 0; i < size && uniqueCount < countPairs; i++) {
            if (massive[i].getStatFill()) {
                if (mapOfValues[massive[i].getValue()] == false) {
                    mapOfValues[massive[i].getValue()] = true;
                    uniqueCount++;
                }
            }
        }
        mapOfValues.clear();

        massInfo[1] = uniqueCount;
        return massInfo;
    }
};

template <typename K, typename V>
class MultiHashMap : public HashMap<K, V> {
private:
    void iterOfDel(size_t hash_val, const K& delKey) {
        if (hash_val >= this->size) {
            return;
        }

        if (this->massive[hash_val].getStatFill()) {
            if (this->massive[hash_val].getKey() == delKey) {
                this->massive[hash_val].delElement();
                this->countPairs--;
            }
            iterOfDel(hash_val + 1, delKey);
        }
    }
    std::vector<Cell<K, V>> getAllElementsByKey(const K& findKey) {
        size_t hash_val = std::hash<K>()(findKey) % this->size;
        std::vector<Cell<K, V>> support;

        if (hash_val < this->size) {
            for (; hash_val != this->size;) {
                if (this->massive[hash_val].getStatFill() == true) {
                    if(this->massive[hash_val].getKey() == findKey)
                        support.push_back(this->massive[hash_val]);
                }
                hash_val++;
            }
        }
        else
            return nullptr;

        return support;
    }
    size_t getCountByKey(const K& findKey) {
        size_t hash_val = std::hash<K>()(findKey) % this->size;
        size_t count = 0;

        if (hash_val < this->size) {
            for (; hash_val != this->size;) {
                if (this->massive[hash_val].getStatFill() == true) {
                    if (this->massive[hash_val].getKey() == findKey)
                        count++;
                }
                hash_val++;
            }
        }
        else
            return count;

        return count;
    }
};

template <typename K, typename V>
void startProgram(std::ifstream &fin, const std::string &out) {
    std::ofstream fout(out);
    HashMap<K, V> Table;
    size_t countLoop;
    fin >> countLoop;

    K key;
    V value;
    char operation;

    for (size_t i = 0; i < countLoop; i++) {
        fin >> operation;

        if (operation == 'A') {
            fin >> key >> value;
            Table.addElement(key, value);
            continue;
        }
        if (operation == 'R') {
            fin >> key;
            Table.delElement(key);
            continue;
        }
    }

    auto s = HashMap<K, V>::Iterator(Table.begin());

    size_t* arrInfo;
    arrInfo = Table.getMainInfo();
    fout << arrInfo[0] << " " << arrInfo[1];
    delete[] arrInfo;

    fout.close();
    fin.close();
}

int main() {

    std::ifstream fin("input.txt");
    std::string out = "output.txt";

    char typeKey, typeValue;
    fin >> typeKey >> typeValue;

    if (typeKey == 'I' && typeValue == 'S') {
        startProgram<int, std::string>(fin, out);
    }
    else if (typeKey == 'I' && typeValue == 'D') {
        startProgram<int, double>(fin, out);
    }
    else if (typeKey == 'D' && typeValue == 'S') {
        startProgram<double, std::string>(fin, out);
    }
    else if (typeKey == 'D' && typeValue == 'I') {
        startProgram<double, int>(fin, out);
    }
    else if (typeKey == 'S' && typeValue == 'I') {
        startProgram<std::string, int>(fin, out);
    }
    else if (typeKey == 'S' && typeValue == 'D') {
        startProgram<std::string, double>(fin, out);
    }
    else if (typeKey == 'I' && typeValue == 'I') {
        startProgram<int, int>(fin, out);
    }
    else if (typeKey == 'D' && typeValue == 'D') {
        startProgram<double, double>(fin, out);
    }
    else if (typeKey == 'S' && typeValue == 'S') {
        startProgram<std::string, std::string>(fin, out);
    }
    return 0;
}
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <math.h>
#include <iostream>
#include <fstream>

template <typename K, typename V>
class Cell {
private:
    bool wasFilling;
    bool statFill;
    K key;
    V value;
public:
    Cell() {
        wasFilling = false;
        statFill = false;
    }
    bool getStatFill() {
        return statFill;
    }
    bool wasItFilled() {
        return wasFilling;
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
        wasFilling = true;
    }
    void setValue(const V& s) {
        value = s;
        statFill = true;
        wasFilling = true;
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
    Cell<K, V>* massive, * old_massive;
    V iterOfFind(size_t hash_val, const K& find_key) {
        if (hash_val < size) {
            if (massive[hash_val].getStatFill()) {
                if (find_key == massive[hash_val].getKey())
                    return massive[hash_val].getValue();
                else {

                    hash_val++;
                    return iterOfFind(hash_val, find_key);
                }
            }
            else {

                hash_val++;
                if (hash_val != size && massive[hash_val].wasItFilled())
                    return iterOfFind(hash_val, find_key);
                else
                    return NULL;
            }
        }
        else
            return NULL;
    }
    void realloc(const size_t& new_size) {

        Cell<K, V>* new_massive = new Cell<K, V>[new_size];
        old_massive = massive;
        massive = new_massive;


        size_t old_size = size;
        size = new_size;
        for (size_t i = 0; i < old_size; i++)
            if (old_massive[i].getStatFill())
                addElement(old_massive[i].getKey(), old_massive[i].getValue());


        delete[] old_massive;
    }
    void iterOfAdd(size_t hash_value, const K& addKey, const V& addValue) {
        if (hash_value >= size) {
            realloc(hash_value * 2);
            hash_value = getHash(addKey);
        }

        if (massive[hash_value].getStatFill()) {

            if (massive[hash_value].getKey() != addKey)
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
    virtual void iterOfDel(size_t hash_val, const K& delKey) {
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
        else {
            if (hash_val + 1 != size && massive[hash_val + 1].wasItFilled())
                iterOfDel(hash_val + 1, delKey);
        }
    }
    size_t getHash(const K& Key) {
        return std::hash<K>()(Key) % size;
    }
public:
    class Iterator {
    private:
        size_t index;
        size_t size, countPair;
        Cell<K, V>* massive;
    public:
        Iterator(Cell<K, V>* _massive, size_t _size, size_t _countPair, size_t _ind) {
            massive = _massive;
            size = _size;
            index = _ind;
            countPair = _countPair;
        }
        Cell<K, V>& operator++() {
            if (index < size) {
                Cell<K, V>* pointer = &massive[index];
                bool flag = false;
                for (int i = index + 1; i < size && !flag; i++)
                    if (massive[i].statFill) {
                        flag = true;
                        pointer = &massive[i];
                    }

                if (flag)
                    return pointer;
                else
                    return &massive[index];
            }
        }
        Cell<K, V>& operator--() {
            if (index >= 0) {
                Cell<K, V>* pointer = &massive[index];
                bool flag = false;
                for (int i = index - 1; i >= 0 && !flag; i--)
                    if (massive[i].statFill) {
                        flag = true;
                        pointer = &massive[i];
                    }

                if (flag)
                    return pointer;
                else
                    return &massive[index];
            }
        }

        bool operator!=(const Iterator It) { return massive[index] != It.massive[It.index]; }
        bool operator==(const Iterator It) { return massive[index] == It.massive[It.index]; }
        Cell<K, V>& operator*() { return &massive[index]; }
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
        size_t hash_val = getHash(find_key);
        return iterOfFind(hash_val, find_key);
    };
    void addElement(const K& addKey, const V& addValue) {
        size_t hash_value = getHash(addKey);
        iterOfAdd(hash_value, addKey, addValue);
    };
    void delElement(const K& delKey) {
        size_t hash_val = getHash(delKey);
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
void startProgram(std::ifstream& fin, const std::string& out) {
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
        if (operation == 'F') {
            fin >> key;

            V c = Table.findElement(key);
            std::cout << c << std::endl;
            continue;
        }
    }

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
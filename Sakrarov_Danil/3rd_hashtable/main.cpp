#include <iostream>
#include <fstream>
#include <map>
#include <assert.h>
#include <iterator>

#define rehash_value 0.5

template<typename K, typename V>
class Slot {
private:
    K key;
    V value;
    bool Filled;
    bool Deleted = false;
public:
    Slot() : Filled(false) {
    }

    V getValue() {
        assert(Filled && !Deleted);
        return value;
    }

    K getKey() {
        return key;
    }

    bool getFilled() {
        return Filled;
    }

    bool getDeleted() {
        return Deleted;
    }

    void setValue(const V &s) {
        value = s;
        Filled = true;
    }

    void setKey(const K &s) {
        key = s;
        Filled = true;
    }

    void delete_element() {
        Filled = false;
        Deleted = true;
    }


};

template<typename K, typename V>
class HashMap {
private:
    size_t capacity = 0;
    size_t size;
    Slot<K, V> *table;
public:
    HashMap() : size(3), table(new Slot<K, V>[size]) {
    };

    HashMap(size_t new_size) : size(new_size), table(new Slot<K, V>[size]) {

    }

    ~HashMap() {
        delete[] table;
    }

    size_t get_cap() {
        return capacity;
    }

    size_t get_size() {
        return size;
    }


    void addElement(const K &addKey, const V &addValue) {
        size_t hash_value = std::hash<K>()(addKey) % size;
        add_val(hash_value, addKey, addValue);
    };

    void delete_element(const K &delKey) {
        size_t hash_val = std::hash<K>()(delKey) % size;
        del_val(hash_val, delKey);
    };


    class Iterator : public std::iterator<std::input_iterator_tag, Slot<K, V>> {
    private:
        friend class HashMap<K, V>;

        Slot<K, V> *curr_it;
        Slot<K, V> *end_it;
    public:
        Iterator() : curr_it(nullptr), end_it(nullptr) {}

        Iterator(Slot<K, V> *curr_it1, Slot<K, V> *end1) {
            curr_it = curr_it1;
            end_it = end1;
        }

        std::pair<K,V> operator*() {
            return std::make_pair(curr_it->getKey(),curr_it->getValue());
        }

        Iterator &operator++() {
            if (curr_it != nullptr) {
                curr_it++;
                while (curr_it != end_it && !curr_it->getFilled()) {
                    curr_it++;
                }
                if (curr_it != end_it) {
                    return *this;
                }

            }

            curr_it = nullptr;
            return *this;
        }


        bool operator!=(const Iterator &It) { return curr_it != It.curr_it; }

        bool operator==(const Iterator &It) { return curr_it == It.curr_it; }


    };

    Iterator begin() {

        for (int i = 0; i < size; i++) {
            if (table[i].getFilled())
                return Iterator(&table[i], &table[size]);
        }
        return end();
    }

    Iterator end() {
        return Iterator(nullptr, nullptr);
    }


    V &find(const K &getKey) {
        size_t hash_value = std::hash<K>()(getKey) % size;

        size_t iter = hash_value;
        if (table[iter].getFilled() && table[iter].getKey() == getKey) {
            return table[iter].getValue();
        } else {
            iter++;
            while (table[iter].getFilled() && table[iter].getKey() != getKey && iter != hash_value) {
                iter++;
                iter %= size;
            }
            if (!table[iter].getDeleted)
                return table[iter].getValue();
            else
                return nullptr;
        }
    }

    void rehash() {


        auto tmp = HashMap<K, V>(size * 2);
        for (auto it:*this) {
            tmp.addElement(it.first, it.second);
        }

        delete[] table;
        size *= 2;
        table = tmp.table;
        tmp.table = nullptr;
        capacity = tmp.capacity;
    }

    void add_val(const size_t hash_value, const K &addKey, const V &addValue) {
        size_t iter = hash_value;
        if (table[iter].getFilled() && table[iter].getKey() == addKey) {
            table[iter].setValue(addValue);
        } else {
            while (table[iter].getFilled() && table[iter].getKey() != addKey) {
                iter++;
                iter %= size;
            }

            table[iter].setKey(addKey);
            table[iter].setValue(addValue);
            capacity++;
        }
        if ((double) capacity / (double) size >= rehash_value) {
            rehash();
        }

    }


    void del_val(size_t hash_value, const K &delKey) {

        size_t iter = hash_value;

        while (table[iter].getFilled()) {
            if (table[iter].getFilled() && table[iter].getKey() == delKey) {
                table[iter].delete_element();
                capacity--;
            }
            iter++;
            iter %= size;

        }
    }


};


template<typename K, typename V>
void start(std::ifstream &fin, const std::string &out) {
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
            Table.delete_element(key);
            continue;
        }
    }

    HashMap<V, bool> mapOfValues(Table.get_size()); //values as keys for cont unique elements
    for (auto it: Table) {
            mapOfValues.addElement(it.second, 1);
    }

    fout << Table.get_cap() << " " << mapOfValues.get_cap();;


    fout.close();
    fin.close();
}

int main() {

    std::ifstream fin("input.txt");
    std::string out = "output.txt";

    char typeKey, typeValue;
    fin >> typeKey >> typeValue;

    if (typeKey == 'I' && typeValue == 'S') {
        start<int, std::string>(fin, out);
    } else if (typeKey == 'I' && typeValue == 'D') {
        start<int, double>(fin, out);
    } else if (typeKey == 'D' && typeValue == 'S') {
        start<double, std::string>(fin, out);
    } else if (typeKey == 'D' && typeValue == 'I') {
        start<double, int>(fin, out);
    } else if (typeKey == 'S' && typeValue == 'I') {
        start<std::string, int>(fin, out);
    } else if (typeKey == 'S' && typeValue == 'D') {
        start<std::string, double>(fin, out);
    } else if (typeKey == 'I' && typeValue == 'I') {
        start<int, int>(fin, out);
    } else if (typeKey == 'D' && typeValue == 'D') {
        start<double, double>(fin, out);
    } else if (typeKey == 'S' && typeValue == 'S') {
        start<std::string, std::string>(fin, out);
    }
    return 0;
}
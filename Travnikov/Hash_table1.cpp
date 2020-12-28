#include <iostream>
#include <fstream>
#include <cstdio>
#include <stdio.h>
#include <vector>
#include <map>
using namespace std;

template <typename K,typename  N>
class Node{
    K key;
    N value;
    char status = 0;  // 0 - ������, 1 - ������ , -1 - ��� ��������� �������
public:
    Node(){
        //this->key = NULL;
        //this->value = NULL;
        this->status = 0;
    }
    void setkey(K k){
        this->key = k;
        this->status = 1;
    }
    void setval(N v){
        this->value = v;
        this->status = 1;
    }
    void write(K k, N v){
        this->key = k;
        this->value = v;
        this->status = 1;
    }
    K getkey(){
        return this->key;
    }
    N getval(){
        return this->value;
    }
    void delnode(){
        //this->value = NULL;
        //this->key = NULL;
        this->status = -1;
    }

    int getstatus(){
        return this->status;
    }
    void setstatus(char s){
        this->status = s;
    }
};

template <typename K,typename N>
class Hashmap {
protected:
    size_t size;
    size_t cap;
    size_t pc;
    Node<K, N> *arr;

public:
    Hashmap() {
        arr = new Node<K, N>[10];
        this->size = 0;
        this->cap = 10;
        this->pc = 70;
    }

    Hashmap(size_t capacity) {
        arr = new Node<K, N>[capacity];
        this->size = 0;
        this->cap = capacity;
        this->pc = 70;
    }

    Hashmap(size_t capacity, size_t percent) {
        arr = new Node<K, N>[capacity];
        this->size = 0;
        this->cap = capacity;
        if (percent > 100) percent = 100;
        this->pc = percent;
    }

    ~Hashmap(){
        delete[] arr;
    }

    size_t getsize() {
        return this->size;
    }

    size_t getcap() {
        return this->cap;
    }

    size_t get_hash(K key) {
        return (std::hash<K>{}(key) % cap);
    }



    bool is_full() {
        //cout << "Cap: " << this->size << " " << this->cap << " " << (this->size * 100 / this->cap) << endl;
        return (this->size * 100 / this->cap) >= this->pc;
    }

    void extend() {
        size_t newcap = this->cap * 1.5;
        Node<K, N> *newarr = new Node<K, N>[newcap];

        for (int i = 0; i < this->cap; i++) {
            if (arr[i].getstatus() == 1) {
                size_t hash = std::hash<K>{}(arr[i].getkey()) % newcap;
                for (int j = hash; j < newcap + (newcap-hash); j++) {
                    if (newarr[j].getstatus() != 1 && hash < newcap) {
                        newarr[j].write(arr[i].getkey(), arr[i].getval());
                        break;
                    }
                    else if (j >=newcap && newarr[j-newcap].getstatus() != 1){
                        newarr[j-newcap].write(arr[i].getkey(), arr[i].getval());
                        break;
                    }





                }
            }
        }

        this->cap = newcap;
        delete [] arr;
        this->arr = newarr;


        //cout << "Extend: " << newcap << endl;

    }

public:
    bool exists(K key) {
        size_t hash = this->get_hash(key);
        for (int i = hash; i < this->cap; i++) {
            if (arr[i].getstatus() == 1 && arr[i].getkey() == key) {
                return true;
            } else if (arr[i].getstatus() == 0) {
                break;
            }
        }
        return false;
    }

    int find(K key, size_t hash) {

        int i = hash;
        while (arr[i].getstatus() != 0){
            if (i<this->cap) {
                if (arr[i].getstatus() == -1) {
                    i++;
                    continue;
                }
                if (arr[i].getstatus() == 1 && arr[i].getkey() == key)
                    return i;
                if (arr[i].getstatus() == 1) {
                    i++;
                    continue;
                }
            }
            else {
                if (arr[i-this->cap].getstatus() == -1) {
                    i++;
                    continue;
                }
                if (arr[i-this->cap].getstatus() == 1 && arr[i].getkey() == key)
                    return i-this->cap;
                if (arr[i-this->cap].getstatus() == 1) {
                    i++;
                    continue;
                }
            }
        }
        return -1;

    }

    void add(size_t hsh,K key, N value) {

        size_t hash = this->get_hash(key);
        int search = find(key,hash);
        if (search >=0){
            arr[search].write(key,value);
            if (this->is_full()) this->extend();

        }
        else {
            for (int i = hash; i < this->cap * 2; i++) {
                if (i < this->cap) {

                    if (arr[i].getstatus() != 1) {
                        arr[i].write(key, value);
                        this->size++;

                        if (this->is_full()) this->extend();
                        break;

                    }

                }
                if (i >= this->cap) {
                    int k = i - this->cap;
                    if (arr[k].getstatus() != 1) {
                        arr[k].write(key, value);
                        this->size++;

                        if (this->is_full()) this->extend();
                        break;

                    }

                }
            }
        }
    }




    void remove(K key) {
        size_t hash = this->get_hash(key);
        for (int i = hash; i < this->cap; i++) {
            if (arr[i].getstatus() == 1 && arr[i].getkey() == key) {
                arr[i].setstatus(-1);
                this->size--;

                return;
            } else if (arr[i].getstatus() == 0) {
                break;
            }
        }
    }


    int count_unique() {
        int uniqueCount = 0;

        map <N, bool> mapOfValues;
        for (size_t i = 0; i < this->cap && uniqueCount < this->size; i++) {
            if (arr[i].getstatus()) {
                if (mapOfValues[arr[i].getval()] == false) {
                    mapOfValues[arr[i].getval()] = true;
                    uniqueCount++;
                }
            }
        }
        mapOfValues.clear();


        return uniqueCount ;
    }



    void printtable(){
        for(int i =0;i<this->cap;i++){
            cout << arr[i].getstatus() << ' ' << arr[i].getval() << endl;
        }
    }


    class Iterator {
    private:
        int cur;
        Node<K, N> *arr;
        size_t cap;
    public:

        Iterator(Node<K, N> *arr, size_t cap) {
            this->arr = arr;
            this->cap = cap;
            this->cur = -1;
        }

        bool has_next() {
            if (this->cur == this->cap - 1) return false;
            for (int i = this->cur + 1; i < this->cap; i++) {
                if (arr[i].getstatus() == 1) {
                    return true;
                }
            }
            return false;
        }

        K next() {
            if (this->cur == this->cap - 1) return arr[this->cur].getkey();
            for (int i = this->cur + 1; i < this->cap; i++) {
                if (arr[i].getstatus() == 1) {
                    this->cur = i;
                    return arr[i].getkey();
                }
            }
            return arr[this->cur].getkey();
        }
    };

    Iterator get_iterator() {
        Iterator it(this->arr, this->cap);
        return it;
    }

};

template<typename K, typename N>


void runprog(std::ifstream &fin, std::ofstream &fout, int opers) {
    Hashmap<K, N> table;
    char oper;
    K key;
    N val;
    for (int i = 0; i < opers; i++) {

        fin >> oper >> key;

        if (oper == 'A') {
            fin >> val;
            size_t hash = (std::hash<K>{}(key)) % table.getcap();
            table.add(hash,key, val);
            continue;
        }
        if (oper == 'R') {
            table.remove(key);
            continue;
        }
    }
    table.printtable();
    fout << table.getsize() << ' ' << table.count_unique();

}

int main() {
    ifstream fin("input.txt");
    ofstream fout("output.txt");
    char K;
    char V;
    int opnum;
    fin >> K >> V >> opnum;


    if (K == 'I' && V == 'S') {
        runprog<int, std::string>(fin, fout, opnum);
    } else if (K == 'I' && V == 'D') {
        runprog<int, double>(fin, fout, opnum);
    } else if (K == 'D' && V == 'S') {
        runprog<double, std::string>(fin, fout, opnum);
    } else if (K == 'D' && V == 'I') {
        runprog<double, int>(fin, fout, opnum);
    } else if (K == 'S' && V == 'I') {
        runprog<std::string, int>(fin, fout, opnum);
    } else if (K == 'S' && V == 'D') {
        runprog<std::string, double>(fin, fout, opnum);
    } else if (K == 'I' && V == 'I') {
        runprog<int, int>(fin, fout, opnum);
    } else if (K == 'D' && V == 'D') {
        runprog<double, double>(fin, fout, opnum);
    } else if (K == 'S' && V == 'S') {
        runprog<std::string, std::string>(fin, fout, opnum);
    }

    return 0;
}
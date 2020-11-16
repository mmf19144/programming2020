#include <iostream>
#include <fstream>
#include <cstdio>
#include <stdio.h>
#include <vector>

using namespace std;

template <typename K,typename  N>
class Node{
    K key;
    N value;
    char status = 0;
public:
    Node(){

        this->status = 0;
    }
    void writekey(K k){
        this->key = k;
        this->status = 1;
    }
    void writeval(N v){
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
        arr = new Node<K, N>[100];
        this->size = 0;
        this->cap = 100;
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

        return (this->size * 100 / this->cap) >= this->pc;
    }

    void extend() {
        size_t newcap = this->cap * 1.5;
        Node<K, N> *newarr = new Node<K, N>[newcap];

        for (int i = 0; i < this->cap; i++) {
            if (arr[i].getstatus() == 1) {
                size_t hash = std::hash<K>{}(arr[i].getkey()) % newcap;
                for (int j = hash; j < newcap; j++) {
                    if (newarr[j].getstatus() != 1) {
                        newarr[j].write(arr[i].getkey(), arr[i].getval());
                        break;
                    }
                }
            }
        }

        this->cap = newcap;
        this->arr = newarr;




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

    N find(K key) {
        size_t hash = this->get_hash(key);
        for (int i = hash; i < this->cap; i++) {
            if (arr[i].getstatus() == 1 && arr[i].getkey() == key) {
                return arr[i].getval();
            } else if (arr[i].getstatus() == 0) {
                break;
            }
        }
        N nothing;
        return nothing;
    }

    void add(K key, N value) {

        size_t hash = this->get_hash(key);
        for (int i = hash; i < this->cap; i++) {
            if (arr[i].getstatus() != 1 ) {
                arr[i].write(key, value);
                this->size++;

                if (this->is_full()) this->extend();
                break;

            }
            if ( arr[i].getkey() == key) {
                arr[i].write(key, value);

                if (this->is_full()) this->extend();
                break;

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

        int res = 0;


        for (int i = 1; i < this->cap; i++) {
            if (arr[i].getstatus() == 1) {
                int j = 0;
                for (j = 0; j < i; j++){
                    if (arr[i].getval() == arr[j].getval())
                        break;}


                if (i == j)
                    res++;
            }
        }
        return res;
    }
    void printtable(){
        for(int i =0;i<this->cap;i++){
            cout << arr[i].getstatus();
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
            table.add(key, val);
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
};
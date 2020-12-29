#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace std;

template <typename T>
class CVector
{
    T* p_vector;
    size_t size;
    size_t total_size;

public:
    CVector() { size = 0; total_size = 0; p_vector = 0; }
    CVector(size_t s);
    CVector(CVector& obj);
    ~CVector() { if (p_vector) delete[] p_vector; }

    int getsize() { return size; }
    int get_total_size() { return total_size; }
    void push_back(T data);
    void erase(size_t index);
    T& operator[](int i);
    CVector<T>& operator=(const CVector<T>& obj);

};


template <typename T>
void CVector<T> ::push_back(T data) {
    T* p_new = p_vector;
    size = size + 1;
    if (size >= total_size)
    {
        total_size = size * 2;
        p_vector = new T[total_size];

        for (int i = 0; i < size - 1; i++)
            p_vector[i] = p_new[i];
        delete[] p_new;
        p_vector[size - 1] = data;
    }
    else {
        p_vector[size - 1] = data;
    }
}


template <typename T>
void CVector<T> ::erase(size_t index) {
    size = size - 1;
    for (size_t i = index; i < size; i++)
        p_vector[i] = p_vector[i + 1];
}


template <typename T>
CVector<T>::CVector(size_t s)
{
    size = s;
    total_size = s;
    p_vector = new T[s];
    
}

template <typename T>
CVector<T>::CVector(CVector& obj)
{
    size = obj.size;
    p_vector = new T[obj.size];

    for (int i = 0; i < size; i++)
        p_vector[i] = obj.p_vector[i];
    
}


template <typename T>
T& CVector<T>::operator[](int i)
{
    if (i < 0 || i >(size - 1)) {
        cout << "ERROR\n";
        exit(1);
    }

    return p_vector[i];
}

template <typename T>
CVector<T>& CVector<T>::operator=(const CVector<T>& obj)
{

    if (total_size < obj.size) {
        delete[] p_vector;
        p_vector = new T[obj.size];
        size = obj.size;
    }

    for (int i = 0; i < size; i++)
        p_vector[i] = obj.p_vector[i];

    return *this;
}

template<typename T>
void run(std::ifstream& fin, std::ofstream& fout, int opers) {
    CVector<T> vect;
    string oper;
    T data;
    int num;

    for (int i = 0; i < opers; i++) {

        fin >> oper;

        if (oper[0] == 'A') {
            fin >> data;
            vect.push_back(data);
            continue;
        }
        if ((oper[0] == 'R') && (oper[1] == 'E')) {
            fin >> num;
            if (num < vect.getsize())
                vect.erase(num);
            else
            {
                fout << "ERROR" << '\n';
                break;
            }
            continue;
        }
        if (oper[0] == 'P') {
            fin >> num;
            if (num < vect.getsize())
                fout << vect[num] << '\n';
            else
            {
                fout << "ERROR" << '\n';
                break;
            }
            continue;
        }
        if (oper[0] == 'U') {//?
            fin >> num >> data;
            if (num < vect.getsize())
                vect[num] = data;
            else
            {
                fout << "ERROR" << '\n';
                break;
            }
            continue;
        }
        if (oper[0] == 'L') {
            fin >> num;
            num = num % vect.getsize();
            CVector<T> tempvect(vect.getsize());
            for (size_t i = 0; i < vect.getsize(); i++)
            {
                tempvect[i] = vect[(i + num) % vect.getsize()];
            }
            for (size_t i = 0; i < vect.getsize(); i++)
            {
                vect[i] = tempvect[i];
            }
        }


        if ((oper[0] == 'R') && (oper[1] == 'S')) {//
            fin >> num;
            num = num % vect.getsize();
            CVector<T> tempvect(vect.getsize());
            for (size_t i = 0; i < vect.getsize(); i++)
            {
                tempvect[(i + num) % vect.getsize()] = vect[i];
            }
            for (size_t i = 0; i < vect.getsize(); i++)
            {
                vect[i] = tempvect[i];
            }
            continue;
        }
    }


    for (size_t i = 0; i < vect.getsize(); i++)
        fout << vect[i] << '\n';

}


int main() {

    ifstream fin("input.txt");
    ofstream fout("output.txt");
    char T;
    size_t N = 0;
    fin >> T >> N;


    if (T == 'I') {
        run<int>(fin, fout, N);
    }
    else if (T == 'D') {
        run<double>(fin, fout, N);
    }
    else if (T == 'S') {
        run<string>(fin, fout, N);
    }

    return 0;
};


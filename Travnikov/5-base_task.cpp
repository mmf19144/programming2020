#include <iostream>
#include <vector>
#include <algorithm>
#include <mutex>
#include <fstream>
#include <thread>

using namespace std;


mutex mtx;

void fileParser(ifstream &fin, vector<string> &strings) {
    string tmp;
    while (true) {
        mtx.lock();
        fin >> tmp;
        if (tmp.empty()) {
            mtx.unlock();
            return;
        }
        strings.push_back(tmp);
        if (fin.eof()) {
            mtx.unlock();
            return;
        }

        mtx.unlock();
    }
}


void wordCounter(const size_t from, const size_t to, size_t &counter,
                 vector<string> &strings, const string &search_word) {
    for (size_t i = from; i < to; i++) {
        if (strings[i] == search_word) {
            mtx.lock();
            counter++;
            mtx.unlock();
        }
    }
}

int main() {
    ofstream fout("output.txt");
    ifstream fin("input.txt");


    vector<thread> threads;
    size_t threads_cnt;
    string search_word;
    fin >> threads_cnt >> search_word;
    vector<string> strings;
    threads.reserve(threads_cnt);
    for (size_t i = 0; i < threads_cnt; i++) {
        threads.emplace_back(fileParser, ref(fin), ref(strings));
    }
    for (size_t i = 0; i < threads_cnt; i++) {
        threads[i].join();
    }
    threads.clear();
    size_t cnt = 0;
    size_t strings_size = strings.size();
    size_t partition = strings_size / threads_cnt + 1;
    for (size_t i = 0; i < threads_cnt; i++) {
        size_t from = partition * i;
        size_t to = partition * (i + 1);
        if (to > strings_size)
            to = strings_size;
        threads.emplace_back(wordCounter, from, to, ref(cnt), ref(strings), ref(search_word));
    }
    for (size_t i = 0; i < threads_cnt; i++) {
        threads[i].join();
    }
    fout << cnt;
    return 0;
}
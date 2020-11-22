#include <iostream>
#include <fstream>
#include <map>
#include <queue>

std::ofstream fout("output.txt");
std::ifstream fin("input.txt");

class DKA {
    size_t q_start = 0;
    size_t current = 0;
    std::map<size_t, bool> accept_states;
    std::map <size_t, std::map<char, size_t>> Automaton;

public:
    DKA(size_t qo): q_start(qo), current(q_start) {}

    void qaccept(const size_t &f) {
        size_t t;
        for (int i = 0; i < f; i++) {
            fin >> t;
            accept_states.insert(std::pair<size_t, bool>(t, true));
        }

    }

    void DKA_push(const size_t &p)
    {
        for (int i = 0; i < p; i++) {
            size_t from, to;
            char val;
            fin >> from >> to >> val;
            Automaton[from].insert(std::pair <char, size_t>(val, to));
        }
    }

    bool check(const std::string& expression) {
        current = q_start;
        for (auto i : expression) {
            current = Automaton[current][i];
        }
        return accept_states[current];
    }



//    void determinaze(){
//        std::queue<std::vector<size_t>> nka_states;
//        nka_states.push(std::vector<size_t>(q_start));
//        std::vector<size_t> dka_states;
//        while (!nka_states.empty()) {
//            auto qp = nka_states.front();
//            for (auto i: qp){
//                std::vector<size_t> tmp;
//                for (auto j : Automaton[i]){
//
//                }
//            }
//        }
//    }
};

void accepted_words(DKA &A){
    size_t T;
    fin >> T;
    for (size_t i = 0; i < T; i++) {
        std::string h;
        fin >> h;
        if (A.check(h)){
            fout << "YES" << '\n';
        } else
            fout << "NO" << '\n';
    }
}


int main() {

    size_t N, k, f, p;
    fin >> N >> k >> f;

    DKA A(k);
    A.qaccept(f);

    fin >> p;
    A.DKA_push(p);
    accepted_words(A);
    return 0;
}
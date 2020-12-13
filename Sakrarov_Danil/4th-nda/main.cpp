#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <fstream>
#include <queue>

std::set<unsigned> getSetOfOneEl(unsigned el) {
    std::set<unsigned> a;
    a.insert(el);
    return a;
}

class NFA {
private:
    unsigned startState = 0;
    size_t statesCnt = 0;
    std::map<std::set<unsigned>, std::map<char, std::set<unsigned>>> transitions;
    std::vector<unsigned> endStates;

public:
    NFA() = default;

    ~NFA() {
        for (auto it : transitions) {
            it.second.clear();
        }
        transitions.clear();
        endStates.clear();
    }

    void make_nfa(std::ifstream &os) {
        unsigned f = 0, p = 0;
        os >> statesCnt >> startState >> f;
        for (size_t i = 0; i < f; i++) {
            unsigned tmp;
            os >> tmp;
            endStates.push_back((tmp));
        }
        os >> p;
        for (size_t i = 0; i < p; i++) {
            unsigned from, to;
            char chr;
            os >> from >> to >> chr;
            transitions[getSetOfOneEl(from)][chr].insert(to);
        }
    }

    unsigned getStartState() const {
        return startState;
    }

    std::map<std::set<unsigned>, std::map<char, std::set<unsigned>>> *getTransitions() {
        return &transitions;
    }

    std::vector<unsigned> *getEndStates() {
        return &endStates;
    }
};


class DFA {

private:
    unsigned startState = 0;
    std::set<std::set<unsigned> > endStates;
    std::map<std::set<unsigned>, std::map<char, std::set<unsigned>>> transitions;
public:
    DFA() = default;

    DFA(NFA &other);

    bool checkWord(std::string &a) {
        auto cur = getSetOfOneEl(startState);
        for (auto i : a) {
            cur = transitions[cur][i];
        }
        return std::find(endStates.begin(), endStates.end(), cur) != endStates.end();
    }


};

DFA::DFA(NFA &other) {
    std::queue<std::set<unsigned> > q;
    startState = other.getStartState();
    q.push(getSetOfOneEl(startState));

    while (!q.empty()) {
        auto top = q.front();
        q.pop();
        for (auto state: top) {
            for (const auto &ch: other.getTransitions()->operator[](getSetOfOneEl(state))) {
                for (const auto &el: ch.second) {
                    transitions[top][ch.first].insert(el);
                }
                if (transitions.find(transitions[top][ch.first]) == transitions.end()) {
                    q.push(transitions[top][ch.first]);
                }
            }
        }
    }

    for (const auto &key: transitions) {
        for (auto t: *other.getEndStates()) {
            if (std::find(key.first.begin(), key.first.end(), t) != key.first.end()) {
                endStates.insert(key.first);
                break;
            }
        }
    }
    for (auto t: *other.getEndStates()) {
        endStates.insert(getSetOfOneEl(t));
    }
}

void check(std::ifstream &fin, std::ofstream &fout, DFA &b) {
    size_t t = 0;
    fin >> t;

    for (size_t i = 0; i < t; i++) {
        std::string tmp;
        fin >> tmp;
        if (b.checkWord(tmp)) {
            fout << "YES" << std::endl;
        } else
            fout << "NO" << std::endl;
    }
}

int main() {
    std::ofstream fout("output.txt");
    std::ifstream fin("input.txt");

    NFA a;
    a.make_nfa(fin);
    DFA b(a);

    check(fin, fout, b);


    return 0;
}


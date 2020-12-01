#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <fstream>
#include <queue>

//#define DEBUG

#define unsignedSet std::set<unsigned>

unsignedSet getSetOfOneEl(unsigned el) {
    unsignedSet a;
    a.insert(el);
    return a;
}

class NFA {
private:
    std::map<unsignedSet, std::map<char, unsignedSet>> transitions;
    std::vector<unsigned> endStates;
    unsigned startState = 0;
    size_t statesCnt = 0;
public:

    NFA() = default;

    ~NFA() {
        for (auto it : transitions) {
            it.second.clear();
        }
        transitions.clear();
        endStates.clear();
    }

    friend std::istream &operator>>(std::istream &os, NFA &m) {
        unsigned f = 0, p = 0;
        os >> m.statesCnt;
        os >> m.startState;
        os >> f;
        for (size_t i = 0; i < f; i++) {
            unsigned tmp;
            os >> tmp;
            m.endStates.push_back((tmp));
        }
        os >> p;
        for (size_t i = 0; i < p; i++) {
            unsigned from, to;
            char chr;
            os >> from >> to >> chr;
            m.transitions[getSetOfOneEl(from)][chr].insert(to);
//            m.transitions[std::pair<unsignedSet, char>(getSetOfOneEl(from), chr)].push_back(getSetOfOneEl(to));
        }
        return os;
    }

    unsigned getStartState() const {
        return startState;
    }

    std::map<unsignedSet, std::map<char, unsignedSet>> *getTransitions() {
        return &transitions;
    }

    std::vector<unsigned> *getEndStates() {
        return &endStates;
    }
};


class DFA {

private:
    std::map<unsignedSet, std::map<char, unsignedSet>> transitions;
    std::set<unsignedSet > endStates;
    unsigned startState = 0;
public:
    DFA() = default;

    explicit DFA(NFA &other) {
        startState = other.getStartState();
        std::queue<unsignedSet > q;
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
#ifdef DEBUG
        for (auto key: endStates) {
            for (auto i: key) {
                std::cout << i;
            }
            std::cout << std::endl;
        }
#endif
    }

    bool checkWord(std::string &a) {
        auto cur = getSetOfOneEl(startState);
        for (auto i : a) {
//            cur = transitions[std::pair<unsignedSet, char>(cur, i)][0];
            cur = transitions[cur][i];
        }
#ifdef DEBUG
        std::cout << "FIND";
        for (auto i: cur) {
            std::cout << i;
        }
        std::cout << std::endl;
#endif
        return std::find(endStates.begin(), endStates.end(), cur) != endStates.end();
    }

};


int main() {
    std::ofstream fout("output.txt");
    std::ifstream fin("input.txt");

    NFA a;
    fin >> a;
    DFA b(a);

    int t = 0;
    fin >> t;
    for (size_t i = 0; i < t; i++) {
        std::string tmp;
        fin >> tmp;
        if (b.checkWord(tmp)) {
            fout << "YES" << std::endl;
        } else
            fout << "NO" << std::endl;
    }


    return 0;
}

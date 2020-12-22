#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <queue>
#include <unordered_set>
#include <unordered_map>

//#define DEBUG


class NFA {
private:
    std::unordered_map<std::vector<bool>, std::unordered_map<char, std::vector<unsigned >>> transitions;
    std::vector<bool> endStates;
    unsigned startState = 0;
    size_t statesCnt = 0;
public:

    NFA() = default;

    ~NFA() {
        for (auto it : transitions) {
            it.second.clear();
        }
//        transitions.clear();
//        endStates.clear();
    }

    friend std::istream &operator>>(std::istream &os, NFA &m) {
        unsigned f = 0, p = 0;
        os >> m.statesCnt;
        os >> m.startState;
        os >> f;
        m.endStates.resize(m.statesCnt);
        for (size_t i = 0; i < f; i++) {
            unsigned tmp;
            os >> tmp;
            m.endStates[tmp] = true;
        }
        os >> p;
        std::vector<bool> bitMask(m.statesCnt);
        std::vector<bool> bitMaskTo(m.statesCnt);
        for (size_t i = 0; i < p; i++) {
            unsigned from, to;
            char chr;
            os >> from >> to >> chr;

            bitMask[from] = true;
            m.transitions[bitMask][chr].push_back(to);

            bitMask[from] = false;

//            m.transitions[std::pair<unsignedSet, char>(getSetOfOneEl(from), chr)].push_back(getSetOfOneEl(to));
        }
        return os;
    }

    unsigned getStartState() const {
        return startState;
    }

    size_t getCntStates() const {
        return statesCnt;
    }

    std::unordered_map<std::vector<bool>, std::unordered_map<char, std::vector<unsigned >>> &getTransitions() {
        return transitions;
    }

    std::vector<bool> &getEndStates() {
        return endStates;
    }
};


class DFA {

private:
    std::unordered_map<std::vector<bool>, std::unordered_map<char, std::vector<bool>>> transitions;
    std::unordered_set<std::vector<bool> > endStates;
    unsigned startState = 0;
    size_t cntStates = 0;

    std::vector<bool> getBitMask(unsigned el) const {
        std::vector<bool> a(cntStates);
        a[el] = true;
        return a;
    }

public:
    DFA() = default;

    explicit DFA(NFA &other) {
        startState = other.getStartState();
        std::queue<std::vector<bool> > q;
        cntStates = other.getCntStates();
        q.push(getBitMask(startState));
#ifdef DEBUG
        std::cout << "OTHER: \n";
        std::cout << "Start state: " << startState << std::endl;

        std::cout << "End state: ";
        for (auto k: other.getEndStates()) {
            std::cout << k;
        }
        std::cout << std::endl;
        std::cout << "explicit DFA(NFA &other)" << std::endl;
        for (const auto &key: other.getTransitions()) {
            for (auto k: key.first) {
                std::cout << k << " ";
            }
            std::cout << ":\n";
            for (const auto &v: key.second) {
                std::cout << v.first << ": ";
                for (auto z: v.second) {
                    std::cout << z << " ";
                }
                std::cout << std::endl;
            }
        }
#endif
        while (!q.empty()) {
            auto top = q.front();
            q.pop();

            for (size_t i = 0; i < cntStates; i++) {
                if (!top[i]) continue;
                for (const auto &ch: other.getTransitions()[getBitMask(i)]) {
//                    size_t k = 0;
                    if (transitions[top][ch.first].size() < cntStates)
                        transitions[top][ch.first].resize(cntStates);
                    for (auto el: ch.second) {
                        transitions[top][ch.first][el] = true;
                    }

//                    for (const auto &el: ch.second) {
//                        transitions[top][ch.first][k] = el || transitions[top][ch.first][k];
//                        k++;
//                    }
                    if (transitions.find(transitions[top][ch.first]) == transitions.end()) {
                        q.push(transitions[top][ch.first]);
                    }
                }
            }
        }


#ifdef DEBUG
        std::cout << "THIS: \n";
        std::cout << "Start state: " << startState << std::endl;

        std::cout << "End state: ";
        for (auto k: other.getEndStates()) {
            std::cout << k;
        }
        std::cout << std::endl;
        std::cout << "explicit DFA(NFA &other)" << std::endl;
        for (const auto &key: transitions) {
            for (auto k: key.first) {
                std::cout << k << " ";
            }
            std::cout << ":\n";
            for (const auto &v: key.second) {
                std::cout << v.first << ": ";
                for (auto z: v.second) {
                    std::cout << z << " ";
                }
                std::cout << std::endl;
            }
        }
#endif

//        for (const auto &key: transitions) {
//            for (auto t: other.getEndStates()) {
//                if (std::find(key.first.begin(), key.first.end(), t) != key.first.end()) {
//                    endStates.insert(key.first);
//                    break;
//                }
//                std::cout << t << std::endl;
//            }
//        }
//        for (auto t: other.getEndStates()) {
//            endStates.insert(getSetOfOneEl(t));
//        }
        auto t = other.getEndStates();
        for (const auto &key: transitions) {
            for (size_t i = 0; i < cntStates; i++) {
                if (!t[i]) continue;
                if (key.first[i]) {
                    endStates.insert(key.first);

                    break;
                }
            }
        }
        for (size_t i = 0; i < cntStates; i++) {
            if (!t[i]) continue;
            endStates.insert(getBitMask(i));
        }
#ifdef DEBUG
        std::cout << "explicit DFA(NFA &other)" << std::endl;
        for (auto key: endStates) {
            for (auto i: key) {
                std::cout << i << " ";
            }
            std::cout << std::endl;
        }
#endif
    }

    bool checkWord(std::string &a) {
        auto cur = getBitMask(startState);
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

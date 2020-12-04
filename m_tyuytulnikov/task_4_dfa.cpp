#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <fstream>
#include <queue>

#define set_of_unsigned set<unsigned>
using namespace std;

vector<size_t> appendUnique(vector<size_t> left, vector<size_t> right) {
    left.insert(left.end(), right.begin(), right.end());
    set<size_t> tmp(left.begin(), left.end());
    vector<size_t> result;
    result.assign(tmp.begin(), tmp.end());
    return result;
}

enum TypeOfCharacter {
    star, bar, dot, chara, paranthesisLeft, paranthesisRight
};

TypeOfCharacter returnCharState(char x) {
    switch (x) {
        case '*':
            return star;
        case '.':
            return dot;
        case '|':
            return bar;
        case '(':
            return paranthesisLeft;
        case ')':
            return paranthesisRight;
        default:
            return chara;
    }
}

set_of_unsigned initializeSet(unsigned el) {
    set_of_unsigned a;
    a.insert(el);
    return a;
}

class DFA {
private:
    map<set_of_unsigned, map<char, set_of_unsigned>> automaton;
    set<set_of_unsigned > final_states;
    unsigned start_state = 0;
    size_t num_of_states = 0;
    set<char> letters;
    vector<char> letter_of_position;

    class State {
    public:
        virtual bool isNullable() = 0;

        virtual vector<size_t> firstPos() = 0;

        virtual vector<size_t> lastPos() = 0;

        virtual void calculateFollowers(vector<vector<size_t>> &followers) {};

    };

    class DotState : public State {
    private:
        State *stateLeft;
        State *stateRight;
    public:
        DotState(State *stateLeft, State *stateRight) {
            this->stateLeft = stateLeft;
            this->stateRight = stateRight;
        }

        bool isNullable() override {
            return stateLeft->isNullable() && stateRight->isNullable();
        }

        vector<size_t> firstPos() override {
            return appendUnique(stateLeft->firstPos(),
                                !stateLeft->isNullable() ? vector<size_t>() : stateRight->firstPos());
        }

        vector<size_t> lastPos() override {
            return appendUnique(stateRight->lastPos(),
                                !stateRight->isNullable() ? vector<size_t>() : stateLeft->lastPos());
        }

        void calculateFollowers(vector<vector<size_t>> &followersMatrix) override {
            stateLeft->calculateFollowers(followersMatrix);
            stateRight->calculateFollowers(followersMatrix);
            for (size_t i : stateLeft->lastPos()) {
                for (size_t j : stateRight->firstPos()) {
                    followersMatrix[i].push_back(j);
                }
            }

        }

        ~DotState() {
            delete stateLeft;
            delete stateRight;
        }

    };

    class NodeState : public State {
        size_t position;
        char character;
    public:
        NodeState(size_t position, char character) {
            this->position = position;
            this->character = character;
        }

        bool isNullable() override {
            return false;
        }

        vector<size_t> firstPos() override {
            return vector<size_t>(1, position);
        }

        vector<size_t> lastPos() override {
            return vector<size_t>(1, position);
        }
    };

    class OrState : public State {

        State *stateLeft;
        State *stateRight;
    public:
        OrState(State *stateLeft, State *stateRight) {
            this->stateLeft = stateLeft;
            this->stateRight = stateRight;
        }

        bool isNullable() override {
            return stateLeft->isNullable() || stateRight->isNullable();
        }

        vector<size_t> firstPos() override {
            return appendUnique(stateLeft->firstPos(), stateRight->firstPos());
        }

        vector<size_t> lastPos() override {
            return appendUnique(stateRight->lastPos(), stateLeft->lastPos());
        }

        void calculateFollowers(vector<vector<size_t>> &followersMatrix) override {
            stateLeft->calculateFollowers(followersMatrix);
            stateRight->calculateFollowers(followersMatrix);

        }

        ~OrState() {
            delete stateLeft;
            delete stateRight;
        }

    };

    class StarState : public State {
        State *state;
    public:
        explicit StarState(State *state) {
            this->state = state;
        }

        bool isNullable() override {
            return true;
        }

        vector<size_t> firstPos() override {
            return state->firstPos();
        }

        vector<size_t> lastPos() override {
            return state->lastPos();
        }

        void calculateFollowers(vector<vector<size_t>> &followersMatrix) override {
            state->calculateFollowers(followersMatrix);
            for (size_t i : lastPos()) {
                for (size_t j : firstPos()) {
                    followersMatrix[i].push_back(j);
                }
            }
        }

        ~StarState() {
            delete state;
        }
    };

    State *convertRegexToTree(const string &regex) {
        vector<State *> states;
        for (char character : regex) {
            TypeOfCharacter type = returnCharState(character);
            switch (type) {
                case chara: {
                    auto *state = new NodeState(letter_of_position.size(), character);
                    letters.insert(character);
                    letter_of_position.push_back(character);
                    states.push_back(state);
                    break;
                }
                case star: {
                    State *lastState = states[states.size() - 1];
                    states.pop_back();
                    auto *newState = new StarState(lastState);
                    states.push_back(newState);
                    break;
                }
                case dot: {
                    State *leftState = states[states.size() - 2];
                    State *rightState = states[states.size() - 1];
                    states.pop_back();
                    states.pop_back();
                    auto *newState = new DotState(leftState, rightState);
                    states.push_back(newState);
                    break;
                }
                case bar: {
                    State *leftState = states[states.size() - 2];
                    State *rightState = states[states.size() - 1];
                    states.pop_back();
                    states.pop_back();
                    auto *newState = new OrState(leftState, rightState);
                    states.push_back(newState);
                    break;
                }
                case paranthesisLeft:
                    break;
                case paranthesisRight:
                    break;
            }

        }
        return states[0];
    }

    size_t findRightParanthesis(string characters, size_t offset) {
        size_t leftParanthesisFound = 0;
        for (size_t i = offset; i < characters.size(); i++) {
            if (characters[i] == '(') {
                leftParanthesisFound++;
            } else if (characters[i] == ')') {
                if (leftParanthesisFound == 0) {
                    return i - 1;
                } else leftParanthesisFound--;

            }
        }
        return characters.size() - 1;
    }

    string convertRegexToPolishForm(string regex) {
        string polish;
        for (size_t i = 0; i < regex.size(); i++) {
            char character = regex[i];
            TypeOfCharacter type = returnCharState(character);
            switch (type) {
                case star: {
                    polish.append("*");
                    break;
                }
                case chara: {
                    string str;
                    str += character;
                    if (!polish.empty()) {
                        str += ".";
                    }
                    polish.append(str);
                    break;
                }
                case bar: {
                    string rightSide;
                    rightSide.assign(regex.begin() + i + 1, regex.begin() + findRightParanthesis(regex, i) + 1);
                    polish.append(convertRegexToPolishForm(rightSide));
                    polish.append("|");
                    i += rightSide.size();
                    break;
                }
                case paranthesisLeft: {
                    string size_terior;
                    size_terior.assign(regex.begin() + i + 1, regex.begin() + findRightParanthesis(regex, i + 1) + 1);
                    string str;
                    str.append(convertRegexToPolishForm(size_terior));
                    i += size_terior.size() + 1;
                    if (i < regex.size() - 1 && regex[i] == '*') {
                        str.append("*");
                        i++;
                    }
                    if (!polish.empty()) {
                        str.append(".");
                    }
                    polish.append(str);
                    break;
                }
                case paranthesisRight: {
                    break;
                }

                case dot:
                    break;
            }
        }
        return polish;
    }

    void add_final_state(size_t t) {
        final_states.insert(initializeSet(t));
    }

public:
    DFA() = default;

    ~DFA() {
        for (auto it : automaton) {
            it.second.clear();
        }
        automaton.clear();
        final_states.clear();
        letters.clear();
        letter_of_position.clear();
    }

    explicit DFA(string &regex) :
            start_state(0) {
        State *state;
        string polish = convertRegexToPolishForm(std::move(regex));
        state = convertRegexToTree(polish);
        letters.erase('#');

        vector<vector<size_t>> vec(letter_of_position.size(), vector<size_t>());
        state->calculateFollowers(vec);
        vector<size_t> firstStatesVector = state->firstPos();
        set<size_t> firstState = set<size_t>(firstStatesVector.begin(), firstStatesVector.end());

        size_t currentState = 0;
        map<set<size_t>, size_t> statesToState;

        statesToState.insert(pair<set<size_t>, size_t>(firstState, currentState));

        queue<set<size_t>> currentStates;
        currentStates.push(firstState);
        currentState++;
        while (!currentStates.empty()) {
            set<size_t> positions = currentStates.front();
            currentStates.pop();
            size_t position = statesToState[positions];
            for (char c : letters) {
                set<size_t> nextState;
                for (size_t pos : positions) {
                    if (letter_of_position[pos] == c) {
                        vector<size_t> followPos = vec[pos];
                        for (size_t follow : followPos) {
                            nextState.insert(follow);
                        }
                    }
                }
                if (statesToState.find(nextState) == statesToState.end()) {
                    statesToState.insert(pair<set<size_t>, size_t>(nextState, currentState));
                    automaton[initializeSet(position)].insert(
                            pair<char, set_of_unsigned >(c, initializeSet(currentState)));
                    currentState++;
                    currentStates.push(nextState);
                } else {
                    automaton[initializeSet(position)][c] = initializeSet(statesToState[nextState]);
                }
            }
        }
        for (pair<set<size_t>, size_t> c : statesToState) {
            if (c.first.find(letter_of_position.size() - 1) != c.first.end()) {
                add_final_state(c.second);
            }
        }
    }

    friend istream &operator>>(istream &in, DFA &m) {
        unsigned f;
        in >> m.num_of_states >> m.start_state >> f;

        vector<unsigned> inputSet;
        for (size_t i = 0; i < f; i++) {
            unsigned tmp;
            in >> tmp;
            inputSet.emplace_back(tmp);
        }
        unsigned p;
        in >> p;
        for (size_t i = 0; i < p; i++) {
            unsigned from, to;
            char chr;
            in >> from >> to >> chr;
            m.automaton[initializeSet(from)][chr].insert(to);
        }

        queue<set_of_unsigned > q;
        q.push(initializeSet(m.start_state));
        while (!q.empty()) {
            auto top = q.front();
            q.pop();
            for (auto state: top) {
                for (const auto &ch: m.automaton[initializeSet(state)]) {
                    for (const auto &el: ch.second) {
                        m.automaton[top][ch.first].insert(el);
                    }
                    if (m.automaton.find(m.automaton[top][ch.first]) == m.automaton.end()) {
                        q.push(m.automaton[top][ch.first]);
                    }
                }
            }
        }

        for (const auto &key: m.automaton) {
            for (auto t: inputSet) {
                if (find(key.first.begin(), key.first.end(), t) != key.first.end()) {
                    m.final_states.insert(key.first);
                    break;
                }
            }
        }
        for (auto t: inputSet) {
            m.add_final_state(t);
        }
        return in;
    }

    bool check(string &a) {
        auto cur = initializeSet(start_state);
        for (auto i : a) {
            cur = automaton[cur][i];
        }
        return find(final_states.begin(), final_states.end(), cur) != final_states.end();
    }
};


int main() {
    ofstream fout("output.txt");
    ifstream fin("input.txt");
    DFA a;
    fin >> a;

    size_t t;
    fin >> t;
    for (size_t i = 0; i < t; i++) {
        string str;
        fin >> str;
        if (a.check(str)) {
            fout << "YES" << endl;
        } else
            fout << "NO" << endl;
    }

//    string regex = "abc(b|c)#";
//    regex += "#";   // char # in the end is required by algorithm
//    DFA a (regex);
//    string s = "abcb";
//    cout << a.check(s);
    return 0;
}
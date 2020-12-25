#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <fstream>
#include <queue>
#include <cassert>
#include <memory>

using set_of_unsigned = std::set<unsigned>;
using namespace std;

vector<size_t> appendUnique(vector<size_t> left, vector<size_t> right) {
    vector<size_t> result;
    set_union(left.begin(), left.end(), right.begin(), right.end(), back_inserter(result));
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
    set<set_of_unsigned> final_states;
    unsigned start_state = 0;
    size_t num_of_states = 0;
    set<char> letters;
    vector<char> letter_of_position;

    class State { //Абстрактный класс для формирования дерева regex
    public:
        virtual bool isNullable() = 0;

        virtual vector<size_t> firstPos() = 0;

        virtual vector<size_t> lastPos() = 0;

        virtual void calculateFollowers(vector<vector<size_t>> &followers) = 0;

        virtual ~State() = default;
    };

    class DotState : public State { //Точка
    private:
        unique_ptr<State> stateLeft;
        unique_ptr<State> stateRight;
    public:
        DotState(unique_ptr<State> stateLeft, unique_ptr<State> stateRight) {
            this->stateLeft = move(stateLeft);
            this->stateRight = move(stateRight);
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

        ~DotState() override {
            stateLeft.reset();
            stateRight.reset();
        }

    };

    class NodeState : public State { //Символ
        size_t position;
    public:
        explicit NodeState(size_t position) {
            this->position = position;
        }

        bool isNullable() override {
            return false;
        }

        void calculateFollowers(vector<vector<size_t>> &followersMatrix) override {
        }

        vector<size_t> firstPos() override {
            return vector<size_t>(1, position);
        }

        vector<size_t> lastPos() override {
            return vector<size_t>(1, position);
        }
    };

    class OrState : public State { //Состояние ИЛИ
        unique_ptr<State> stateLeft;
        unique_ptr<State> stateRight;
    public:
        OrState(unique_ptr<State> stateLeft, unique_ptr<State> stateRight) {
            this->stateLeft = move(stateLeft);
            this->stateRight = move(stateRight);
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

        ~OrState() override {
            stateLeft.reset();
            stateRight.reset();
        }

    };

    class StarState : public State { //Звездочка
        unique_ptr<State> state;
    public:
        explicit StarState(unique_ptr<State> state) {
            this->state = move(state);
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

        ~StarState() override {
            state.reset();
        }
    };

    unique_ptr<State> convertRegexToTree(const string &regex) {
        vector<unique_ptr<State>> states;
        for (char character : regex) {
            TypeOfCharacter type = returnCharState(character);

            switch (type) {
                case chara: {
                    auto state = unique_ptr<State>(new NodeState(letter_of_position.size()));
                    letters.insert(character);
                    letter_of_position.push_back(character);
                    states.push_back(move(state));
                    break;
                }
                case star: {
                    assert(!states.empty());
                    auto lastState = move(states[states.size() - 1]);
                    states.pop_back();
                    auto newState = unique_ptr<State>(new StarState(move(lastState)));
                    states.push_back(move(newState));
                    break;
                }
                case dot: {
                    assert(!states.empty());
                    auto rightState = move(states[states.size() - 1]);

                    unique_ptr<State> leftState = nullptr;
                    if (states.size() > 1) {
                        leftState = move(states[states.size() - 2]);
                    }
                    states.pop_back();
                    states.pop_back();
                    auto newState = unique_ptr<State>(new DotState(move(leftState), move(rightState)));
                    states.push_back(move(newState));
                    break;
                }
                case bar: {
                    assert(!states.empty());
                    auto rightState = move(states[states.size() - 1]);

                    unique_ptr<State> leftState = nullptr;
                    if (states.size() > 1) {
                        leftState = move(states[states.size() - 2]);
                    }
                    states.pop_back();
                    states.pop_back();
                    auto newState = unique_ptr<State>(new OrState(move(leftState), move(rightState)));
                    states.push_back(move(newState));
                    break;
                }
                case paranthesisLeft:
                    break;
                case paranthesisRight:
                    break;
            }
        }
        assert(!states.empty());

        return move(states[0]);
    }

    static size_t findRightParanthesis(string characters, size_t offset) {
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

    static string convertRegexToPolishForm(const string &regex) {
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
                    string str = {character};
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
    }

    explicit DFA(const string &regex) :
            start_state(0) {
        string polish = convertRegexToPolishForm(regex);
        auto state = convertRegexToTree(polish); //Конвертируем regex string в дерево стейтов
        letters.erase('#'); //Убираем разграничивающую #

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
        while (!currentStates.empty()) { //Обход по стейтам
            set<size_t> positions = currentStates.front();
            currentStates.pop();
            size_t position = statesToState[positions];
            for (char c : letters) {
                set<size_t> nextState;
                for (size_t pos : positions) {
                    if (letter_of_position[pos] == c) {
                        vector<size_t> followPos = vec[pos];
                        for (size_t follow : followPos) {
                            nextState.insert(follow); //Добавляем в стейты из дерева
                        }
                    }
                }
                if (statesToState.find(nextState) == statesToState.end()) {
                    statesToState.insert(pair<set<size_t>, size_t>(nextState, currentState));
                    automaton[initializeSet(position)].insert(
                            pair<char, set_of_unsigned>(c, initializeSet(currentState)));
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

        queue<set_of_unsigned> q;
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

//    string regex = "abc(d|e)";
//    regex += "#";   // char # in the end is required by algorithm
//    DFA a(regex);
//    string s = "abcd";
//    cout << a.check(s);

    return 0;
}
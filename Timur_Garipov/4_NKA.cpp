
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <set>

using namespace std;
typedef unsigned short int type;

struct Key {
    type cond;
    char symbol;
    Key(type c, char s) {
        this->cond = c;
        this->symbol = s;
    }
    Key() {
        cond = 0;
        symbol = 0;
    }
};

namespace std
{
    template<> struct less<Key>
    {
        bool operator() (const Key& lhs, const Key& rhs) const
        {
            if (lhs.cond < rhs.cond)
                return true;
            else if (lhs.cond > rhs.cond)
                return false;
            else {
                if (lhs.symbol < rhs.symbol)
                    return true;
                else
                    return false;
            }
        }
    };
}

class NKA {
private:
    std::map<Key, std::vector<type>> map_;
    std::vector<std::vector<type>> ways;
    void clearWays() {
        for (type i = 0; i < ways.size(); i++)
            ways[i].clear();
        ways.clear();
    }
    void addWay(const type& index) {
        ways.push_back(ways[index]);
    }
protected:
    std::vector<type> end_cond;
    type start_cond;
    bool checkRead() {
        for (type i = 0; i < ways.size(); i++) {
            for (type j = 0; j < end_cond.size(); j++)
                if (ways[i][ways[i].size() - 1] == end_cond[j] && ways[i][0] == 0)
                    return true;
        }
        return false;
    }
    void checkIter(const char& symb) {

        int size = ways.size();
        for (int i = 0; i < size; i++) {

            if (ways[i][0] == 0) {

                type currentCond = ways[i][ways[i].size() - 1];
                Key c(currentCond, symb);


                if (map_[c].size() == 0) {
                    ways[i][0] = 1;
                }

                else if (map_[c].size() == 1) {
                    ways[i].push_back(map_[c][0]);
                }

                else if (map_[c].size() > 1) {

                    int z = map_[c].size() - 1;
                    for (int j = 0; j < z; j++)
                        addWay(i);

                    ways[i].push_back(map_[c][0]);


                    for (int j = ways.size() - 1; z != 0; j--, z--) {
                        ways[j].push_back(map_[c][z]);
                    }
                }
            }
        }
    }
public:
    NKA() {}
    NKA(std::ifstream& fin) {
        type countEndCond, countFunctionTrans, countCond;
        fin >> countCond >> start_cond >> countEndCond;
        for (type i = 0; i < countEndCond; i++) {
            type value;
            fin >> value;
            end_cond.push_back(value);
        }
        fin >> countFunctionTrans;
        for (type i = 0; i < countFunctionTrans; i++) {
            type from, to;
            char value;
            fin >> from >> to >> value;
            Key c(from, value);
            map_[c].push_back(to);
        }
    }
    ~NKA() {
        auto it = map_.begin();
        for (int i = 0; it != map_.end(); i++, it++) {
            it->second.clear();
        }
        map_.clear();
        clearWays();
        end_cond.clear();
    }
    std::string checkWord(const std::string& word) {
        bool readFlag = 0;
        int size = word.length();

        std::vector<type> support;
        support.push_back(0);
        support.push_back(start_cond);
        ways.push_back(support);
        support.clear();
        for (int i = 0; i < size; i++) {
            checkIter(word[i]);
        }

        readFlag = checkRead();
        clearWays();
        if (readFlag) return "YES";
        else return "NO";
    }
};


#define set_unsigned set<unsigned>
vector<size_t> appendUnique(vector<size_t> left, vector<size_t> right) {
    left.insert(left.end(), right.begin(), right.end());
    set<size_t> tmp(left.begin(), left.end());
    vector<size_t> result;
    result.assign(tmp.begin(), tmp.end());
    return result;
}

class DKA :public NKA {
private:
    std::map<Key, type> map_;
    std::vector<type> way;
    void checkIter(const char& symb) {

        if (way[0] == 0) {

            type currentCond = way[way.size() - 1];
            Key c(currentCond, symb);

            if(map_.count(c) == 0){
                way[0] = 1;
            }

            else {
                way.push_back(map_[c]);
            }
        }

    }
    bool checkRead() {
        for (type j = 0; j < end_cond.size(); j++)
            if (way[way.size() - 1] == end_cond[j])
                return true;
        return false;
    }

    map<set_unsigned, map<char, set_unsigned>> automation;
    set<set_unsigned > final_states;
    vector<char> letter_of_position;
    set<char> letters;
    class State {
    public:
        virtual bool isNullable() = 0;
        virtual vector<size_t> firstPos() = 0;
        virtual vector<size_t> lastPos() = 0;
        virtual void calculateFollowers(vector<vector<size_t>>& followers) {};
    };
    class DotState : public State {
    private:
        State* stateLeft;
        State* stateRight;
    public:
        DotState(State* stateLeft, State* stateRight) {
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
        void calculateFollowers(vector<vector<size_t>>& followersMatrix) override {
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
        State* stateLeft;
        State* stateRight;
    public:
        OrState(State* stateLeft, State* stateRight) {
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
        void calculateFollowers(vector<vector<size_t>>& followersMatrix) override {
            stateLeft->calculateFollowers(followersMatrix);
            stateRight->calculateFollowers(followersMatrix);

        }
        ~OrState() {
            delete stateLeft;
            delete stateRight;
        }
    };
    class StarState : public State {
        State* state;
    public:
        explicit StarState(State* state) {
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

        void calculateFollowers(vector<vector<size_t>>& followersMatrix) override {
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
    enum typeChar {
        star, bar, dot, char_, parLeft, parRight
    };
    typeChar getTypeOfChar(const char& symb) {
        switch (symb) {
            case '*':
                return star;
            case '|':
                return bar;
            case '(':
                return parLeft;
            case ')':
                return parRight;
            default:
                return char_;
        }
    }
    int findRightPar(string characters, int offset) {
        int leftParanthesisFound = 0;
        for (int i = offset; i < characters.size(); i++) {
            if (characters[i] == '(') {
                leftParanthesisFound++;
            }
            else if (characters[i] == ')') {
                if (leftParanthesisFound == 0) {
                    return i - 1;
                }
                else leftParanthesisFound--;

            }
        }
        return characters.size() - 1;
    }
    string convertRegexToPolishForm(string regex) {
        string polish;
        for (size_t i = 0; i < regex.size(); i++) {
            char character = regex[i];
            typeChar type = getTypeOfChar(character);
            switch (type) {
                case star: {
                    polish.append("*");
                    break;
                }
                case char_: {
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
                    rightSide.assign(regex.begin() + i + 1, regex.begin() + findRightPar(regex, i) + 1);
                    polish.append(convertRegexToPolishForm(rightSide));
                    polish.append("|");
                    i += rightSide.size();
                    break;
                }
                case parLeft: {
                    string size_terior;
                    size_terior.assign(regex.begin() + i + 1, regex.begin() + findRightPar(regex, i + 1) + 1);
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
                case parRight: {
                    break;
                }

                case dot:
                    break;
            }
        }
        return polish;
    }
    State* convertRegexToTree(const string& regex) {
        vector<State*> states;
        for (char character : regex) {
            typeChar type = getTypeOfChar(character);
            switch (type) {
                case char_: {
                    auto* state = new NodeState(letter_of_position.size(), character);
                    letters.insert(character);
                    letter_of_position.push_back(character);
                    states.push_back(state);
                    break;
                }
                case star: {
                    State* lastState = states[states.size() - 1];
                    states.pop_back();
                    auto* newState = new StarState(lastState);
                    states.push_back(newState);
                    break;
                }
                case dot: {
                    State* leftState = states[states.size() - 2];
                    State* rightState = states[states.size() - 1];
                    states.pop_back();
                    states.pop_back();
                    auto* newState = new DotState(leftState, rightState);
                    states.push_back(newState);
                    break;
                }
                case bar: {
                    State* leftState = states[states.size() - 2];
                    State* rightState = states[states.size() - 1];
                    states.pop_back();
                    states.pop_back();
                    auto* newState = new OrState(leftState, rightState);
                    states.push_back(newState);
                    break;
                }
                case parLeft:
                    break;
                case parRight:
                    break;
            }

        }
        return states[0];
    }
    set_unsigned initializeSet(unsigned el) {
        set_unsigned a;
        a.insert(el);
        return a;
    }
    void add_final_state(size_t t) {
        final_states.insert(initializeSet(t));
    }
public:
    DKA(std::ifstream& fin) {
        type countEndCond, countFunctionTrans, countCond;
        fin >> countCond >> start_cond >> countEndCond;
        for (type i = 0; i < countEndCond; i++) {
            type value;
            fin >> value;
            end_cond.push_back(value);
        }
        fin >> countFunctionTrans;
        for (type i = 0; i < countFunctionTrans; i++) {
            type from, to;
            char value;
            fin >> from >> to >> value;
            Key c(from, value);
            map_[c] = to;
        }
    }
    DKA(std::string str_reg) {
        str_reg += '#';
        int size = str_reg.size();
        string polish = convertRegexToPolishForm(str_reg);

        State* state;
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
                    automation[initializeSet(position)].insert(
                            pair<char, set_unsigned >(c, initializeSet(currentState)));
                    currentState++;
                    currentStates.push(nextState);
                }
                else {
                    automation[initializeSet(position)][c] = initializeSet(statesToState[nextState]);
                }
            }
        }
        for (pair<set<size_t>, size_t> c : statesToState) {
            if (c.first.find(letter_of_position.size() - 1) != c.first.end()) {
                add_final_state(c.second);
            }
        }
        return;
    }
    ~DKA() {
        way.clear();
        map_.clear();
        end_cond.clear();
    }
    std::string checkWord(const std::string& word) {
        bool readFlag = 0;
        int size = word.length();

        way.push_back(0);
        way.push_back(start_cond);
        for (int i = 0; i < size; i++) {
            checkIter(word[i]);
        }

        readFlag = checkRead();
        if (readFlag) return "YES";
        else return "NO";
    }
};

int main()
{

    std::ofstream fout("output.txt");
    std::ifstream fin("input.txt");
    NKA objectNKA(fin);

    int countWord;
    fin >> countWord;

    for (int i = 0; i < countWord; i++) {
        std::string Word;
        fin >> Word;
        fout << objectNKA.checkWord(Word) << std::endl;
    }

    fout.close();
    fin.close();

    return EXIT_SUCCESS;
}
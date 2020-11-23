#include <iostream>
#include <fstream>
#include <vector>
#include <map>

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

class DKA :public NKA {
private:
    std::map<Key, type> map_;
    std::vector<type> way;
    void checkIter(const char& symb) {
        if (way[0] == 0) {
            type currentCond = way[way.size() - 1];
            Key c(currentCond, symb);

            if (map_[c] == NULL) {
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
}
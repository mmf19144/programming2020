#include <iostream>
#include <fstream>
#include <cstring>
#include <map>
#include <algorithm>
#include <cctype>

bool cmp(const std::pair<std::string, int> &a, const std::pair<std::string, int> &b) {
    return a.second < b.second;
}

int main() {
    std::ofstream fout("output.txt");
    std::ifstream fin("input.txt");

    std::map<std::string, int> dict;
    std::string word;
    while (fin >> word) {
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);

        for (int i = 0, len = word.size(); i < len; i++) {
            if (std::ispunct(word[i])) {
                word.erase(i--, 1);
                len = word.size();
            }
        }
        if (!word.empty()) {
            if (dict.count(word) == 0)
                dict[word] = 1;
            else
                dict[word]++;
        }
    }

    int max_freq = std::max_element(dict.begin(), dict.end(), cmp)->second;
    int min_freq = std::min_element(dict.begin(), dict.end(), cmp)->second;

    std::cout << dict.size() << " " << max_freq << " " << min_freq;
    return 0;
}

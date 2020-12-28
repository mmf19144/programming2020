#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <cctype>
#include <algorithm>

using namespace std;
bool compare(const pair <string,int>&a, const pair <string, int>&b){
    return a.second < b.second;
}



int main(){
    ifstream ifile ("input.txt");
    ofstream ofile ("output.txt");
    map <string , int> dict;
    string word;
    while (ifile >> word ) {
        transform(word.begin(), word.end(), word.begin(), ::tolower);
        string::iterator it;
        it = word.begin();
        while (it != word.end()){
            if (ispunct(*it)) {
                it = word.erase(it);

            } else {
                ++it;
            }
        }

        if (dict.count(word) == 0){
            dict[word] =1;
        } else{
            dict[word]++;
        }
    }

    int max = max_element(dict.begin(), dict.end(), compare)->second;
    int min = min_element(dict.begin(), dict.end(), compare)->second;

    ofile << dict.size() <<' '<< max << ' '<< min;


    return 0;
}

#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <algorithm>


using namespace std;

int main()
{
    ifstream fin{ "input.txt" };
    ofstream fout{ "output.txt" };

    string dev(".:,;()-?!");//Разделители
    string str;
    string temp;
    //Ассоциативный контейнер, хранит пары (слово, число вхождений)
    map<string, size_t> freq;
    while (getline(fin, temp))
    {
        str.append(temp, 0, temp.length());
        str.push_back(' ');
    }
    transform(str.begin(), str.end(), str.begin(), tolower);
    size_t str_count = 0;//Число слов
    //Замена символов-разделителей из строки на пробел
    size_t index;
    while ((index = str.find_first_of(dev)) != string::npos)
        str[index] = ' ';
    //fout << str << endl;
    //Разбиение строки на слова, помещение слов в ассоциативный контейнер 
    //и подсчет числа вхождений слова в строку
    stringstream ss(str);
    while (ss >> str)
    {
        ++str_count;
        freq[str]++;
    }
    size_t max = 0;
    size_t min = str.size();
    size_t all = 0;
    for (map<string, size_t>::iterator iter = freq.begin(); 
        iter != freq.end(); ++iter)
    {
        size_t p = iter->second;
        if (min > p) min = p;
        if (max < p) max = p;
        all++;
        
    }
    fout << all << ' ' << max << ' ' << min;
    return 0;
}
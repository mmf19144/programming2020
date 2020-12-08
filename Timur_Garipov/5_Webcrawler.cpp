#include <iostream>
#include <fstream>
#include <vector>
#include <mutex>
#include <thread>
#include <queue>
#include <chrono>

#include <map>

#define STORE "store/"
#define PATH "test_data/"
using namespace std;

int countThread;
queue<string> q_Ref;
map<string, bool> wasHere;
map<std::thread::id, bool> threadMap;
vector<thread*> Threads;
mutex locker;
vector<bool> isWorking;


class Web_page {
private:
    std::string content;
    std::vector<std::string> ref;
    int getPosition(const std::string& str, const int& i1, char symb) {
        for (int i = i1; i < str.length(); i++) {
            if (str[i] == symb)
                return i;
        }
        return i1;
    }
    void getReference() {
        for (int i = 0; i < content.size(); i++) {
            if (content[i] == '<' && i + 10 < content.size()) {
                std::string token = content.substr(i + 1, getPosition(content, i + 1, '>') - 1);
                if (token.length() > 0 && token[0] == 'a' && token.substr(2, 6) == "href=\"") {
                    token = token.substr(8, getPosition(token, 8, '>') - 9);
                    if (token.length() > 0) {

                        locker.lock();
                        q_Ref.push(token);
                        locker.unlock();

                        ref.push_back(token);
                    }
                }
            }
        }
    }
    string getProtocol(const std::string& input) {
        string delimetr = "://";
        string NAME = input.substr(0, input.find(delimetr));
        return NAME;
    }
    string getAddress(const std::string& input) {
        string delimetr = "://";
        string NAME = input.substr(input.find(delimetr) + 3, input.size() - delimetr.size());
        return NAME;
    }
    string getNameOfFIle(const std::string& input) {
        int i;
        for (i = input.size() - 1; ; --i)
            if (input[i] == '/')
                break;
        i++;
        return input.substr(i, input.size() - 1);
    }
public:
    void copy(const std::string& input) {
        std::string out(STORE);
        out.append(getNameOfFIle(input));

        ofstream fout(out);
        fout << content;
        fout.close();
    }
    void buildPage(const std::string& input) {
        content = "";
        ref.clear();
        string full = string(PATH).append(getAddress(input));
        ifstream fin(full);
        content = string(istreambuf_iterator<char>(fin), istreambuf_iterator<char>());
        fin.close();
        getReference();
    }
    Web_page() {  }
    ~Web_page() {
        content.clear();
        ref.clear();
    }
};

bool oneIsWorking() {
    for (auto i = threadMap.begin(); i != threadMap.end(); i++)
        if (i->second == true) {
            return true;
        }

    return false;
}

void startProgram() {
    auto this_id = std::this_thread::get_id();
    Web_page web;
    while (oneIsWorking()) {
        while (true) {
            locker.lock();
            if (q_Ref.empty()) {
                locker.unlock();
                break;
            }

            threadMap[this_id] = true;
            string str = q_Ref.front();
            q_Ref.pop();
            locker.unlock();

            if (wasHere[str] == false) {
                locker.lock();
                wasHere[str] = true;
                locker.unlock();

                web.buildPage(str);
            }
        }
        threadMap[this_id] = false;
    }
}

int main()
{
    string start;
    ifstream fin("input.txt");
    fin >> start >> countThread;
    fin.close();
    q_Ref.push(start);
    Threads.resize(countThread);
    isWorking.resize(countThread);

    auto time_start = std::chrono::system_clock::now();
    for (int i = 0; i < countThread; i++) {
        Threads[i] = new thread(startProgram);
        auto id = Threads[i]->get_id();
        threadMap[id] = true;
    }
    for (int i = 0; i < countThread; i++)
        Threads[i]->join();
    auto time_end = std::chrono::system_clock::now();

    for (int i = 0; i < countThread; i++)
        Threads[i]->~thread();

    std::chrono::duration<double> time = time_end - time_start;
    ofstream fout("output.txt");
    fout << wasHere.size() << " " << time.count() << std::endl;
    fout.close();
    wasHere.clear();

    return EXIT_SUCCESS;
}
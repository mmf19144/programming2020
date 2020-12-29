#include <iostream>
#include <fstream>
#include <vector>
#include <mutex>
#include <thread>
#include <queue>
#include <chrono>
#include <regex>
#include <map>
#include <set>

const std::string STORE = "store/";
const std::string PATH = "test_data/";
const std::regex regExpr("<a href=\"([^<]+)\">");
using namespace std;

class Web_page {
private:
    mutex* locker;
    queue<string>* queueRef;
    std::string content;
    std::set<std::string> ref;
    int getPosition(const std::string& str, const int& i1, char symb) {
        for (int i = i1; i < str.length(); i++) {
            if (str[i] == symb)
                return i;
        }
        return i1;
    }
    void getReference() {
        std::smatch result;
        while (regex_search(content, result, regExpr)) {
            std::unique_lock<std::mutex> guard(*locker);

            queueRef->push(result[1]);
            ref.insert(result[1]);
            content = result.suffix().str();
        }
    }
    string getProtocol(const std::string& request) {
        string delimetr = "://";
        string NAME = request.substr(0, request.find(delimetr));
        return NAME;
    }
    string getAddress(const std::string& request) {
        string delimetr = "://";
        string NAME = request.substr(request.find(delimetr) + 3, request.size() - delimetr.size());
        return NAME;
    }
    string getNameOfFIle(const std::string& address) {
        int i;
        for (i = address.size() - 1; ; --i)
            if (address[i] == '/')
                break;
        i++;
        return address.substr(i, address.size() - 1);
    }
public:
    void copy(const std::string& address) {
        std::string out(STORE);
        out.append(getNameOfFIle(address));

        ofstream fout(out);
        fout << content;
        fout.close();
    }
    static Web_page buildPage(const std::string& input, mutex& lock, queue<string>& q_Ref) {
        Web_page web;
        web.queueRef = &q_Ref;
        web.locker = &lock;
        web.content = "";
        web.ref.clear();
        string full = string(PATH).append(web.getAddress(input));
        ifstream fin(full);
        web.content = string(istreambuf_iterator<char>(fin), istreambuf_iterator<char>());
        fin.close();
        web.getReference();
        return web;
    }
    Web_page() { }
    ~Web_page() { }
};

void startProgram(map<std::thread::id, bool>& threadMap, set<string>& wasHere,
                  queue<string>& q_Ref, mutex& locker) {
    auto this_id = std::this_thread::get_id();
    Web_page web;

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

        std::unique_lock<std::mutex> guard(locker);
        if (wasHere.count(str) == 0) {
            wasHere.insert(str);
            guard.unlock();

            web = Web_page::buildPage(str, locker, q_Ref);
        }
    }
    threadMap[this_id] = false;
}

int main()
{
    int countThread;
    queue<string> q_Ref;
    set<string> wasHere;
    map<std::thread::id, bool> threadMap;
    vector<thread> Threads;
    vector<bool> isWorking;
    mutex locker;

    string start;
    ifstream fin("input.txt");
    fin >> start >> countThread;
    fin.close();
    q_Ref.push(start);
    isWorking.resize(countThread);

    auto time_start = std::chrono::system_clock::now();
    for (int i = 0; i < countThread; i++) {
        Threads.emplace_back(startProgram, ref(threadMap), ref(wasHere), ref(q_Ref), ref(locker));
        auto id = Threads[i].get_id();
        threadMap[id] = true;
    }
    for (int i = 0; i < countThread; i++)
        Threads[i].join();
    auto time_end = std::chrono::system_clock::now();

    std::chrono::duration<double> time = time_end - time_start;
    ofstream fout("output.txt");
    fout << wasHere.size() << " " << time.count() << std::endl;
    fout.close();
    wasHere.clear();

    return EXIT_SUCCESS;
}
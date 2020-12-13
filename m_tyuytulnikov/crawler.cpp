#include <iostream>
#include <fstream>
#include <vector>
#include <mutex>
#include <thread>
#include <queue>
#include <chrono>
#include <map>

#define PATH "test_data/"
using namespace std;

queue<string> file_queue;
map<string, bool> completed_files;
mutex locker;
map<std::thread::id, bool> threadStatus;

class FilePage {
private:
    std::string page_content;

    void getReferences() {
        for (size_t i = 0; i < page_content.size(); i++) {
            if (page_content[i] == '<' && i + 10 < page_content.size()) {
                std::string token = page_content.substr(i + 1, page_content.find('<', i + 1) - 1);
                if (token.length() > 0 && token[0] == 'a' && token.substr(2, 6) == "href=\"") {
                    token = token.substr(8, token.find('>', 8) - 9);
                    if (token.length() > 0) {
                        locker.lock();
                        file_queue.push(token);
                        locker.unlock();
                    }
                }
            }
        }
    }

    static string getAddress(const std::string &input) {
        string delimiter = "://";
        string NAME = input.substr(input.find(delimiter) + 3, input.size() - delimiter.size());
        return NAME;
    }

public:
    void analyze(const std::string &input) {
        page_content = "";
        string full = string(PATH).append(getAddress(input));
        ifstream fin(full);
        page_content = string(istreambuf_iterator<char>(fin), istreambuf_iterator<char>());
        fin.close();
        getReferences();
    }

    FilePage() = default;

    ~FilePage() {
        page_content.clear();
    }
};

bool threadsWorking() {
    for (auto &i : threadStatus)
        if (i.second) {
            return true;
        }
    return false;
}

void run() {
    auto this_id = std::this_thread::get_id();
    FilePage page;
    while (threadsWorking()) {
        while (true) {
            locker.lock();
            if (file_queue.empty()) {
                locker.unlock();
                break;
            }
            threadStatus[this_id] = true;
            string str = file_queue.front();
            file_queue.pop();
            locker.unlock();

            if (!completed_files[str]) {
                locker.lock();
                completed_files[str] = true;
                locker.unlock();
                page.analyze(str);
            }
        }
        threadStatus[this_id] = false;
    }
}

class Crawler {
    const size_t num_of_threads;
    vector<thread *> Threads;
    vector<bool> isWorking;

public:
    Crawler(string startPage, size_t num_of_threads) :
            num_of_threads(num_of_threads) {
        file_queue.push(startPage);
        Threads.resize(num_of_threads);
        isWorking.resize(num_of_threads);

        auto time_start = std::chrono::system_clock::now();
        for (size_t i = 0; i < num_of_threads; i++) {
            Threads[i] = new thread(run);
            auto id = Threads[i]->get_id();
            threadStatus[id] = true;
        }
    }

    pair<unsigned long long int, double> crawl() {
        auto time_start = std::chrono::system_clock::now();
        for (size_t i = 0; i < num_of_threads; i++)
            Threads[i]->join();
        auto time_end = std::chrono::system_clock::now();
        std::chrono::duration<double> time = time_end - time_start;

        auto p = make_pair(completed_files.size(), time.count());
        return p;
    }

    ~Crawler() {
        for (size_t i = 0; i < num_of_threads; i++)
            Threads[i]->~thread();
    }
};

int main() {
    string startPage;
    size_t num_of_threads;
    ifstream fin("input.txt");
    fin >> startPage >> num_of_threads;
    fin.close();

    Crawler crawler(startPage, num_of_threads);
    auto res = crawler.crawl();

    ofstream fout("output.txt");
    fout << res.first << " " << res.second;
    fout.close();
    completed_files.clear();
    return 0;
}
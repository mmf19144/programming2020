#include <iostream>
#include <fstream>
#include <regex>
#include <exception>
#include <unordered_set>
#include <queue>
#include <mutex>
#include <thread>
#include <chrono>
#include <utility>
#include <sstream>


using namespace std;

#define BASE_PATH "test_data/"

class FileCrawler {
private:
    queue<string> tasks;
    unordered_set<string> parsed_files;
    mutex pushLock;
    size_t ready_tasks = 0;
    size_t added_tasks = 0;
public:
    const regex hrefRegexp;
    const regex urlRegexp;

    explicit FileCrawler(const string &baseUrl) : hrefRegexp("<a\\s+(?:[^>]*?\\s+)?href=\"([^\"]*)\""), urlRegexp(
            R"((http:\/\/www\.|https:\/\/www\.|http:\/\/|https:\/\/|file:\/\/)?([a-z0-9]+([\-\.]{1}[a-z0-9]+)*\.[a-z]{2,5}(:[0-9]{1,5})?(\/.*)?))") {
        pushTask(baseUrl);
    }

    ~FileCrawler() = default;

    pair<bool, string> getTask() {
        unique_lock<mutex> guard(pushLock);
        if (tasks.empty())
            return {false, ""};
        else {
            string tmp = tasks.front();
            tasks.pop();
            return {true, tmp};
        }
    }

    void pushTask(const string &href) {
        unique_lock<mutex> guard(pushLock);

        if (!href.empty() && parsed_files.find(href) == parsed_files.end()) {
            parsed_files.insert(href);
            tasks.push(href);
            added_tasks++;
        }
    }

    pair<size_t, double> run(size_t threads_cnt);

    bool hasWork() {
        unique_lock<mutex> guard(pushLock);
        return ready_tasks < added_tasks;
    }

    void finishTask() {
        unique_lock<mutex> guard(pushLock);
        ready_tasks++;
    }


    string getStrByHref(const string &href) {
        smatch result;
        regex_search(href, result, urlRegexp);


        if (result.empty()) {
            throw invalid_argument("Protocol is not available");
        }
        auto protocol = result.str(1);
        auto path = result.str(2);


        if (protocol == "file://") {
            ifstream fin(BASE_PATH + path);
            std::stringstream strStream;
            strStream << fin.rdbuf();
            return strStream.str();
        }

        throw invalid_argument("Protocol is not available");
    }
};


void threadFunction(FileCrawler *Crawler) {

    while (Crawler->hasWork()) {
        auto res = Crawler->getTask();
        do {
            smatch result;

            if (res.first) {
                string body = Crawler->getStrByHref(res.second);
                while (regex_search(body, result, Crawler->hrefRegexp)) {
                    string href = result.str(1);
                    Crawler->pushTask(href);
//                    cout << href << endl;
                    body = result.suffix().str();
                }
                Crawler->finishTask();
            }
            res = Crawler->getTask();
        } while (res.first);

    }
}

pair<size_t, double> FileCrawler::run(size_t threads_cnt) {

    vector<thread *> threads;
    threads.resize(threads_cnt);
    auto start = std::chrono::system_clock::now();
    for (int i = 0; i < threads_cnt; i++) {
        threads[i] = new thread(threadFunction, this);
    }


    for (int i = 0; i < threads_cnt; i++)
        threads[i]->join();

    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;

    return {ready_tasks, elapsed_seconds.count()};
}

int main() {
    ifstream fin("input.txt");
    ofstream fout("output.txt");
    string baseHref;
    size_t workersCnt;
    fin >> baseHref >> workersCnt;
    FileCrawler craw(baseHref);

    auto a = craw.run(workersCnt);
    fout << a.first << " " << a.second << endl;
    return 0;
}
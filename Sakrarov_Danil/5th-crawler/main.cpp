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

using namespace std;

const string BASE_PATH = "test_data";

class FileCrawler {
private:
    queue<string> tasks;
    unordered_set<string> parsed_files;
    mutex pushLock;
    size_t ready_tasks = 0;
    size_t added_tasks = 0;
public:
    FileCrawler(const string &baseUrl, string &basePath) {
        tasks.push(baseUrl);
        parsed_files.insert(baseUrl);
        added_tasks++;
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

    void run(size_t threads_cnt);

    bool hasWork() const {
        return ready_tasks < added_tasks;
    }

    void finishTask() {
        ready_tasks++;
    }


    static string getStrByHref(const string &href) {
        string delimiter = "://";
        auto finding = href.find(delimiter);
        if (finding == -1) {
            throw invalid_argument("Protocol is not available");
        }

        string protocol = href.substr(0, finding);
        string path = href.substr(finding + delimiter.length(), href.length());
        if (protocol == "file") {
            ifstream fin(BASE_PATH + '/' + path);
            return string((istreambuf_iterator<char>(fin)),
                          (istreambuf_iterator<char>()));
        }
        throw invalid_argument("Protocol is not available");
    }
};


void threadFunction(FileCrawler *Crawler) {
    regex regexp("<a[^>]* href=\"([^<]+)\"[^>]*>");
    while (Crawler->hasWork()) {
        auto res = Crawler->getTask();
        do {
            smatch result;

            if (res.first) {
                string body = FileCrawler::getStrByHref(res.second);
                while (regex_search(body, result, regexp)) {
                    string href = result.str(1);
                    Crawler->pushTask(href);
                    body = result.suffix().str();
                }
                Crawler->finishTask();
            }
            res = Crawler->getTask();
        } while (res.first);

    }
}

void FileCrawler::run(size_t threads_cnt) {

    vector<thread *> threads;
    threads.resize(threads_cnt);
    for (int i = 0; i < threads_cnt; i++) {
        threads[i] = new thread(threadFunction, this);
    }

    auto start = std::chrono::system_clock::now();
    for (int i = 0; i < threads_cnt; i++)
        threads[i]->join();

    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;

    cout << "READY " << parsed_files.size() << " pages by " << elapsed_seconds.count() << " s" << endl;
}

int main() {
    ifstream fin("input.txt");
    ofstream fout("output.html");
    string baseHref;
    size_t workersCnt;
    fin >> baseHref >> workersCnt;
    string base_path = "test_data";
    FileCrawler craw(baseHref, base_path);

    craw.run(workersCnt);

    return 0;
}
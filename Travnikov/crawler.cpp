#include <iostream>
#include <fstream>
#include <regex>
#include <exception>
#include <set>
#include <queue>
#include <mutex>
#include <thread>
#include <chrono>
#include <utility>

using namespace std;

const string BASE_PATH = "test_data";

string getStrByHref(const string &href) {
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


class MultiThreadCrawler {
private:
    const size_t threads_cnt;
    vector<string> ready_urls;
    queue<string> q;
    vector<unique_ptr<thread>> threads;
    vector<bool> threads_status;
    mutex lock;

public:
    explicit MultiThreadCrawler(size_t cnt_thread) : threads_cnt(cnt_thread) {
        threads.resize(cnt_thread);
        threads_status.resize(cnt_thread);
    }

    ~MultiThreadCrawler() {
        threads.clear();
        threads_status.clear();
        ready_urls.clear();
    }

    void appendTask(const string &href) {
        if (href.empty())
            return;
        lock.lock();
        if (!any_of(ready_urls.begin(), ready_urls.end(), [&](const string& i) { return i == href;})) {
            q.push(href);
            ready_urls.push_back(href);
        }
        lock.unlock();
    }

    string frontTask() {
        lock.lock();
        if (q.empty()) {
            lock.unlock();
            return "";
        }
        string tmp = q.front();
        q.pop();
        lock.unlock();
        return tmp;
    }

    bool mustStop() {
        lock.lock();
        bool s = any_of(threads_status.begin(), threads_status.end(), [](bool i) { return i; });
        lock.unlock();
        return !s;
    }

    void setWorkForThread(size_t idx, bool status) {
        lock.lock();
        threads_status[idx] = status;
        lock.unlock();
    }

    void run(const string &startHref);
};

void threadFunc(MultiThreadCrawler *c, size_t idx) {
    while (!c->mustStop()) {
        auto str = c->frontTask();
        while (!str.empty()) {
            c->setWorkForThread(idx, true);
            string body = getStrByHref(str);
            for (size_t i = 0; i < body.size(); i++) {
                if (body[i] == '<' && i + 10 < body.size()) {
                    string token = body.substr(i + 1, body.find('<', i + 1) - 1);
                    if (token.length() > 0 && token[0] == 'a' && token.substr(2, 6) == "href=\"") {
                        token = token.substr(8, token.find('>', 8) - 9);
                        c->appendTask(token);
                    }
                }
            }
            str = c->frontTask();
        }
        c->setWorkForThread(idx, false);
    }
}

void MultiThreadCrawler::run(const string &startHref) {
    appendTask(startHref);
    for (int i = 0; i < threads_cnt; i++) {
        threads[i] = std::make_unique<thread>(threadFunc, this, i);
        threads_status[i] = true;
    }
    auto start = std::chrono::system_clock::now();
    for (int i = 0; i < threads_cnt; i++)
        threads[i]->join();
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    cout << ready_urls.size() << " " << elapsed_seconds.count();
}


int main() {
    ifstream fin("input.txt");
    string baseHref;
    size_t workersCnt;
    fin >> baseHref >> workersCnt;
    MultiThreadCrawler craw(workersCnt);
    craw.run(baseHref);
    return 0;
}
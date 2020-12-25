#include <iostream>
#include <fstream>
#include <regex>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <chrono>
#include <utility>
#include <unordered_set>
#include <condition_variable>

#include <re2/re2.h>

using namespace std;

//#define DEBUG
string ROOT_DIR = "test_data";
static regex regexp("<a[^>]* href=\"([^<]+)\"[^>]*>");

template<class Uniq>
class IJob {
public:
    virtual ~IJob() = default;

    virtual const Uniq &getUniq() = 0;

    virtual void Execute() = 0;
};

template<class T, class Uniq>
class Queue {
public:
    void Push(std::unique_ptr<T> job) {
        std::unique_lock<std::mutex> guard(lock);
        if (set.find(job->getUniq()) == set.end()) {
            set.insert(job->getUniq());
            queue.emplace(std::move(job));
            addedJobs++;
            cond.notify_one();
        }
    }

    std::unique_ptr<T> Pop() {
        std::unique_lock<std::mutex> guard(lock);
        cond.wait(guard, [&]() { return stop || !queue.empty(); });
        if (stop) {
            return nullptr;
        }

        auto job = std::move(queue.front());
        queue.pop();
        return std::move(job);
    }

    void Stop() {
        std::unique_lock<std::mutex> guard(lock);
        stop = true;
        cond.notify_all();
    }

    void CompleteJob() {
        std::unique_lock<std::mutex> guard(lock);
        completedJobs++;
        if (addedJobs == completedJobs) {
            stop = true;
            cond.notify_all();
        }
    }

    size_t getUniqSize() {
        return set.size();
    }

private:
    std::queue<std::unique_ptr<T>> queue;
    std::unordered_set<Uniq> set;
    std::mutex lock;
    std::condition_variable cond;
    bool stop = false;

    size_t addedJobs = 0;
    size_t completedJobs = 0;
};

class UrlJob : public IJob<std::string> {
private:
    string href;

    string getBodyByHref() {
        string delimiter = "://";
        auto finding = href.find(delimiter);
        if (finding == -1) {
#ifdef DEBUG
            cout << href << endl;

#endif
            throw invalid_argument("Protocol is not available");
        }


        string protocol = href.substr(0, finding);
        string path = href.substr(finding + delimiter.length(), href.length());
        if (protocol == "file") {
            ifstream fin(ROOT_DIR + '/' + path);
            return string((istreambuf_iterator<char>(fin)),
                          (istreambuf_iterator<char>()));
        }
        throw invalid_argument("Protocol is not available");
    }

public:
    explicit UrlJob(string &href_) : href(href_) {}

    std::string &getUniq() override {
        return href;
    }

    void Execute() override;
};

Queue<UrlJob, string> q;

void UrlJob::Execute() {
#ifdef DEBUG
    cout << href << endl;
#endif
    if (href.empty())
        return;

    string body = getBodyByHref();
    re2::StringPiece input(body);
    std::string newHref;
    while (re2::RE2::FindAndConsume(&input, "<a[^>]* href=\"([^<]+)\"[^>]*>", &newHref)) {
        if (newHref.empty()) {
            continue;
        }
        q.Push(std::make_unique<UrlJob>(newHref));
    }
}

void threadFunc() {
    std::unique_ptr<UrlJob> a = std::move(q.Pop());
    while (a != nullptr) {
        a->Execute();
        q.CompleteJob();
        a = std::move(q.Pop());
    }
}

int main() {

    ifstream fin("input.txt");
    ofstream fout("output.html");


    string baseHref;
    size_t workersCnt;
    fin >> baseHref >> workersCnt;

    vector<thread *> threads;
    q.Push(std::make_unique<UrlJob>(baseHref));
    threads.resize(workersCnt);

    for (int i = 0; i < workersCnt; i++) {
        threads[i] = (new thread(threadFunc));
    }
    auto start = std::chrono::system_clock::now();
    for (int i = 0; i < workersCnt; i++)
        threads[i]->join();

    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;

    cout << "READY " << q.getUniqSize() << " pages by " << elapsed_seconds.count() << " s" << endl;

    return 0;
}

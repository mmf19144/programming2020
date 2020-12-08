#include <iostream>
#include <fstream>
#include <regex>
#include <exception>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <utility>
#include <unordered_set>

using namespace std;


/*
 * For i9 with 8-threads (2,3) 16 threads are optimal
 */


//#define DEBUG

string ROOT_DIR = "test_data";

template<class Uniq>
class IJob {
public:
    virtual ~IJob() = default;

    virtual Uniq &getUniq() = 0;

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

    std::unique_ptr<T> TryPop() {
        std::unique_lock<std::mutex> guard(lock);
        if (stop || queue.empty()) {
            return nullptr;
        }

        auto job = std::move(queue.front());
        queue.pop();
        return std::move(job);
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
    const regex regexp("<a[^>]* href=\"([^<]+)\"[^>]*>");
    smatch result;
    if (href.empty())
        return;

    string body = getBodyByHref();

    while (regex_search(body, result, regexp)) {
        string newHref = result.str(1);
        if (newHref.empty()) {
            body = result.suffix().str();
            continue;
        }
        q.Push(std::make_unique<UrlJob>(newHref));
        body = result.suffix().str();
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

#include <iostream>
#include <fstream>
#include <regex>
#include <exception>
#include <unordered_set>
#include <queue>
#include <mutex>
#include <thread>
#include <chrono>

using namespace std;


/*
 * For i9 with 8-threads (2,3) 16 threads are optimal
 */


//#define DEBUG

const string ROOT_DIR = "";

mutex pushLock;

vector<bool> threadStatus;

string getBodyByHref(const string &href) {
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

bool checkTreads() {
    pushLock.lock();
    for (auto i: threadStatus) {
        if (i) {
            pushLock.unlock();
            return true;
        }

    }
    pushLock.unlock();
    return false;
}

void threadFunc(queue<string> *q, unordered_set<string> *urls, size_t id) {
    std::thread::id this_id = std::this_thread::get_id();
    const regex regexp("<a[^>]* href=\"([^<]+)\"[^>]*>");
    smatch result;
    while (checkTreads()) {
//
        while (!q->empty()) {
            pushLock.lock();
            threadStatus[id] = true;
            if (q->empty())
            {
                pushLock.unlock();
                continue;
            }

            string h = q->front();
            q->pop();

            pushLock.unlock();
            string body = getBodyByHref(h);
#ifdef DEBUG
            cout << h << endl;
#endif
            while (regex_search(body, result, regexp)) {
                string href = result.str(1);
                if (href.empty()) {
                    cout << result.str(0);
                }

                pushLock.lock();
                if (!href.empty() && urls->find(href) == urls->end()) {
                    urls->insert(href);
                    q->push(href);
                }

                pushLock.unlock();
                body = result.suffix().str();
            }

        }
        pushLock.lock();
        threadStatus[id] = false;
        pushLock.unlock();
    }
#ifdef DEBUG
    pushLock.lock();
    cout << "id" << this_id << " " << "\n";
    pushLock.unlock();
#endif

}


int main() {
    unordered_set<string> urls;
    queue<string> q;

    ifstream fin("input.txt");
    ofstream fout("output.html");


    string baseHref;
    size_t workersCnt;
    fin >> baseHref >> workersCnt;

    vector<thread *> threads;
    q.push(baseHref);
    urls.insert(baseHref);
    threads.resize(workersCnt);
    threadStatus.resize(workersCnt);

    for (int i = 0; i < workersCnt; i++) {
        threads[i] = (new thread(threadFunc, &q, &urls, i));
        threadStatus[i] = true;
    }
    auto start = std::chrono::system_clock::now();
    for (int i = 0; i < workersCnt; i++)
        threads[i]->join();

    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;

    cout << "READY " << urls.size() << " pages by " << elapsed_seconds.count() << " s" << endl;
    return 0;
}
#include <iostream>
#include <fstream>
#include <vector>
#include <mutex>
#include <thread>
#include <queue>
#include <chrono>
#include <map>
#include <unordered_set>
#include <regex>

using namespace std;

const string PATH = "test_data/";
mutex locker;

vector<bool> threadStatus;

bool threadsWorking() {
    std::unique_lock<std::mutex> guard(locker);
    for (auto i: threadStatus) {
        if (i) {
            return true;
        }
    }
    return false;
}

static string getFileName(const string &input) {
    smatch res;
    const regex rx("[0-9]*.html");
    regex_search(input, res, rx);
    return res.str();
}

void run(int thread_id, map<string, bool> &completed_files, queue<string> &file_queue) {
    const regex rx("<a[^>]* href=\"([^<]+)\"[^>]*>");

    while (threadsWorking()) {
        while (!file_queue.empty()) {
            locker.lock();

            string file_to_open = file_queue.front();
            file_queue.pop();
            locker.unlock();

            string page_content;
            ifstream in;
            in.open(PATH + file_to_open);
//            cout << file_to_open << " " << file_queue.size() << " " << completed_files.size() << endl;

            page_content = string(istreambuf_iterator<char>(in), istreambuf_iterator<char>());
            in.close();
            smatch regex_res;

            while (regex_search(page_content, regex_res, rx)) {
                string file_name = getFileName(regex_res[0]);
                if (!completed_files[file_name]) {
                    locker.lock();
                    file_queue.push(file_name);
                    completed_files[file_name] = true;
                    locker.unlock();
                }
                page_content = regex_res.suffix().str();
            }
        }
        threadStatus[thread_id] = false;
    }
}

class Crawler {
    const size_t num_of_threads;
    vector<thread> threads;
    map<string, bool> completed_files;
    queue<string> file_queue;
public:
    Crawler(const string &start_file, size_t num_of_threads) :
            num_of_threads(num_of_threads) {
        file_queue.push(start_file);
        completed_files[start_file] = true;

        threads.resize(num_of_threads);
        threadStatus.resize(num_of_threads);

        for (size_t i = 0; i < num_of_threads; i++) {
            threads[i] = thread(run, i, ref(completed_files), ref(file_queue));
            threadStatus[i] = true;
        }
    }

    pair<unsigned long long int, double> crawl() {
        auto time_start = std::chrono::system_clock::now();
        for (size_t i = 0; i < num_of_threads; i++)
            threads[i].join();
        auto time_end = std::chrono::system_clock::now();
        std::chrono::duration<double> time = time_end - time_start;

        return make_pair(completed_files.size(), time.count());
    }
};


int main() {
    string startPage;
    ifstream in;
    int num_of_threads;

    in.open("input.txt");
    in >> startPage;
    startPage = getFileName(startPage);
    in >> num_of_threads;
    in.close();

    Crawler crawler(startPage, num_of_threads);
    auto res = crawler.crawl();

    ofstream fout("output.txt");
    fout << res.first << " " << res.second;
    fout.close();

    return 0;
}
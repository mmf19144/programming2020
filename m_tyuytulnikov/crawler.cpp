#include <iostream>
#include <fstream>
#include <vector>
#include <mutex>
#include <thread>
#include <queue>
#include <chrono>
#include <map>
#include <regex>

using namespace std;

const string PATH = "test_data/";
mutex locker;

vector<bool> threadStatus;


static string getFileName(const string &input) {
    smatch res;
    const regex rx("[0-9]*.html");
    regex_search(input, res, rx);
    return res.str();
}

const regex rx("<a[^>]* href=\"([^<]+)\"[^>]*>");


/*
 *
 *    ^
 *   | |  - Elon Mask is genius
 *  <| |>
 *    *
 *    *
 *    *
 */

class Crawler {
    const size_t num_of_threads;
    vector<thread> threads;
    map<string, bool> completed_files;
    queue<string> file_queue;
    mutex m;
    bool stop = false;
    condition_variable conditionVariable;
    size_t available_tasks = 0;
public:
    Crawler(const string &start_file, size_t num_of_threads);

    pair<unsigned long long int, double> crawl() {
        auto time_start = std::chrono::system_clock::now();
        for (size_t i = 0; i < num_of_threads; i++)
            threads[i].join();
        auto time_end = std::chrono::system_clock::now();
        std::chrono::duration<double> time = time_end - time_start;

        return make_pair(completed_files.size(), time.count());
    }

    string queuePop() {
        std::unique_lock<std::mutex> guard(m);
        conditionVariable.wait(guard, [&]() { return stop || !file_queue.empty(); });
        if (stop) {
            return "";
        }
        auto tmp = file_queue.front();
        file_queue.pop();
        return tmp;
    }

    void queuePush(string &s) {
        std::unique_lock<std::mutex> guard(m);
        if (s.empty() || completed_files[s]) return;
        available_tasks++;
        file_queue.push(s);
        conditionVariable.notify_one();
        completed_files[s] = true;
    }

    void taskReady(){
        std::unique_lock<std::mutex> guard(m);
        available_tasks--;
        if (!available_tasks){
            stop = true;
            conditionVariable.notify_all();
        }
    }

};


void run(int thread_id, Crawler *cr) {


    while (true) {
        string file_to_open = cr->queuePop();
        if (file_to_open.empty())
            break;
        string page_content;
        ifstream in;
        in.open(PATH + file_to_open);
//        cout << file_to_open << endl;

        page_content = string(istreambuf_iterator<char>(in), istreambuf_iterator<char>());
        in.close();
        smatch regex_res;

        while (regex_search(page_content, regex_res, rx)) {
            string file_name = getFileName(regex_res[0]);
            cr->queuePush(file_name);
            page_content = regex_res.suffix().str();
        }
        cr->taskReady();
    }
    threadStatus[thread_id] = false;

}

Crawler::Crawler(const string &start_file, size_t num_of_threads) : num_of_threads(num_of_threads) {
    file_queue.push(start_file);
    completed_files[start_file] = true;
    available_tasks++;
    threads.resize(num_of_threads);
    threadStatus.resize(num_of_threads);

    for (size_t i = 0; i < num_of_threads; i++) {
        threads[i] = thread(run, i, this);
        threadStatus[i] = true;
    }

}


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
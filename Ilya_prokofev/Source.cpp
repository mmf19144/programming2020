#include <vector>
#include <thread>
#include <string>
#include <set>
#include <mutex>
#include <chrono>
#include <fstream>
#include <sstream>
#include <regex>
#include <iostream>

using namespace std;
set <string> opened_links;
set <string> links_to_open;
string start_page;
mutex opened_links_mutex;
mutex links_to_open_mutex;
const string path_to_test_data = "C:\\Users\\iprok\\test_data\\";
size_t opened_links_cnt = 0;
size_t links_to_open_cnt = 1;
const regex re_link("<a href=\"file://[_a-zA-Z0-9]*\\.(\\html)\">");

clock_t last_link_adding_time = clock();

string readFile(const string& fileName) {
	ifstream f(fileName);
	stringstream ss;
	ss << f.rdbuf();
	f.close();
	return ss.str();
}

set <string> open_page(string page) {
	string page_content = readFile(page);
	set<string> result;

	auto matches_begin = sregex_iterator(page_content.begin(), page_content.end(), re_link);
	auto matches_end = sregex_iterator();

	for (sregex_iterator link = matches_begin; link != matches_end; link++) {
		smatch match = *link;
		string match_str = match.str();
		match_str = match_str.substr(strlen("<a href=\"file://"), match_str.length() - strlen("<a href=\"file://") - strlen("\">"));

		result.insert(path_to_test_data + match_str);
	}
	return result;
}

void index() {
	string cur_page = "";
	unique_lock <mutex> unique_opened_links_mutex (opened_links_mutex, defer_lock);
	unique_lock <mutex> unique_links_to_open_mutex (links_to_open_mutex, defer_lock);
	while (1) {
		lock(unique_opened_links_mutex, unique_links_to_open_mutex);
		if (!links_to_open.empty()) {
			cur_page = *links_to_open.begin();
			links_to_open.erase(cur_page);
			opened_links.insert(cur_page);
			opened_links_cnt++;
			unique_opened_links_mutex.unlock();
			unique_links_to_open_mutex.unlock();
		}
		else {
			unique_opened_links_mutex.unlock();
			unique_links_to_open_mutex.unlock();
			this_thread::sleep_for(chrono::seconds(10));
			continue;
		}
		set <string> found_pages = open_page(cur_page);

		lock(unique_opened_links_mutex, unique_links_to_open_mutex);
		for (auto page : found_pages) {
			if (opened_links.find(page) == opened_links.end() && links_to_open.find(page) == links_to_open.end()) {
				links_to_open.insert(page);
				links_to_open_cnt ++;
			}
		}

		cout << opened_links_cnt << " " << links_to_open_cnt << endl;

		unique_opened_links_mutex.unlock();
		unique_links_to_open_mutex.unlock();
	}
}

void start_indexing(string start_page, size_t threads_num) {
	vector <thread> threads;
	links_to_open.insert(start_page);
	for (size_t i = 0; i < threads_num; i++) {
		threads.emplace_back(index);
	}

	for (auto& t : threads)
	{
		t.detach();
	}

	while (links_to_open_cnt != opened_links_cnt) {
		this_thread::sleep_for(chrono::seconds(10));
	}
}



int main() {
	ifstream input("input.txt");
	ofstream output("output.txt");
	string page_address;
	size_t workers_num;
	input >> page_address >> workers_num;
	auto start = chrono::system_clock::now();
	start_indexing(page_address, workers_num);
	auto finish = chrono::system_clock::now();
	chrono::duration<double> time = finish - start;

	output << opened_links_cnt << " " << time.count();
	input.close();
	output.close();
	return 0;
}
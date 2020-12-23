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

const string path_to_test_data = "C:\\Users\\iprok\\test_data\\";
const regex re_link("<a href=\"file://[_a-zA-Z0-9]*\\.(\\html)\">");

string readFile(const string& fileName) {
	ifstream f(fileName);
	string content((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
	f.close();
	return content;
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

void index(mutex& links_mutex, mutex& time_mutex, set <string>& opened_links, set <string>& links_to_open, chrono::system_clock::time_point& last_link_adding_time) {
	string cur_page = "";
	
	while (1) {
		links_mutex.lock();
		if (!links_to_open.empty()) {
			cur_page = *links_to_open.begin();
			links_to_open.erase(cur_page);
			opened_links.insert(cur_page);
			links_mutex.unlock();
		}
		else {
			links_mutex.unlock();
			this_thread::sleep_for(chrono::seconds(10));
			continue;
		}
		set <string> found_pages = open_page(cur_page);

		links_mutex.lock();
		for (auto page : found_pages) {
			if (opened_links.find(page) == opened_links.end() && links_to_open.find(page) == links_to_open.end()) {
				links_to_open.insert(page);
			}
		}
		cout << opened_links.size() << endl;
		links_mutex.unlock();
		if (!found_pages.empty()) {
			time_mutex.lock();
			last_link_adding_time = chrono::system_clock::now();
			time_mutex.unlock();
		}
	}
}

size_t start_indexing(string start_page, size_t threads_num) {
	set <string> opened_links;
	set <string> links_to_open;
	mutex links_mutex;
	mutex time_mutex;
	vector <thread> threads;
	auto last_link_adding_time = chrono::system_clock::now();
	links_to_open.insert(start_page);
	for (size_t i = 0; i < threads_num; i++) {
		threads.emplace_back(index, ref(links_mutex), ref(time_mutex), ref(opened_links), ref(links_to_open), ref(last_link_adding_time));
	}

	for (auto& t : threads)
	{
		t.detach();
	}


	while (1) {
		time_mutex.lock();
		chrono::duration<double> past_time = chrono::system_clock::now() - last_link_adding_time;
		time_mutex.unlock();
		if (past_time.count() > 3) {
			break;
		}
		this_thread::sleep_for(chrono::seconds(2));
	}
	return opened_links.size();
}



int main() {
	ifstream input("input.txt");
	ofstream output("output.txt");
	string page_address;
	size_t workers_num;
	input >> page_address >> workers_num;
	auto start = chrono::system_clock::now();
	auto cnt = start_indexing(page_address, workers_num);
	auto finish = chrono::system_clock::now();
	chrono::duration<double> time = finish - start;

	output << cnt << " " << time.count();
	input.close();
	output.close();
	return 0;
}
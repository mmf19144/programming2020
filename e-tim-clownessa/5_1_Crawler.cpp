#include <iostream>
#include <fstream>
#include <queue>
#include <set>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <ctime>
#include <direct.h>
#include <string>
using namespace std;

string dlpath = "0path/";
string href = "<a href = \"file://";
atomic<int> counter = 0;
bool notified = false;
bool done = false;
int thrWait = 0, thrs;
condition_variable cond;
queue<string> order;
mutex morder, mmarks;
set<string> marks;

string setNice(string fn) { 
	fn.erase(fn.begin(), fn.begin() + 7);
	return fn;
}

void bfs(int thrCnt) {
	string cur;
	if (thrCnt > 0) {
		thread next(bfs, thrCnt - 1);
		next.detach();
	}
	
	while (!done) {
		cout << counter << endl;
		unique_lock<mutex> locker(morder);
		if (order.empty() && thrWait == thrs - 1) {
			done = true;
			cond.notify_all();
		}

		thrWait++;
		while (!notified) {
			cond.wait(locker);
		}
		thrWait--;

		if (!order.empty() && !done) {
			cur = order.front();
			order.pop();
		}

		if (order.empty()) {
			notified = false;
		}

		counter++;
		cout << counter;
		char c;
		string str;
		ifstream fin(cur);
		ofstream fout(dlpath + cur);
		cout << 'a';
		while (!fin.eof()) {
			cout << 'a';
			getline(fin, str);
			fout << str;
			for (size_t i = str.find(href, 0); i != string::npos; i = str.find(href, i + 1)) {
				cur.clear();
				i = i + href.length();
				while (str[i] != '\"') {
					cur += str[i];
					i++;
				}
				cout << endl << cur << endl;
				mmarks.lock();
				if (marks.find(cur) == marks.end()) {
					cout << cur << endl;
					marks.insert(cur);
					cout << 'a';
					mmarks.unlock();
					//cout << morder.try_lock();
					morder.lock();
					cout << 'a';
					order.push(cur);
					cout << 'a';
					notified = true;
					cond.notify_one();
					morder.unlock();
				}
				else {
					mmarks.unlock();
				}

				cout << 'b';
			}
			fin.close();
			fout.close();
		}

		
	}
	return;
}

int main() {
	using namespace std::chrono;

	steady_clock::time_point t1 = steady_clock::now();
	_mkdir("0pack/");
	string str;
	cin >> str;
	str = setNice(str);
	order.push(str);
	marks.insert(str);
	cin >> thrs;
	thread thread1(bfs, thrs - 1);
	notified = true;
	

	steady_clock::time_point t2 = steady_clock::now();

	duration<double> dur = duration_cast<duration<double>>(t2 - t1);

	cout << counter << ' ' << dur.count() << '\n';
}

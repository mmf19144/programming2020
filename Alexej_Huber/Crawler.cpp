#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <chrono>
#include <thread>
#include <mutex>
#include <unordered_map>

using namespace std;

class Crawler
{
	const regex regexp_link{ R"(<a href="[^>]+">)" };
	const size_t max_threads;

	unordered_map<string, string> current_files;
	unordered_map<string, string> viewed_files;
	chrono::milliseconds timing{};

	vector<thread> threads;
	int work_threads = 0;
	mutex mut;

	pair<string, string> split(string str, string delimiter = "//")
	{
		size_t index = str.find(delimiter) + delimiter.length();
		string first = str.substr(0, index);
		string second = str.substr(index, str.length());
		return { first, second };
	}

	vector<string> get_links(string& data)
	{
		vector<string> pairs;
		smatch m;

		while (regex_search(data, m, regexp_link))
		{
			string temp = m.str();

			regex re{ R"(".*")" };
			smatch m1;

			regex_search(temp, m1, re);

			string temp1 = m1.str();
			auto pair_path = temp1.substr(1, temp1.length() - 2);
			pairs.push_back(pair_path);
			data = m.suffix();
		}
		return pairs;
	}

	void parsing()
	{
		mut.lock();
		if (current_files.empty())
		{
			mut.unlock();
			return;
		}
		work_threads++;
		string address = (*current_files.begin()).second;

		current_files.erase(current_files.begin());
		viewed_files.insert({ address, address });
		mut.unlock();

		auto [protocol, path] = split(address);
		if (protocol == "file://")
		{
			ifstream fin{ path };
			string data;
			getline(fin, data, (char)fin.eof());

			auto links = get_links(data);
			mut.lock();
			for (const auto& l : links)
			{
				if (viewed_files.find(l) == viewed_files.end())
				{
					current_files.insert({ l, l });
				}
			}
			mut.unlock();
		}
		work_threads--;
	}

	void start_parse()
	{
		while (!current_files.empty() || work_threads != 0)
		{
			parsing();
		}
	}

public:
	Crawler(const size_t max_threads = 6) : max_threads{ max_threads }
	{	}

	void run(string address)
	{
		auto start = chrono::high_resolution_clock::now();
		auto [protocol, path] = split(address);
		current_files.insert({ address, address });

		for (size_t i = 0; i < max_threads; i++)
		{
			threads.push_back(thread([&]() { start_parse(); }));
		}

		for (auto& t : threads)
		{
			t.join();
		}
		timing = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start);
	}

	pair<chrono::milliseconds, size_t> get_statistics()
	{
		return { timing, viewed_files.size() };
	}
};

int main()
{
	string address;
	cin >> address;

	Crawler crawler;
	crawler.run(address);
	auto [time2, count2] = crawler.get_statistics();
	cout << "Crawler 6 threads. Time - " << time2.count() << "ms, count pages - " << count2 << endl;
}

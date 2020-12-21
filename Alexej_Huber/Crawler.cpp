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
	const regex reg_modify{ "<a href=\"([^\"]*)\">" };
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

	vector<string> get_links(string & data)
	{
		vector<string> pairs;
		smatch m;
		for (auto i = sregex_iterator(data.begin(), data.end(), reg_modify); i != std::sregex_iterator(); i++)
		{
			std::smatch m = *i;
			pairs.push_back(m[1].str());
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
			for (const auto & l : links)
			{
				mut.lock();
				if (viewed_files.find(l) == viewed_files.end())
				{
					current_files.insert({ l, l });
				}
				mut.unlock();
			}
		}
		mut.lock();
		work_threads--;
		mut.unlock();
	}

	void start_parse()
	{
		mut.lock();
		while (!current_files.empty() || work_threads != 0)
		{
			mut.unlock();
			parsing();
			mut.lock();
		}
		mut.unlock();
	}

public:
	Crawler(const size_t max_threads = 8) : max_threads{ max_threads }
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
	string address = "file://0.html";
	cin >> address;

	Crawler crawler{8};
	crawler.run(address);
	auto [time2, count2] = crawler.get_statistics();
	cout << "Crawler 6 threads. Time - " << time2.count() << "ms, count pages - " << count2 << endl;
	
}

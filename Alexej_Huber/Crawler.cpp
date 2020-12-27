#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <chrono>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <condition_variable>
#include <future>

using namespace std;


class Crawler
{

	const regex reg_modify{ "<a href=\"([^\"]*)\">" };
	const size_t max_threads;

	unordered_map<string, string> current_files;
	unordered_map<string, string> viewed_files;
	chrono::milliseconds timing{};

	mutex mut;
	condition_variable cond_var;
	int work_threads = 0;

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

	void parsing(string address)
	{
		pair<string, string> pr = split(address);
		
		if (pr.first == "file://")
		{
			ifstream fin{ pr.second };
			string data;
			getline(fin, data, (char)fin.eof());

			auto links = get_links(data);
			for (const auto & l : links)
			{
				unique_lock<mutex> lock{ mut };
				if (viewed_files.find(l) == viewed_files.end())
				{
					current_files.insert({ l, l });
					cond_var.notify_one();
				}
			}
		}
		work_threads--;
		if (work_threads <= 0 && current_files.empty())
		{
			cond_var.notify_all();
		}
	}
	
	void start_parse()
	{
		{
			std::unique_lock<std::mutex> lk(mut);
			cout << "Thread enter: id: " << std::this_thread::get_id() << "\r\n";
		}
		do
		{
			string address;
			{
				std::unique_lock<std::mutex> lk(mut);
				if (current_files.empty() && work_threads > 0)
				{
					cond_var.wait(lk);
				}
				if (current_files.empty()) { break; }
				address = (*current_files.begin()).second;

				current_files.erase(current_files.begin());
				viewed_files.insert({ address, address });
				bFilesInQueue = !current_files.empty();
				work_threads++;
			}
			parsing(address);
		} while (true);

		{
			std::unique_lock<std::mutex> lk(mut);
			cout << "Thread exit: id: " << std::this_thread::get_id() << "\r\n";
		}
	}

public:
	Crawler(const size_t max_threads = 8) : max_threads{ max_threads }
	{	}

	void run(string address)
	{
		vector<future<void>> threads;

		auto start = chrono::high_resolution_clock::now();
		auto [protocol, path] = split(address);
		current_files.insert({ address, address });

		for (size_t i = 0; i < max_threads; i++)
		{
			threads.push_back(async(launch::async, [&]() { start_parse(); }));
		}

		for (auto & t : threads)
		{
			t.wait();
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

	Crawler crawler{ 8 };
	crawler.run(address);
	auto [time2, count2] = crawler.get_statistics();
	cout << "Crawler 8 threads. Time - " << time2.count() << "ms, count pages - " << count2 << endl;

}

#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <queue>
#include <set>
#include <string>
#include <fstream>

typedef std::set<size_t> Set;


class FA
{
	std::map<Set, std::map<char, Set>> matrix;
	std::set<Set> terminal_states;
	size_t start_state;

public:
	FA() : start_state{ 0 }
	{
		matrix = std::map<Set, std::map<char, Set>>{};
		terminal_states = std::set<Set>{};
	}

	FA(const FA & other)
		: matrix{ other.matrix }
		, terminal_states{ other.terminal_states }
		, start_state{ other.start_state }
	{	}

	friend std::istream & operator>>(std::istream & in, FA & automat)
	{
		size_t count_states;
		in >> count_states;

		in >> automat.start_state;

		size_t count_terminal_states;
		in >> count_terminal_states;
		for (size_t i = 0; i < count_terminal_states; i++)
		{
			size_t temp;
			in >> temp;
			automat.terminal_states.insert({ temp });
		}

		size_t count_transitions;
		in >> count_transitions;
		for (size_t i = 0; i < count_transitions; i++)
		{
			size_t from, to;
			char value;
			in >> from >> to >> value;
			automat.matrix[Set({ from })][value].insert(to);
		}
		return in;
	}

	// Очевидно, что не будет работать на недетерминированном автомате
	bool recognition(std::string str)
	{
		Set index = Set({ start_state });
		for (const auto & ch : str)
		{
			index = matrix[index][ch];
		}
		return terminal_states.find(index) != terminal_states.end();
		//return std::find(terminal_states.begin(), terminal_states.end(), index) != terminal_states.end();
	}

	void determine()
	{
		std::map<Set, std::map<char, Set>> matrix_1;

		std::queue<Set> q({ { start_state} });
		while (!q.empty())
		{
			Set front = q.front();
			q.pop();
			for (const auto& it : front)
			{
				for (const auto& it1 : matrix[{ it }])
				{
					for (const auto& it2 : it1.second)
					{
						matrix_1[front][it1.first].insert(it2);
					}
					if (matrix_1.find(matrix_1[front][it1.first]) == matrix_1.end())
					{
						q.push(matrix_1[front][it1.first]);
					}
				}
			}

		}

		std::set<Set> ts_1;
		for (const auto& it : matrix_1)
		{
			for (const auto& it1 : terminal_states)
			{
				if (it.first.find(*(it1.begin())) != it.first.end())
				// if (std::find(it.first.begin(), it.first.end(), *(it1.begin())) != it.first.end())
				{
					ts_1.insert(it.first);
					break;
				}
			}
		}

		for (const auto& state : terminal_states)
		{
			ts_1.insert(state);
		}

		matrix = matrix_1;
		terminal_states = ts_1;
	}
};


int main()
{
	std::ofstream fout{ "output.txt" };
	std::ifstream fin{ "input.txt" };

	FA automat;
	fin >> automat;
	int t;
	fin >> t;
	automat.determine();
	for (size_t i = 0; i < t; i++)
	{
		std::string tmp;
		fin >> tmp;
		if (automat.recognition(tmp))
		{
			fout << "YES" << std::endl;
		}
		else
		{
			fout << "NO" << std::endl;
		}
	}
}

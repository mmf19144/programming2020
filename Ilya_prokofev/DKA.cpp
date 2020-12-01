#include <iostream>
#include <unordered_map>
#include <set>
#include <vector>
#include <queue>
#include <string>
#include <fstream>
#include <algorithm>
using namespace std;

typedef unordered_map <string, bool> map_s;
typedef unordered_multimap <char, pair<string, string>> map_f;

vector<string> split(string input) {
	size_t cur_pos = 0, next;
	vector<string> result;
	if (input.find("_") == string::npos) {
		return { input };
	}
	while (input.find("_", cur_pos) != string::npos) {
		next = input.find("_", cur_pos);
		result.push_back(input.substr(cur_pos, next - cur_pos));
		cur_pos = next+1;
	}
	result.push_back(input.substr(cur_pos , input.length() - cur_pos));
	return result;
}
class FA{
	map_s states;
	string initial_state;
	map_f transition_functions;
public:
	FA() {}
	FA(const FA& that)
	{
		this->states = that.states;
		this->initial_state = that.initial_state;
		this->transition_functions = that.transition_functions;
	}
	set<char> alphabet(){
		set<char> result;
		map_f::iterator iter = transition_functions.begin();
		while (iter != transition_functions.end()){
			result.insert(iter->first);
			iter++;
		}
		return result;
	}
	istream& input_from_stream(istream& input){
		size_t n, f, tmp, p;
		string from, to;
		char letter;
		input >> n;
		input >> initial_state;
		for (size_t i = 0; i < n; i++){
			states.insert(map_s::value_type(to_string(i), false));
		}
		input >> f;
		for (size_t i = 0; i < f; i++){
			input >> tmp;
			states.erase(to_string(tmp));
			states.insert(map_s::value_type(to_string(tmp), true));
		}
		input >> p;
		for (size_t i = 0; i < p; i++){
			input >> from >> to >> letter;
			transition_functions.insert(map_f::value_type(letter, make_pair(from,to)));
		}
		return input;
	}
	FA determine(){
		FA result;
		result.initial_state = this->initial_state;
		set<char> alphabet = this->alphabet();
		queue<string> Q;
		Q.push(initial_state);
		result.states.insert(map_s::value_type(initial_state, this->states.find(this->initial_state)->second));
		while (!Q.empty()) {
			string top = Q.front();
			Q.pop();
			vector<string> states_list = split(top);
			for (auto c : alphabet) {
				set<string> states_res;
				for (auto state : states_list) {
					auto its = this->transition_functions.equal_range(c);
					for (auto it = its.first; it != its.second; ++it) {
						if (it->second.first == state) {
							states_res.insert(it->second.second);
						}
					}
				}
				if (states_res.empty()) {
					result.transition_functions.insert(map_f::value_type(c, make_pair(top, "empty_set")));

					if (result.states.find("empty_set") == result.states.end()) {
						for (auto ch : alphabet) {
							result.transition_functions.insert(map_f::value_type(ch, make_pair("empty_set", "empty_set")));
						}
						result.states.insert(map_s::value_type("empty_set", false));
					}
					continue;
				}
				string states_result_str = "";
				bool is_finite = false;
				vector<string> states_result_vec(states_res.begin(), states_res.end());
				sort(states_result_vec.begin(), states_result_vec.end());
				for (auto str : states_result_vec) {
					states_result_str += str + "_";
					if (!is_finite && this->states.find(str)->second) {
						is_finite = true;
					}
				}
				states_result_str = states_result_str.substr(0, states_result_str.length() - 1);
				if (result.states.find(states_result_str) == result.states.end()) {
					result.states.insert(map_s::value_type(states_result_str, is_finite));
					Q.push(states_result_str);
				}
				result.transition_functions.insert(map_f::value_type(c, make_pair(top, states_result_str)));
			}
		}
		return result;
	}
	string step(string state, char letter) {
		auto its = this->transition_functions.equal_range(letter);

		for (auto it = its.first; it != its.second; ++it) {
			if (it->second.first == state){
				return it->second.second;
			}
		}
		return ("empty");
	}
	bool recognition(string input) {
		string cur_state = initial_state;
		for (auto ch : input) {
			cur_state = step(cur_state, ch);
			if (cur_state == "empty_set") {
				return false;
			}
		}
		return (states.find(cur_state)->second);
	}
};

int main() {
	ifstream in("input.txt");
	ofstream out("output.txt");
	FA automat;
	automat.input_from_stream(in);
	automat = automat.determine();
	size_t n;
	string str;
	in >> n;
	for (size_t i = 0; i < n; i++) {
		in >> str;
		if (automat.recognition(str)) {
			out << "YES" << endl;
		}
		else {
			out << "NO" << endl;
		}
	}
	in.close();
	out.close();
	return 0;
}
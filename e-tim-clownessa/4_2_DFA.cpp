#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class DFA {
	string** data;
	bool* final;
	int init;
	int n;
public:
	DFA(int count, int init) {
		n = count;
		this->init = init;
		final = new bool[n];
		for (int i = 0; i < n; i++) {
			final[i] = 0;
			}
		data = new string*[n];
		for (int i = 0; i < n; i++) {
			data[i] = new string[n];
		}
	}
	
	~DFA() {
		for (int i = 0; i < n; i++)
			delete[] data[i];
		delete[] data;
		delete[] final;
	}
	
	void setFinal(ifstream& fin, int k) {
		int tmp;
		for (int i = 0; i < k; i++) {
			fin >> tmp;
			final[tmp] = 1;;
		}
	}

	void add(int from, int to, char c) {
		data[from][to].push_back(c);
	}

	bool hasVal(int from, int to, char c) {
		int l = data[from][to].length();
		for (int i = 0; i < l; i++) {
			if ((data[from][to])[i] == c)
				return 1;
		}
		return 0;
	}

	bool isValid(string str, ifstream& fin) {
		int len = str.length();
		int cur = init;
		string tmp;
		for (int i = 0; i < len; i++) {
			int flag = 0;
			for (int j = 0; j < n; j++) {
				if (hasVal(cur, j, str[i])) {
					cur = j;
					flag = 1;
					break;
				}
			}
			if (flag == 0)
				return 0;
			else
				flag = 0;
		}
		return final[cur];
	}
};

int main() {
	ifstream fin("input.txt");
	ofstream fout("output.txt");

	int init, n, k, from, to;
	char val;
	string str;

	fin >> n >> init >> k;

	DFA dfa(n, init);
	dfa.setFinal(fin, k);

	fin >> k;

	for (int i = 0; i < k; i++) {
		fin >> from >> to >> val;
		dfa.add(from, to, val);
	}

	fin >> k;

	for (int i = 0; i < k; i++) {
		fin >> str;
		if (dfa.isValid(str, fin))
			fout << "YES" << endl;
		else
			fout << "NO" << endl;
	}

	fin.close();
	fout.close();
}
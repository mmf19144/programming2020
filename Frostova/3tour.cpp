#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

template<class K, class V>
struct Bucket {
	K key;
	V val;
	Bucket(const K& k, const V& v) : val(v), key(k) {}
};

template<class K, class V >
class HashMap {
private:
	vector<vector<Bucket<K, V> > > table;
	size_t size;
	double max_load_factor;
	void rehash(int new_size) {
		vector<vector<Bucket<K, V> > > table0;
		table0.resize(new_size);
		swap(table, table0);
		size = 0;
		for (int i = 0; i < table0.size(); ++i) {
			for (int j = 0; j < table0[i].size(); ++j) {
				set(table0[i][j].key, table0[i][j].val);
			}
		}
	}
public:
	size_t getSize() const {
		return size;
	}

	HashMap(int size, double max_load_factor0) {
		max_load_factor = max_load_factor0;
		table.resize(size);
		size = 0;
	}

	~HashMap() {}

	void set(const K& k, const V& v) {
		Bucket<K, V> b(k, v);
		size_t index = hash(k);
		for (int i = 0; i < table[index].size(); i++) {
			if (table[index][i].key == k) {
				table[index][i] = b;
				return;
			}
		}
		table[index].push_back(b);
		++size;
		if (size > table.size()* max_load_factor) {
			rehash(table.size() * 2);
		}
	}

	V get(const K& k) {
		size_t index = hash(k);
		for (int i = 0; i < table[index].size(); i++)
			if (table[index][i].key == k)
				return table[index][i].val;
		return V();
	}

	void erase(const K& k) {
		size_t index = hash(k);
		for (int i = 0; i < table[index].size(); i++)
			if (table[index][i].key == k) {
				table[index][i] = table[index].back();
				table[index].pop_back();
				--size;
				return;
			}
	}

	bool exist(const K& k) const {
		size_t index = hash(k);
		for (int i = 0; i < table[index].size(); i++)
			if (table[index][i].key == k)
				return true;
		return false;
	}

	size_t hash(const K& k) const {
		std::hash<K> hashFunction;
		return hashFunction(k) % table.size();
	}

	class Iterator {
	private:
		size_t i, j;
		const vector<vector<Bucket<K, V> > >& table;
	public:
		Iterator(size_t i0, size_t j0, const vector<vector<Bucket<K, V> > >& table0) :
			i(i0),
			j(j0),
			table(table0)
		{}

		Bucket<K, V> operator *() {
			return table[i][j];
		}

		void operator ++() {
			if (j + 1 < table[i].size()) {
				++j;
			}
			else {
				do { 
					++i;
					//cout << i << endl;
					if (i == table.size())
						break;
				} while (table[i].empty());
				j = 0;
			}
		}
		bool operator !=(Iterator z) {
			return ((i!=z.i)||(j!=z.j));
		}
	};

	Iterator begin() const {
		for (int i = 0; i < table.size(); ++i) {
			if (!table[i].empty()) {
				return Iterator(i, 0, table);
			}
		}
		return Iterator(table.size(), 0, table);
	}

	Iterator end() const{
		return Iterator(table.size(), 0, table);
	}
};

template<class K, class V> int process() {
	int N;
	char c;
	K key;
	V val;
	HashMap<K, V> m(8, 0.5);
	ofstream cout("output.txt");
	ifstream cin("input.txt");
	char a, b;
	cin >> a >> b;
	cin >> N;
	for (int i = 0; i < N; ++i) {
		cin >> c >> key;
		if (c == 'A') {
			cin >> val;
			m.set(key, val);
		}
		else {
			m.erase(key);
		}
	}
	int count = 0;
	count = m.getSize();
	HashMap<V, int> m0(8, 0.5);

	for (auto it = m.begin(); it != m.end(); ++it) {
		Bucket<K, V> b = *it;
		m0.set(b.val, 0);
	}
	int count0 = m0.getSize();
	cout << count << ' ' << count0;

	return 0;
}

int main() {
	ifstream cin("input.txt");
	char k, v;
	cin >> k >> v;
	switch (k) {
	case 'I':
		switch (v) {
		case 'I': return process<int, int>();
		case 'D': return process<int, double>();
		case 'S': return process<int, string>();
		}
	case 'D':
		switch (v) {
		case 'I': return process<double, int>();
		case 'D': return process<double, double>();
		case 'S': return process<double, string>();
		}
	case 'S':
		switch (v) {
		case 'I': return process<string, int>();
		case 'D': return process<string, double>();
		case 'S': return process<string, string>();
		}
	}
	return 0;
}

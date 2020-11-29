#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <utility>
#include <set>

using namespace std;

template <typename K, typename V>
struct Element {
	pair<K, V> value;
	Element* next_ptr;
};

template <typename K, typename V>
Element<K, V>* get_el(K key, V value) {
	Element<K, V>* n_el = new Element<K, V>;
	n_el->value = make_pair(key, value);
	n_el->next_ptr = nullptr;
	return n_el;
}

template <typename K, typename V>
class HashMap {
protected:
	friend class Iterator;
	Element<K, V>** table;
	unsigned int h_size = 13;
	unsigned int curr_element_number = 0;
	double occupancy_limit = 0.7;

	size_t get_hash(K key, unsigned int mod) const noexcept {
		hash<K> hash_func;
		return hash_func(key) % mod;
	}

	bool add(Element<K, V>** my_table, K key, V value, unsigned int mod) {
		size_t hash = get_hash(key, mod);
		Element<K, V>* n_ptr = get_el<K, V>(key, value);

		if (my_table[hash] != nullptr) {
			Element<K, V>* last_ptr = my_table[hash];
			while (last_ptr->next_ptr != nullptr || last_ptr->value.first == key) {
				if (last_ptr->value.first == key) {
					last_ptr->value.second = value;
					delete n_ptr;
					return false;
				}

				last_ptr = last_ptr->next_ptr;
			}
			last_ptr->next_ptr = n_ptr;

		}
		else { my_table[hash] = n_ptr; }
		return true;
	}

	void clean() {
		for (unsigned int i = 0; i < h_size; i++) {
			for (Element<K, V>* ptr = table[i], *del_ptr = ptr; ptr != nullptr; ) {
				ptr = ptr->next_ptr;
				delete del_ptr;
				del_ptr = ptr;
			}
		}
		delete[] table;
	}

	void update() {
		Element<K, V>** new_table = new Element<K, V> * [h_size * 2];
		for (int i = 0; i < h_size * 2; i++)
			new_table[i] = nullptr;
		for (typename HashMap<K, V>::Iterator curr_iter = this->begin(); curr_iter != this->end(); ++curr_iter) {
			this->add(new_table, curr_iter->first, curr_iter->second, h_size * 2);
		}

		clean();
		h_size *= 2;
		table = new_table;


	}

	void occupancy_check() noexcept {
		if ((double)curr_element_number / h_size > occupancy_limit) {
			this->update();
		}
	}



public:

	HashMap() : HashMap(0.7, 13) {}  //
	HashMap(int) : HashMap() {}
	HashMap(double custom_occupancy_limit, unsigned int custom_h_size) : occupancy_limit(custom_occupancy_limit)
		, h_size(custom_h_size) {
		table = new Element<K, V> * [h_size];
		for (int i = 0; i < h_size; i++)
			table[i] = nullptr;
	}



	class Iterator {
	private:
		Element<K, V>** iter_table;
		Element<K, V>* iter;
		unsigned int h_size;
		unsigned int curr_h;

		void copy_func(const Iterator& copying) {
			iter_table = copying.iter_table;
			iter = copying.iter;
			h_size = copying.h_size;
			curr_h = copying.curr_h;
		}

	public:
		Iterator() : iter(nullptr) {}
		Iterator(int) : iter(nullptr) {}
		Iterator(Element<K, V>** table, Element<K, V>* el, unsigned int size, unsigned int idx) : iter_table(table)
			, iter(el)
			, h_size(size)
			, curr_h(idx) {}

		Iterator(const Iterator& copying) {
			copy_func(copying);
		}

		Iterator& operator++() {
			if (iter != nullptr) {
				if (iter->next_ptr != nullptr) {
					iter = iter->next_ptr;
				}
				else {
					iter = nullptr;
					curr_h++;
					while (iter == nullptr && curr_h < h_size) {
						iter = iter_table[curr_h];
						if (iter == nullptr)
							curr_h++;
					}
				}
			}
			return *this;
		}




		Iterator& operator=(Iterator& copying) {
			if (this != &copying) {
				copy_func(copying);
			}
			return *this;
		}

		bool operator!=(const Iterator& other) const {
			return iter != other.iter;
		}

		bool operator==(const Iterator& other) const {   //copypaste oops
			return !(iter != other.iter);
		}

		pair<K, V>* operator->() {
			return &(iter->value);
		}

		

	};

	Iterator begin() const {
		Element<K, V>* curr_begin = table[0];
		int idx = 0;
		while (curr_begin == nullptr && idx < h_size) {
			curr_begin = table[idx];
			idx++;

		}
		if (idx > 0) idx--;

		return Iterator(table, curr_begin, h_size, idx);
	}

	Iterator end() const {
		return Iterator();
	}

	HashMap<K, V>::Iterator value_search(K key) {
		size_t hash = get_hash(key, h_size);
		Element<K, V>* ptr = table[hash];
		while (ptr != nullptr && ptr->value.first != key) {
			ptr = ptr->next_ptr;
		}
		if (ptr == nullptr) {
			return Iterator();
		}
		return Iterator(table, ptr, h_size, hash);

	}

	void add_value(K key, V value) {
		if (add(table, key, value, h_size)) {
			curr_element_number++;
			occupancy_check();
		}
	}


	unsigned int get_num() const noexcept {
		return curr_element_number;
	}

	void value_delete(K key) {
		size_t hash = get_hash(key, h_size);
		Element<K, V>* ptr = table[hash], * prev = ptr;
		for (; ptr != nullptr && ptr->value.first != key; prev = ptr, ptr = ptr->next_ptr);
		if (ptr != nullptr) {
			if (ptr == table[hash]) {
				table[hash] = ptr->next_ptr;
			}
			else {
				prev->next_ptr = ptr->next_ptr;
			}

			delete ptr;
			curr_element_number--;
		}
	}

	~HashMap() {
		clean();
	}

};

template<typename K, typename V>
void data_read() {
	HashMap<K, V> hashmap;
	unsigned int comands_num = 0;
	cin >> comands_num;
	K key;
	V value;
	for (unsigned int i = 0; i < comands_num; i++) {
		char curr_comand;
		cin >> curr_comand;
		if (curr_comand == 'A') {
			cin >> key >> value;
			hashmap.add_value(key, value);
		}
		else {
			cin >> key;
			hashmap.value_delete(key);
		}

		

	}

	set<V> values;
	for (typename HashMap<K, V>::Iterator curr_iter = hashmap.begin(); curr_iter != hashmap.end(); ++curr_iter) {
		values.insert(curr_iter->second);
	}
	cout << hashmap.get_num() << " " << values.size();
	 
}


template <typename T>
void read_needed_type(T _) {
	char value_type = 0;
	cin >> value_type;

	switch (value_type) {
	case 'I': {
		data_read<T, int>();
		break;
	}
	case 'D': {
		data_read<T, double>();
		break;
	}
	case 'S': {
		data_read<T, string>();
		break;
	}
	}
}




int main() {
	freopen("input.txt", "r", stdin);
	freopen("output.txt", "w", stdout);
	char key_type, value_type;
	cin >> key_type;
	switch (key_type)
	{
	case 'I':
		read_needed_type(int(0));
		break;
	case 'D':
		read_needed_type(double(0.0));
		break;
	case 'S':
		read_needed_type(string());
		break;
	}
	

	return 0;
}
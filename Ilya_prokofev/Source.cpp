#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>

using namespace std;

template < typename K, typename V>
struct node {
	K key;
	V value;
	node* next;
	node(K k, V v, node* next) : key(k), value(v), next(next) {}
};
template < typename K, typename V>
struct list {
	node<K,V>* head;
	list() : head(nullptr) {}
	void push(K k, V v) {
		node<K, V>* new_node = new node <K, V>(k, v, head);
		head = new_node;
	}
	size_t size() {
		size_t cnt = 0;
		node<K, V>* tmp = head;
		while (tmp) {
			cnt++;
			tmp = tmp->next;
		}
		return cnt;
	}
	vector<pair<K, V>> get_pairs() {
		node<K, V>* tmp = head;
		vector<pair<K, V>> res;
		while (tmp) {
			res.push_back(make_pair(tmp->key, tmp->value));
			tmp = tmp->next;
		}
		return res;
	}
	void rewrite(K k, V v) {
		node<K, V>* tmp = head;
		while (tmp != nullptr) {
			if (tmp->key == k) {
				tmp->value = v;
				return;
			}
			tmp = tmp->next;
		}
		push(k, v);
	}
	void remove(K key)
	{
		node<K, V>* current = this->head;
		node<K, V>* prev = nullptr;

		while (current){
			if (current->key == key){
				if (prev){
					prev->next = current->next;
				}
				else{
					this->head = current->next;
				}
				delete current;
				return;
			}

			prev = current;
			current = current->next;
		}
	}
	V get(K k) {
		node<K, V>* tmp = head;
		while (tmp) {
			if (tmp->key == k) {
				return tmp->value;
			}
			tmp = tmp->next;
		}
		throw invalid_argument("node with given key doesnt exist");
	}
	size_t get_index_by_key(K key){
		node<K, V>* tmp = this->head;
		size_t result = 0;
		while (tmp){
			if (tmp->key == key){
				return result;
			}

			tmp = tmp->next;
			result++;
		}
		throw invalid_argument("Element with given key doesn't exist.");
	}
	K get_key(size_t index){
		node<K, V>* tmp = this->head;
		while (tmp){
			if (index == 0){
				return tmp->key;
			}

			tmp = tmp->next;
			index--;
		}

		throw out_of_range("Given index is greater than list size.");
	}
	V get_value(size_t index){
		node<K, V>* tmp = this->head;

		while (tmp){
			if (index == 0){
				return tmp->value;
			}

			tmp = tmp->next;
			index--;
		}
		throw out_of_range("Given index is greater than list size.");
	}
	~list() {
		node<K, V>* prev = nullptr;
		node<K, V>* current = head;
		while (current) {
			prev = current;
			current = current->next;
			delete prev;
		}
	}
};
template < typename K, typename V>
class HashMap {
	vector <list<K, V>*> arr;
	hash <K> hasher;
	double load_factor;
	size_t elements_to_resize;
	HashMap(double load_factor, size_t arr_size) : load_factor(load_factor) {
		arr.resize(arr_size);
		fill(arr.begin(), arr.end(), nullptr);
		elements_to_resize = (size_t)(load_factor * arr.size()) + 1;
	}
public:
	HashMap() {
		load_factor = 0.75;
		arr.resize(10);
		fill(arr.begin(), arr.end(), nullptr);
		elements_to_resize = (size_t)(load_factor * arr.size()) + 1;
	}
	HashMap(double load_factor) : load_factor(load_factor) {
		arr.resize(10000);
		fill(arr.begin(), arr.end(), nullptr);
		elements_to_resize = (size_t)(load_factor * arr.size()) + 1;
	}
	void clear() {
		for (size_t i = 0; i < arr.size(); i++) {
			if (arr[i]) {
				delete arr[i];
				arr[i] = nullptr;
			}
		}
	}
	~HashMap() {
		clear();
	}
	size_t size() {
		size_t cnt = 0;
		for (size_t i = 0; i < arr.size(); i++) {
			if (arr[i]) {
				cnt += arr[i]->size();
			}
		}
		return cnt;
	}
	bool need_to_rehash() {
		return (this->size() >= elements_to_resize);
	}
	void rehash() {
		HashMap new_map (load_factor, 2*arr.size());
		for (auto a : *this) {
			new_map.add(a.key(), a.value());
		}
		this->arr.swap(new_map.arr);
	}
	void add(K k, V v) {
		size_t i = get_index(k);
		if (arr[i]) {
			arr[i]->rewrite(k, v);
		}
		else {
			arr[i] = new list<K,V>();
			arr[i]->push(k, v);
		}
		if (need_to_rehash()) {
			rehash();
		}
	}
	size_t get_index(K k) {
		return hasher(k) % arr.size();
	}
	void remove(K k) {
		size_t i = get_index(k);
		if (arr[i]) {
			arr[i]->remove(k);
			if (!arr[i]->head) {
				delete arr[i];
				arr[i] = nullptr;
			}
		}
	}
	class Iterator{
		int idx; 
		typename vector<list<K, V>*>::iterator current;
		typename vector<list<K, V>*>::iterator end;

	public:
		Iterator(size_t idx, typename vector<list<K, V>*>::iterator i, typename vector<list<K, V>*>::iterator e) : idx(idx), current(i), end(e) {}

		Iterator operator++(int){
			if (this->idx == -1){
				throw out_of_range("Can't increment map iterator past the end.");
			}

			Iterator this_copy = *this;

			if (this->idx < (int)(*(this->current))->size() - 1){
				*this = Iterator(this->idx + 1, this->current, this->end);
				return this_copy;
			}

			for (typename vector<list<K, V>*>::iterator i = this->current + 1; i != this->end; i++){
				if (*i){
					*this = Iterator(0, i, this->end);
					return this_copy;
				}
			}

			*this = Iterator(-1, this->end, this->end); // this is the end
			return this_copy;
		}

		Iterator& operator++(){
			(*this)++;
			return *this;
		}

		friend bool operator==(const Iterator& one, const Iterator& two){
			return (one.current == two.current) && (one.idx == two.idx);
		}

		friend bool operator!=(const Iterator& one, const Iterator& two){
			return !(one == two);
		}

		K key() const{
			if (this->idx != -1 && *(this->current)){
				return (*(this->current))->get_key(this->idx);
			}
			throw out_of_range("Can't dereference iterator over an empty collection.");
		}

		V value() const{
			if (this->idx != -1 && *(this->current)){
				return (*(this->current))->get_value(this->idx);
			}
			throw out_of_range("Can't dereference iterator over an empty collection.");
		}

		Iterator operator*(){
			return *this;
		}
	};
	Iterator begin()	{
		// find first non-empty list
		for (size_t i = 0; i < this->arr.size(); i++){
			if (this->arr[i]){
				return Iterator(0, this->arr.begin() + i, this->arr.end());
			}
		}
		return Iterator(0, this->arr.begin(), this->arr.begin());
	}

	Iterator end(){
		for (size_t i = 0; i < this->arr.size(); i++){
			if (this->arr[i]){
				return Iterator(-1, this->arr.end(), this->arr.end());
			}
		}
		return Iterator(0, this->arr.begin(), this->arr.begin());
	}
	size_t size() const{
		return this->map_size;
	}

	Iterator get(K key){
		size_t index = this->get_index(key);
		if (!this->arr[index]){
			return this->end();
		}
		size_t list_idx = this->arr[index]->get_index_by_key(key);
		return Iterator(list_idx, this->arr.begin() + index, this->arr.end());
	}
};


template<typename K,typename V>
void execute_commands(istream& input, size_t commands_number){
	HashMap<K, V> map;
	K key;
	V value;
	char command;

	for (size_t i = 0; i < commands_number; i++){
		input >> command;

		switch (command){
		case 'R':
			input >> key;
			map.remove(key);
			break;
		case 'A':
			input >> key;
			input >> value;
			map.add(key, value);
			break;
		default:
			break;
		}
	}
	HashMap <V, K> result;
	for (auto a : map) {
		result.add(a.value(), a.key());
	}
	cout << map.size() << " " << result.size() << endl;
}

template <typename T>
void execute_commands_depending_on_type(istream& input, size_t commands_number, char v_type){
	switch (v_type){
	case 'I':
		execute_commands<T, int>(input, commands_number);
		break;
	case 'D':
		execute_commands<T, double>(input, commands_number);
		break;
	case 'S':
		execute_commands<T, string>(input, commands_number);
		break;
	default:
		throw invalid_argument("Incorrect value type.");
	}
}

int main(){
	ifstream input("input.txt");
	char k_type, v_type;
	size_t commands_number;

	input >> k_type >> v_type;
	input >> commands_number;

	switch (k_type){
	case 'I':
		execute_commands_depending_on_type<int>(input, commands_number, v_type);
		break;
	case 'D':
		execute_commands_depending_on_type<double>(input, commands_number, v_type);
		break;
	case 'S':
		execute_commands_depending_on_type<string>(input, commands_number, v_type);
		break;
	default:
		throw invalid_argument("Incorrect key type.");
	}

	input.close();

	return 0;
}

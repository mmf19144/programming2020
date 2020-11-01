#include <iostream>
#include <fstream>
#include <cstring>
#include <unordered_map>
#include <cassert>

using namespace std;

template<typename T>
class LinkedList {
private:
    struct Node {
        T element;
        Node *next{};
    };
    Node *head;

    Node *createNode(T const &value) {
        Node *node = new Node;
        node->next = nullptr;
        node->element = value;
        return node;
    }

public:
    LinkedList() {
        head = nullptr;
    };

    ~LinkedList() {
        clear();
    }

    void clear() {
        if (!isNull()) {
            Node *ptr = head;
            while (ptr->next != nullptr) {
                Node *node_to_delete = ptr;
                ptr = ptr->next;
                delete (node_to_delete);
            }
        }
        head = nullptr;
    }

    explicit LinkedList(T &element) {
        head = createNode(element);
    }

    bool isNull() {
        return head == nullptr;
    }

    void pushFront(T const &element) {
        Node *h = head;
        head = createNode(element);
        head->next = h;
    };

    T getFront() {
        return head->element;
    }

    class Iterator {
    private:
        friend class LinkedList<T>;

    public:
        Node *curr;

        Iterator() {
            curr = nullptr;
        }

        Iterator(LinkedList<T>::Iterator const &that) {
            this->curr = that.curr;
        }

        explicit Iterator(LinkedList<T> const &list) {
            curr = list.head;
        }

        T &operator*() {
            assert(this->curr != nullptr);
            return this->curr->element;
        }

        Iterator &operator++() {
            if (curr == nullptr) {
                curr = nullptr;
            } else {
                curr = curr->next;
            }
            return *this;
        }

        bool operator!=(LinkedList<T>::Iterator that) const {
            return (this->curr != that.curr);
        }

        bool operator==(Iterator const &that) const {
            return (this->curr == that.curr);
        }

        bool isNull() {
            return curr == nullptr;
        }

    };

    Iterator begin() {
        Iterator it(*this);
        return it;
    };

    Iterator end() {
        Iterator it;
        it.curr = nullptr;
        return it;
    };

    bool search(T const &element) {
        for (Iterator it = this->begin(); it != this->end(); ++it) {
            if ((*it) == element) {
                return true;
            }
        }
        return false;
    };

    bool deleteByValue(T const &element) {
        if ((*begin()) == element) {
            Node *node_to_delete = head;
            head = head->next;
            delete (node_to_delete);
            return true;
        }
        if (begin() == end()) {
            return false;
        }

        auto prev = begin();
        auto iter = begin();
        iter++;
        while (iter != end()) {
            if ((*iter) == element) {
                prev.curr->next = iter.curr->next;
                delete (iter.curr);
                return true;
            }
            iter++;
            prev++;
        }
        if ((*iter) == element) {
            prev.curr->next = nullptr;
            delete (iter.curr);
        }
    }

    void setHead(Node *new_head) {
        head = new_head;
    }
};

template<typename Key, typename Value>
class HashMap {
    size_t size;
    size_t capacity;
    const double rehash_value;

    size_t calcHash(Key &key) {
        return std::hash<Key>{}(key) % size;
    }

    void rehash() {
        HashMap<Key, Value> newHashmap(size * 2, rehash_value);
        for (auto iter = this->begin(); iter != this->end(); ++iter) {
            newHashmap.insert((*iter).first, (*iter).second);
        }
        auto temp = data;
        this->data = newHashmap.data;
        newHashmap.data = temp;

        this->size *= 2;
        newHashmap.size /= 2;
    }

public:
    LinkedList<pair<Key, Value>> *data;

    HashMap(size_t size, double rehash_value)
            : data(new LinkedList<pair<Key, Value>>[size]),
              size(size), capacity(0), rehash_value(rehash_value) {
        for (size_t i = 0; i < size; i++) {
            data[i] = LinkedList<pair<Key, Value>>();
        }
    }

    ~HashMap() {
        for (size_t i = 0; i < size; i++) {
            data[i].clear();
        }
        delete[] data;
    }

    void insert(Key &key, Value &val) {
        size_t hash = calcHash(key);
        if (data[hash].search({key, val}))
            return;

        data[hash].pushFront({key, val});
        capacity++;
        checkRehash();
    }

    void checkRehash() {
        if ((double) capacity / (double) size >= 0.75) {
            rehash();
        }
    }

    void deleteByKey(Key &key) {
        size_t hash = calcHash(key);
        auto iter = data[hash].begin();
        if (iter.isNull()) return;

        while (!iter.isNull() && (*iter).first == key) {
            data[hash].setHead(iter.curr->next);
            delete (iter.curr);
            iter = data[hash].begin();
            capacity--;
        }
        auto prev = iter;
        ++iter;
        while (!iter.isNull()) {
            if ((*iter).first == key) {
                prev.curr->next = iter.curr->next;
                auto node_to_delete = iter.curr;
                ++iter;
                prev = iter;
                ++iter;
                delete node_to_delete;
                capacity--;
            } else {
                ++iter;
                ++prev;
            }

        }
    }

    Value getValueByKey(Key &key) {
        size_t hash = calcHash(key);
        return data[hash].getFront().second;
    }

    size_t getNumOfElements(Key &key) {
        size_t hash = calcHash(key);
        size_t count = 0;
        for (auto iter = data[hash].begin(); iter != data[hash].end(); iter++) {
            if ((*iter).first == key)
                count++;
        }
        return count;
    }

    Value *getAllValuesByKey(Key &key) {
        size_t hash = calcHash(key);
        size_t count = getNumOfElements(key);
        auto *val_arr = new Value[count];
        size_t id = 0;
        for (auto iter = data[hash].begin(); iter != data[hash].end(); iter++) {
            if ((*iter).first == key) {
                val_arr[id] = *iter;
                id++;
            }
        }
        return val_arr;
    }

    void replace(Key &key, Value &val) {
        deleteByKey(key);
        insert(key, val);
    }

    class Iterator {
    private:
        friend class HashMap<Key, Value>;
        LinkedList<pair<Key, Value>> *data;
        typename LinkedList<pair<Key, Value>>::Iterator list_iter;
        size_t data_id;
        size_t data_size;

    public:
        Iterator() = default;

        pair<Key, Value> &operator*() {
            assert(!list_iter.isNull());
            return (*list_iter);
        }

        Iterator &operator++() {
            ++list_iter;
            if (list_iter.isNull()) {
                data_id++;
                while (data_id < data_size && data[data_id].isNull())
                    data_id++;
                if (data_id != data_size)
                    list_iter = data[data_id].begin();
            }
            return *this;
        }

        bool operator!=(Iterator const &that) const {
            if (data_id != that.data_id) return true;
            return (this->list_iter != that.list_iter);
        }

        bool operator==(Iterator const &that) const {
            return (this->list_iter == that.list_iter);
        }
    };

    Iterator begin() {
        if (capacity == 0)
            return end();
        Iterator it;
        it.data = data;
        it.data_size = size;
        for (size_t id = 0; id < size; id++) {
            if (!data[id].isNull()) {
                it.data_id = id;
                it.list_iter = data[id].begin();
                break;
            }
        }
        return it;
    }

    Iterator end() {
        Iterator it;
        it.data = data;
        it.data_size = size;
        it.data_id = size;
        return it;
    }

    size_t countValues() {
        return capacity;
    }

    size_t countUniqueValues() {
        if (countValues()==0)
            return 0;
        HashMap<Value, Value> map(size, rehash_value);
        for (auto iter = this->begin(); iter != this->end(); ++iter) {
            map.insert((*iter).second, (*iter).second);
        }
        return map.countValues();
    }
};


template<typename Key, typename Value>
void run(istream &input, ostream &output) {
    size_t n;
    input >> n;
    char action;
    HashMap<Key, Value> hashMap(100, 0.75);
    for (size_t i = 0; i < n; i++) {
        input >> action;
        Key key;
        Value val;
        if (action == 'A') {
            input >> key >> val;
            hashMap.replace(key, val);
        }
        if (action == 'R') {
            input >> key;
            hashMap.deleteByKey(key);
        }
    }
    output << hashMap.countValues() << " " << hashMap.countUniqueValues();
}

int main() {
    char key_type, val_type;
    std::ifstream input("input.txt");
    std::ofstream output("output.txt");

    input >> key_type >> val_type;

    if (key_type == 'I' && val_type == 'S') {
        run<int, string>(input, output);
    } else if (key_type == 'I' && val_type == 'D') {
        run<int, double>(input, output);
    } else if (key_type == 'D' && val_type == 'S') {
        run<double, string>(input, output);
    } else if (key_type == 'D' && val_type == 'I') {
        run<double, int>(input, output);
    } else if (key_type == 'S' && val_type == 'I') {
        run<string, int>(input, output);
    } else if (key_type == 'S' && val_type == 'D') {
        run<string, double>(input, output);
    } else if (key_type == 'I' && val_type == 'I') {
        run<int, int>(input, output);
    } else if (key_type == 'D' && val_type == 'D') {
        run<double, double>(input, output);
    } else if (key_type == 'S' && val_type == 'S') {
        run<string, string>(input, output);
    }


    return 0;
}
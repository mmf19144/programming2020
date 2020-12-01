#include <iostream>
#include <fstream>
#include <limits>
#include <memory>
#include <algorithm>
#include <exception>
#include <iterator>
#include <memory>
#include <cassert>



//#define ADDITIONAL_TASK
#ifdef ADDITIONAL_TASK
#include "Matrix.cpp"
#endif

template<typename T>
class List {
private:
    class Node {
    private:
        T value;
        std::unique_ptr<Node> next;
    public:
        ~Node() = default;

        Node() : next(nullptr), value(nullptr) {}

        Node(T &v, std::unique_ptr<Node> n) : value(v), next(std::move(n)) {}

        Node *getNextRawPtr() {
            return next.get();
        }

        std::unique_ptr<Node> getNext() {
            return std::move(next);
        }

        T &getValue() {
            return value;
        }

        void setNext(std::unique_ptr<Node> n) {
            next = std::move(n);
        }


    };

    std::unique_ptr<List<T>::Node> head;
public:

    List() : head(nullptr) {}

    ~List() {
        for (std::unique_ptr<Node> current = std::move(head);
             current;
             current = std::move(current->getNext()));
    }

    void pushFront(T v) {
        head = std::make_unique<List<T>::Node>(v, std::move(head));
    }

    bool isEmpty() {
        return this->head == nullptr;
    }

    T getFront() {
        return head->getValue();
    }

    void popFront() {
        head = std::move(head->getNext());
    }

    class Iterator : public std::iterator<std::input_iterator_tag, T> {
    private:
        friend class List<T>;

        Node *current;
    public:
        Iterator() : current(nullptr) {}

        explicit Iterator(List<T> &l) : current(l.head.get()) {}

        Iterator(List<T>::Iterator const &other) : current(other.current) {}

        T &operator*() {
            return this->current->getValue();
        }


        Iterator operator++() {
            if (current) {
                current = current->getNextRawPtr();
            }
            return *this;
        }

        Iterator operator--() {
            if (current)
                current--;
            return *this;
        }

        bool operator==(List<T>::Iterator const &other) {
            return this->current == other.current;
        }

        bool operator!=(List<T>::Iterator const &other) {
            return this->current != other.current;
        }

        Node *getCurrentPtr() { return current; }
    };

    List<T>::Iterator begin() {
        return List<T>::Iterator(*this);
    }

    List<T>::Iterator end() {
        return List<T>::Iterator();
    }

    template<typename Key>
    bool isExist(Key val) {
        return std::any_of(begin(), end(),
                           [&val](auto v) { return v.first == val; }); // не сильно хорошо, что зашиваю логику в класс
    }

    void popByValue(T val) {
        for (auto it = begin(), it_end = end(); it != it_end; ++it) {
            if ((*it).first == val.first) {
                if (it == begin()) {
                    popFront();
                } else {
                    Node *prev = (--it).getCurrentPtr();
                    prev->setNext(std::move(it.getCurrentPtr()->getNext()));
                }
                return;
            }
        }
    }

    template<typename K, typename V>
    T &operator[](std::pair<K, V> const &val) {
        for (auto it = begin(), it_end = end(); it != it_end; ++it) {
            if ((*it).first == val.first)
                return *it;
        }
        throw std::invalid_argument("Key doesn't exists");
    }
};


template<typename Key, typename Value>
class hash_map {
private:
    List<std::pair<Key, Value>> *data;
    float rehash_value;
    size_t size;
    size_t capacity = 0;

    void rehash() {
        hash_map<Key, Value> new_table(size * 2, rehash_value);
        for (const auto &it: *this) {
            new_table.insert(it.first, it.second);
        }
        std::swap(data, new_table.data);
        capacity = new_table.capacity;
        size = new_table.size;
    }

    size_t hash(Key k) {
        return std::hash<Key>()(k) % size;
    }

public:
    hash_map(size_t size_, float rehash) : rehash_value(rehash),
                                           size(size_),
                                           data(new List<std::pair<Key, Value>>[size_]) {
        assert(data != nullptr);
    }

    ~hash_map() {
        delete[] data;
    }

    void insert(Key k, Value v) {
        size_t key_hash = hash(k);
        std::pair<Key, Value> a(k, v);
        if (data[key_hash].isExist(k)) {
            data[key_hash][a].second = v;
        } else {
            capacity++;
            data[key_hash].pushFront(a);
        }

        if ((float) capacity / size >= rehash_value)
            rehash();
    }

    void delKey(Key k) {
        size_t key_hash = hash(k);
        if (data[key_hash].isExist(k)) {
            std::pair<Key, Value> p(k, operator[](k));
            data[key_hash].popByValue(p);
            if (data[key_hash].isEmpty())
                capacity--;
        }
    }

    Value &operator[](Key k) {
        size_t key_hash = hash(k);

        for (auto it = data[key_hash].begin(), it_end = data[key_hash].end();
             it != it_end; ++it) {
            if ((*it).first == k)
                return (*it).second;
        }


        throw std::invalid_argument("Key doesn't exists");
    }

    class Iterator : public std::iterator<std::input_iterator_tag, std::pair<Key, Value>> {
    private:
        friend class hash_map<Key, Value>;

        List<std::pair<Key, Value>> *cur_ptr;
        List<std::pair<Key, Value>> *end_ptr;

        typename List<std::pair<Key, Value>>::Iterator list_iter;
        typename List<std::pair<Key, Value>>::Iterator end_list_iter;

    public:
        Iterator() : cur_ptr(nullptr), end_ptr(nullptr) {}

        Iterator(List<std::pair<Key, Value>> *cur_, List<std::pair<Key, Value>> *end_) :
                cur_ptr(cur_), end_ptr(end_), list_iter(cur_->begin()), end_list_iter(cur_->end()) {}

        Iterator(hash_map<Key, Value>::Iterator const &other) : cur_ptr(other.cur_ptr),
                                                                end_ptr(other.end_ptr),
                                                                list_iter(other.list_iter),
                                                                end_list_iter(other.end_list_iter) {}

        std::pair<Key, Value> &operator*() {
            return *list_iter;
        }

        Iterator operator++() {
            if (cur_ptr != nullptr) {
                ++list_iter;
                if (list_iter == end_list_iter) {
                    cur_ptr++;
                    while (cur_ptr != end_ptr && cur_ptr->isEmpty()) {
                        cur_ptr++;
                    }
                    if (cur_ptr != end_ptr) {
                        list_iter = cur_ptr->begin();
                        end_list_iter = cur_ptr->end();
                    } else {
                        cur_ptr = nullptr;
                    }
                }
            }
            return *this;
        }

        bool operator==(hash_map<Key, Value>::Iterator const &other) {
            return (cur_ptr == other.cur_ptr && list_iter == other.list_iter);
        }

        bool operator!=(hash_map<Key, Value>::Iterator const &other) {
            return !operator==(other);
        }

    };

    hash_map<Key, Value>::Iterator begin() {
        for (size_t i = 0; i < size; i++) {
            if (!data[i].isEmpty()) {
                return hash_map<Key, Value>::Iterator(&data[i], &data[size]);
            }
        }
        return end();
    }

    hash_map<Key, Value>::Iterator end() {
        return hash_map<Key, Value>::Iterator();
    }

    size_t getCapacity() {
        return capacity;
    }
};

#ifdef ADDITIONAL_TASK
namespace std {
    template<>
    struct hash<Matrix> {
        size_t operator()(const Matrix &m) const {
            long sum = 0;
            for (size_t i = 1; i <= m.getSize(); i++) {
                for (size_t j = 1; j <= m.getSize(); j++) {
                    sum += std::hash<int>{}(m[i][j]);
                }
            }
            return sum % std::numeric_limits<size_t>::max();
        }
    };
}
#endif

template<typename Key, typename Value>
size_t getCntUniqValues(hash_map<Key, Value> *hm) {
    hash_map<Value, bool> a(5, 0.5);
    for (const auto &it: *hm) {
        a.insert(it.second, true);
    }
    return a.getCapacity();
}

template<typename Key, typename Value>
void run(std::istream &input, std::ostream &output) {
    size_t n;
    input >> n;
    char action;
    hash_map<Key, Value> hash_map(100, 0.5);
    Key key;
    Value val;
    for (size_t i = 0; i < n; i++) {
        input >> action;
        switch (action) {
            case 'A': {
                input >> key >> val;
                hash_map.insert(key, val);
                break;
            }
            case 'R': {
                input >> key;
                hash_map.delKey(key);
                break;
            }
            default:
                break;

        }
    }
    output << hash_map.getCapacity() << " " << getCntUniqValues<Key, Value>(&hash_map);
}

int main() {
    char key_type, val_type;
    std::ifstream input("input.txt");
    std::ofstream output("output.txt");

    input >> key_type >> val_type;

    if (key_type == 'I' && val_type == 'S') {
        run<int, std::string>(input, output);
    } else if (key_type == 'I' && val_type == 'D') {
        run<int, double>(input, output);
    } else if (key_type == 'D' && val_type == 'S') {
        run<double, std::string>(input, output);
    } else if (key_type == 'D' && val_type == 'I') {
        run<double, int>(input, output);
    } else if (key_type == 'S' && val_type == 'I') {
        run<std::string, int>(input, output);
    } else if (key_type == 'S' && val_type == 'D') {
        run<std::string, double>(input, output);
    } else if (key_type == 'I' && val_type == 'I') {
        run<int, int>(input, output);
    } else if (key_type == 'D' && val_type == 'D') {
        run<double, double>(input, output);
    } else if (key_type == 'S' && val_type == 'S') {
        run<std::string, std::string>(input, output);
    }

    return 0;

}

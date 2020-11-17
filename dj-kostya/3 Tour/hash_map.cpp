//
// Created by Konstantin on 15.11.2020.
//

#include "hash_map.h"
#include <exception>

template<typename Key, typename Value>
size_t hash_map<Key, Value>::hash(Key k) {
    return std::hash<Key>()(k) % size;
}

template<typename Key, typename Value>
void hash_map<Key, Value>::insert(Key k, Value v) {
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

template<typename Key, typename Value>
Value &hash_map<Key, Value>::operator[](Key k) {
    size_t key_hash = hash(k);

    for (auto it = data[key_hash].begin(), it_end = data[key_hash].end();
         it != it_end; ++it) {
        if ((*it).first == k)
            return (*it).second;
    }


    throw std::invalid_argument("Key doesn't exists");
}

template<typename Key, typename Value>
void hash_map<Key, Value>::rehash() {
    hash_map<Key, Value> new_table(size * 2, rehash_value);
    for (auto it: *this) {
        new_table.insert(it.first, it.second);
    }
    data = new_table.data;
    capacity = new_table.capacity;
    size *= 2;
}

template<typename Key, typename Value>
typename hash_map<Key, Value>::Iterator hash_map<Key, Value>::end() {
    return hash_map<Key, Value>::Iterator();
}

template<typename Key, typename Value>
typename hash_map<Key, Value>::Iterator hash_map<Key, Value>::begin() {
    for (size_t i = 0; i < size; i++) {
        if (!data[i].isEmpty()) {
            return hash_map<Key, Value>::Iterator(&data[i], &data[size]);
        }
    }
    return end();
}

template<typename Key, typename Value>
void hash_map<Key, Value>::delKey(Key k) {
    size_t key_hash = hash(k);
    if (data[key_hash].isExist(k))
    {
        std::pair<Key, Value>p(k, operator[](k));
        data[key_hash].popByValue(p);
        if (data[key_hash].isEmpty())
            capacity--;
    }

}


template<typename Key, typename Value>
hash_map<Key, Value>::Iterator::Iterator(List<std::pair<Key, Value>> *cur_, List<std::pair<Key, Value>> *end_):
        cur_ptr(cur_), end_ptr(end_), list_iter(cur_->begin()), end_list_iter(cur_->end()) {}

template<typename Key, typename Value>
hash_map<Key, Value>::Iterator::Iterator(const hash_map<Key, Value>::Iterator &other): cur_ptr(other.cur_ptr),
                                                                                       end_ptr(other.end_ptr),
                                                                                       list_iter(other.list_iter),
                                                                                       end_list_iter(
                                                                                               other.end_list_iter) {}

template<typename Key, typename Value>
std::pair<Key, Value> &hash_map<Key, Value>::Iterator::operator*() {
    return *list_iter;
}

template<typename Key, typename Value>
typename hash_map<Key, Value>::Iterator hash_map<Key, Value>::Iterator::operator++() {
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

template<typename Key, typename Value>
bool hash_map<Key, Value>::Iterator::operator==(const hash_map<Key, Value>::Iterator &other) {
    return (cur_ptr == other.cur_ptr && list_iter == other.list_iter);
}

template<typename Key, typename Value>
bool hash_map<Key, Value>::Iterator::operator!=(const hash_map<Key, Value>::Iterator &other) {
    return !operator==(other);
}

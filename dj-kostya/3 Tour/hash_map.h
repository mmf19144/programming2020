//
// Created by Konstantin on 15.11.2020.
//

#ifndef HASHMAP_HASH_MAP_H
#define HASHMAP_HASH_MAP_H

#include "list.cpp"
#include <vector>

template<typename Key, typename Value>
class hash_map {
private:
    List<std::pair<Key, Value>> *data;
    float rehash_value;
    size_t size;
    size_t capacity = 0;

    void rehash();

    size_t hash(Key k);

public:
    hash_map(size_t size_, float rehash) : rehash_value(rehash),
                                           size(size_),
                                           data(new List<std::pair<Key, Value>>[size_]) {
    }

    void insert(Key k, Value t);

    void delKey(Key k);

    Value &operator[](Key k);

    class Iterator {
    private:
        friend class hash_map<Key, Value>;

        List<std::pair<Key, Value>> *cur_ptr;
        List<std::pair<Key, Value>> *end_ptr;

        typename List<std::pair<Key, Value>>::Iterator list_iter;
        typename List<std::pair<Key, Value>>::Iterator end_list_iter;

    public:
        Iterator() : cur_ptr(nullptr), end_ptr(nullptr) {}

        Iterator(List<std::pair<Key, Value>> *cur_, List<std::pair<Key, Value>> *end_);

        Iterator(hash_map<Key, Value>::Iterator const &other);

        std::pair<Key, Value> &operator*();

        Iterator operator++();

        bool operator==(hash_map<Key, Value>::Iterator const &other);

        bool operator!=(hash_map<Key, Value>::Iterator const &other);

    };

    hash_map<Key, Value>::Iterator begin();

    hash_map<Key, Value>::Iterator end();

    size_t getCapacity(){
        return capacity;
    }
};



#endif //HASHMAP_HASH_MAP_H

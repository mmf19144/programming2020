//
// Created by Konstantin on 10.11.2020.
//

#pragma once

#include <algorithm>


template<typename T>
class List {
private:


    class Node {
    private:
        T value;
        std::unique_ptr<Node> next;
    public:
        ~Node();

        Node();

        Node(T &v, std::unique_ptr<Node> n);

        Node *getNextRawPtr();

        std::unique_ptr<Node> getNext() {
            return std::move(next);
        }

        T &getValue();

        void setNext(std::unique_ptr<Node> n) {
            next = std::move(n);
        }


    };

    std::unique_ptr<List<T>::Node> head;
public:

    List();

    ~List();

    void pushFront(T v);

    bool isEmpty();

    T getFront();

    void popFront();

    class Iterator {
    private:
        friend class List<T>;

        Node *current;
    public:
        Iterator();

        explicit Iterator(List<T> &l);

        Iterator(List<T>::Iterator const &other);

        T &operator*();

        Iterator operator++();

        Iterator operator--();

        bool operator==(List<T>::Iterator const &other);

        bool operator!=(List<T>::Iterator const &other);

        Node *getCurrentPtr() { return current; }
    };

    List<T>::Iterator begin();

    List<T>::Iterator end();

    template<typename Key>
    bool isExist(Key val);

    void popByValue(T val);

    template<typename K, typename V>
    T &operator[](std::pair<K, V> const &val);
};



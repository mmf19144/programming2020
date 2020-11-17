//
// Created by Konstantin on 10.11.2020.
//
#include "list.h"


template<typename T>
List<T>::Node::~Node() = default;

template<typename T>
List<T>::Node::Node():next(nullptr), value(nullptr) {}

template<typename T>
List<T>::Node::Node(T &v, std::unique_ptr<Node> n) : value(v), next(std::move(n)) {}

template<typename T>
typename List<T>::Node *List<T>::Node::getNextRawPtr() {
    return next.get();
}

template<typename T>
T &List<T>::Node::getValue() {
    return value;
}


template<typename T>
void List<T>::pushFront(T v) {
    head = std::make_unique<List<T>::Node>(v, std::move(head));
}

template<typename T>
bool List<T>::isEmpty() {
    return this->head == nullptr;
}

template<typename T>
T List<T>::getFront() {
    return head->getValue();
}

template<typename T>
void List<T>::popFront() {
    head = std::move(head->getNext());
}

template<typename T>
List<T>::List() : head(nullptr) {}

template<typename T>
typename List<T>::Iterator List<T>::begin() {
    return List<T>::Iterator(*this);
}

template<typename T>
typename List<T>::Iterator List<T>::end() {
    return List<T>::Iterator();
}
template<typename T>
template<typename Key>
bool List<T>::isExist(Key val) {
    return std::any_of(begin(), end(),
                       [&val](auto v) { return v.first == val; }); // не сильно хорошо, что зашиваю логику в класс
}

template<typename T>
List<T>::~List() {
    for (std::unique_ptr<Node> current = std::move(head);
         current;
         current = std::move(current->getNext()));
}

template<typename T>
template<typename K, typename V>
T &List<T>::operator[](std::pair<K, V> const &val) {
    for (auto it = begin(), it_end = end(); it != it_end; ++it) {
        if ((*it).first == val.first)
            return *it;
    }
    throw std::invalid_argument("Key doesn't exists");
}

template<typename T>
void List<T>::popByValue(T val) {
    for (auto it = begin(), it_end = end(); it != it_end; ++it) {
        if ((*it).first == val.first) {
            if (it == begin()) {
                popFront();
            } else {
                Node *prev =  (--it).getCurrentPtr();
                prev->setNext(std::move(it.getCurrentPtr()->getNext()));
            }
            return;
        }
    }
}

template<typename T>
T &List<T>::Iterator::operator*() {
    return this->current->getValue();
}

template<typename T>
typename List<T>::Iterator List<T>::Iterator::operator++() {
    if (current) {
        current = current->getNextRawPtr();
    }
    return *this;
}

template<typename T>
bool List<T>::Iterator::operator==(const List<T>::Iterator &other) {
    return this->current == other.current;
}

template<typename T>
bool List<T>::Iterator::operator!=(const List<T>::Iterator &other) {
    return this->current != other.current;
}

template<typename T>
List<T>::Iterator::Iterator(): current(nullptr) {

}

template<typename T>
List<T>::Iterator::Iterator(List<T> &l) : current(l.head.get()) {}

template<typename T>
List<T>::Iterator::Iterator(const List<T>::Iterator &other) : current(other.current) {}

template<typename T>
typename List<T>::Iterator List<T>::Iterator::operator--() {
    if (current)
        current--;
    return *this;
}

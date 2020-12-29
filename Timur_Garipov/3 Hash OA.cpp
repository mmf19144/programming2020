#include <iostream>
#include <fstream>
#include <vector>
#include <mutex>
#include <thread>
#include <queue>
#include <chrono>
#include <regex>
#include <map>
#include <set>
#include <exception>

using namespace std;

template<typename T>
class Stack {
private:
    class Node {
    public:
        explicit Node(T &v) : val(v), next(nullptr) {}

        ~Node() = default;

        Node *getNextPtr() {
            return next.get();
        }

        void setNext(unique_ptr<Node> other) {
            next = move(other);
        }

        unique_ptr<Node> getNextUniq() {
            return move(next);
        }

        T getValue() {
            return val;
        }

    private:
        T val;
        unique_ptr<Node> next;
    };

    unique_ptr<Node> head;
public:

    Stack() : head(nullptr) {};

    ~Stack() = default;


    void push(T val) {
        unique_ptr<Node> p = make_unique<Node>(val);
        p->setNext(move(head));
        head = move(p);
    }

    T pop() {
        if (isEmpty()) {
            throw invalid_argument("Stack is empty");
        }
        unique_ptr<Node> q = move(head);
        head = move(q->getNextUniq());
        return q->getValue();
    }

    bool isEmpty() {
        return head == nullptr;
    }
};

std::ifstream fin("input.txt");

template<typename T>
void start() {

    size_t cnt;
    fin >> cnt;
    Stack<T> st;
    for (size_t i = 0; i < cnt; i++) {
        string action;
        fin >> action;
        if (action == "PUSH") {
            T val;
            fin >> val;
            st.push(val);
        } else if (action == "POP") {
            st.pop();
        }
    }
    std::ofstream fout("output.txt");
    T sum;
    bool is_first = true;
    size_t cnt_el = 0;
    while (!st.isEmpty()) {
        if (is_first) {
            sum = st.pop();
            is_first = false;
        } else
            sum += st.pop();
        cnt_el++;
    }
    fout << cnt_el;
    if (cnt_el) {
        fout << " " << sum << endl;
    }

}


int main() {
    Stack<int> a;

    char typeKey;
    fin >> typeKey;
    switch (typeKey) {
        case 'I':
            start<int>();
            break;
        case 'D':
            start<double>();
            break;
        case 'S':
            start<string>();
            break;
    }


    return 0;
}
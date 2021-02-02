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
          explicit Node(T& v) : val(v), next(nullptr) {}
          ~Node() = default;
          Node* getNextPtr() {
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
             return NULL;
          }
          unique_ptr<Node> q = move(head);
          head = move(q->getNextUniq());
          return q->getValue();
       }
       bool isEmpty() {
          return head == nullptr;
       }
    };

    template<typename T>
    void start(Stack<T>& st, std::ifstream& fin) {
       std::ofstream fout("output.txt");
       T sum;
       bool is_first = true;
       size_t cnt_el = 0;

       while (fin)
       {
          string choice;
          T el;
          fin >> choice;

          if (choice == string("PUSH")) {
             fin >> el;
             st.push(el);
          }
          else if (choice == string("POP"))
             st.pop();
       }

       while (!st.isEmpty()) {
          if (is_first) {
             sum = st.pop();
             is_first = false;
          }
          else
             sum += st.pop();
          cnt_el++;
       }
       fout << cnt_el;
       if (cnt_el) {
          fout << " " << sum << endl;
       }

    }

    template<typename T>
    void prepareFunc(std::ifstream& fin) {
       Stack<T>* stack = new Stack<T>;
       start<T>(*stack, fin);
    }

    int main() {
       std::ifstream fin("input.txt");

       char typeKey;
       fin >> typeKey;
       switch (typeKey) {
       case 'I':
          prepareFunc<int>(fin);
          break;
       case 'D':
          prepareFunc<double>(fin);
          break;
       case 'S':
          prepareFunc<string>(fin);
          break;
       }

       fin.close();
       return 0;
    }
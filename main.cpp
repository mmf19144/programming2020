#include <iostream>
#include <string>
#include <stack>
#include <new>
#include <fstream>


using namespace std;


class Expression {
public:
    virtual string get_str_expression() const = 0;
    virtual Expression* derivate(string _var) const = 0;
    virtual Expression* get_better_value() = 0;
    virtual Expression* copy() = 0;
    virtual bool var_exist() = 0;
    virtual int eval(string values) = 0;
    virtual void print(ofstream& output) const = 0;
    virtual ~Expression() = default;
};


class Number : public Expression {
private:
    int num;
public:
    Number() : num(0) {}
    Number(int value) : num(value) {}
    void print(ofstream& output) const {
        output << this->get_str_expression();
    }

    Expression* copy() {
        return new Number(num);
    }

    Expression* derivate(string _var) const {
        return new Number(0);
    }

    int eval(string values) {
        return num;
    }

    string get_str_expression() const {
        string num_exp = "";
        if (num != 0) {
            for (int i = num; i > 0; i /= 10) {
                num_exp = (char)((i % 10) + '0') + num_exp;
            }
        }
        else {
            num_exp = "0";
        }
        return num_exp;
    }

    Expression* get_better_value() {
        return new Number(num);
    }

    bool var_exist() {
        return true;
    }
};


class Variable : public Expression {
private:
    string var;
public:
    Variable() : var("x") {}
    Variable(string _var) : var(_var) {}

    void print(ofstream& output) const {
        output << this->get_str_expression();
    }

    Expression* copy() {
        return new Variable(var);
    }

    Expression* derivate(string _var) const {
        if (var == _var)
            return new Number(1);
        else
            return new Number(0);
    }

    int eval(string values) {
        int curr_value = 0;
        for (int i = 0; i < values.length(); i++) {			// parsing
            string check_var = "";
            while (values[i] != ' ') {
                check_var += values[i++];					// reading the name
            }
            if (check_var == var) {
                i += 4;										// Skipping arrow symbols
                while (values[i] != ';' && i < values.length())
                    curr_value = curr_value * 10 + (values[i++] - '0'); // converting string to int
                return curr_value;
            }
            else {
                for (; values[i] != ';' && i < values.length(); i++);
                i++;
            }
        }
        return 0;
    }

    string get_str_expression() const {
        return var;
    }

    Expression* get_better_value() {
        return new Variable(var);
    }

    bool var_exist() {
        return false;
    }
};


class Add : public Expression {
private:
    Expression* left, * right;
public:
    Add() : left(nullptr), right(nullptr) {}
    Add(Expression* _left, Expression* _right) : left(_left), right(_right) {}

    void print(ofstream& output) const {
        output << this->get_str_expression();
    }

    Expression* copy() {
        return new Add(left->copy(), right->copy());
    }

    Expression* derivate(string _var) const {
        return new Add(left->derivate(_var), right->derivate(_var));
    }

    int eval(string values) {
        return left->eval(values) + right->eval(values);
    }

    string get_str_expression() const {
        return "(" + left->get_str_expression() + "+" + right->get_str_expression() + ")";
    }

    Expression* get_better_value() {
        if (left->get_str_expression() == "0") {
            return right->get_better_value();
        }
        else if (right->get_str_expression() == "0") {
            return left->get_better_value();
        }
        return new Add(left->get_better_value(), right->get_better_value());
    }

    bool var_exist() {
        return left->var_exist() || right->var_exist();
    }

    ~Add() {
        delete left;
        delete right;
    }
};


class Sub : public Expression {
private:
    Expression* left, * right;

public:
    Sub() : left(nullptr), right(nullptr) {}
    Sub(Expression* _left, Expression* _right) : left(_left), right(_right) {}

    void print(ofstream& output) const {
        output << this->get_str_expression();
    }


    Expression* copy() {
        return new Sub(left->copy(), right->copy());
    }


    int eval(string values) {
        return left->eval(values) - right->eval(values);
    }

    Expression* derivate(string _var) const {
        return new Sub(left->derivate(_var), right->derivate(_var));
    }

    string get_str_expression() const {
        return "(" + left->get_str_expression() + "-" + right->get_str_expression() + ")";
    }

    Expression* get_better_value() {
        if (left->get_str_expression() == right->get_str_expression()) {
            return new Number(0);
        }
        else {
            return new Sub(left->get_better_value(), right->get_better_value());;
        }
    }

    bool var_exist() {
        return left->var_exist() || right->var_exist();
    }

    ~Sub() {
        delete left;
        delete right;
    }
};


class Mul : public Expression {
private:
    Expression* left, * right;

public:
    Mul() : left(nullptr), right(nullptr) {}
    Mul(Expression* _left, Expression* _right) : left(_left), right(_right) {}

    void print(ofstream& output) const {
        output << this->get_str_expression();
    }

    int eval(string values) {
        return left->eval(values) * right->eval(values);
    }

    Expression* copy() {
        return new Mul(left->copy(), right->copy());
    }

    Expression* derivate(string _var) const {
        return new Add(new Mul(left->derivate(_var), right->copy()), new Mul(left->copy(), right->derivate(_var)));
    }

    string get_str_expression() const {
        return "(" + left->get_str_expression() + "*" + right->get_str_expression() + ")";
    }

    Expression* get_better_value() {
        if (left->get_str_expression() == "0") {
            return new Number(0);
        }
        else if (right->get_str_expression() == "0") {
            return new Number(0);
        }
        if (left->get_str_expression() == "1") {
            return right->get_better_value();
        }
        else if (right->get_str_expression() == "1") {
            return left->get_better_value();
        }
        return new Mul(left->get_better_value(), right->get_better_value());;
    }

    bool var_exist() {
        return left->var_exist() || right->var_exist();
    }

    ~Mul() {
        delete left;
        delete right;
    }
};


class Div : public Expression {
private:
    Expression* left, * right;

public:
    Div() : left(nullptr), right(nullptr) {}
    Div(Expression* _left, Expression* _right) : left(_left), right(_right) {}

    void print(ofstream& output) const {
        output << this->get_str_expression();
    }

    int eval(string values) {
        return left->eval(values) / right->eval(values);
    }

    Expression* copy() {
        return new Div(left->copy(), right->copy());
    }

    Expression* derivate(string _var) const {
        return new Div(new Sub(new Mul(left->derivate(_var), right->copy()), new Mul(left->copy(), right->derivate(_var))), new Mul(right, right));
    }

    string get_str_expression() const {
        return "(" + left->get_str_expression() + "/" + right->get_str_expression() + ")";
    }

    Expression* get_better_value() {
        return new Div(left->get_better_value(), right->get_better_value());
    }

    bool var_exist() {
        return left->var_exist() || right->var_exist();
    }

    ~Div() {
        delete left;
        delete right;
    }
};


int get_priority(char check_for_operation) {        //operator priority
    char operations[] = "()+-*/";
    for (int priority = 0; operations[priority] != '\0'; priority++) {
        if (check_for_operation == operations[priority])
            return priority / 2;
    }
    return -1;
}


string get_postfix(string input_data) {
    stack <char> operations;
    string postfix = "";
    bool number_before = false; // For (-(-n)) cases
    int priority = 0;
    for (int i = 0; i < input_data.length(); i++) {
        switch (input_data[i]) {
        case ' ': break;

        case '(':
            operations.push('(');
            number_before = false;
            break;

        case ')':
            while (operations.top() != '(') {
                postfix = postfix + ' ' + operations.top();
                operations.pop();
            }
            operations.pop();
            break;

        case '-':
            if (!number_before) {
                postfix += '0';
            }
        case '+':
        case '*':
        case '/':
            priority = get_priority(input_data[i]);
            while (!operations.empty() && priority <= get_priority(operations.top())) {
                postfix = postfix + ' ' + operations.top();
            }
            postfix = postfix + ' ';
            operations.push(input_data[i]);
            number_before = false;
            break;

        default:
            postfix = postfix + input_data[i];
            number_before = true;
            break;
        }
    }
    while (!operations.empty()) {
        postfix = postfix + ' ' + operations.top();
        operations.pop();
    }
    return postfix;
}


Expression* generate_expression(string postfix) {
    string operations = "-+*/";
    stack <Expression*> values;

    for (int i = 0; i < postfix.length(); i++) {
        if (get_priority(postfix[i]) == -1) {
            Expression* new_exp = nullptr;
            if ('0' <= postfix[i] && postfix[i] <= '9') {
                int num = 0;
                while (postfix[i] != ' ' && i < postfix.length()) {
                    num = num * 10 + (postfix[i] - '0');
                    i++;
                }
                new_exp = new Number(num);
            }
            else {
                string var = "";
                while (postfix[i] != ' ' && i < postfix.length()) {
                    var += postfix[i];
                    i++;
                }
                new_exp = new Variable(var);
            }
            values.push(new_exp);
        }
        else {
            Expression* exp2 = values.top();
            values.pop();
            Expression* exp1 = values.top();
            values.pop();
            switch (postfix[i]) {
            case '+':
                values.push(new Add(exp1, exp2));
                break;

            case '-':
                values.push(new Sub(exp1, exp2));
                break;

            case '*':
                values.push(new Mul(exp1, exp2));
                break;

            case '/':
                values.push(new Div(exp1, exp2));
                break;
            }
            i++;
        }
    }
    return values.top();
}


Expression* make_this_better(Expression* bad_expression) {
    if (!bad_expression->var_exist()) {				  // checking for variable
        return new Number(bad_expression->eval(""));  // calculating if no variables
    }
    Expression* better_expression = bad_expression->get_better_value();
    if (better_expression->get_str_expression() != bad_expression->get_str_expression()) {
        return make_this_better(better_expression);
    }

    return better_expression;
}


int main() {
    ifstream input("input.txt");
    ofstream output("output.txt");
    string input_data;
    input >> input_data;
    if (input_data == "")
        throw("dude seriously");
    input_data = get_postfix(input_data);
    Expression* ready_expression = generate_expression(input_data);
    Expression* der_exp = ready_expression->derivate("x");
    der_exp->print(output);
    delete der_exp;
    delete ready_expression;
    return 0;
}

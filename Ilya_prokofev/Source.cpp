#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <list>
#include <cctype>
#include <vector>

using namespace std;

class Env;


class Expression {
public:
	virtual Expression* eval (Env* data) = 0;
    virtual ~Expression() {}
};

int get_value(Expression* expression);

class Env  {
    unordered_map<string, Expression*> env;
    friend class Var;
    friend class Let;
    friend class Call;
public:
    Env& operator=(const Env& other){
        if (this != &other){
            env = other.env;
        }
        return *this;
    }

    Expression* fromEnv(string id) {
        return env.at(id);
    }
};

class Val: public Expression {
    int val;
    friend int get_value(Expression* expression);
public:
    Val(int val) : val(val) {}
    virtual Expression* eval(Env* data) {
        return this;
    }
};

class Var: public Expression {
    string id;
public:
    Var(string& id) : id(id) 
    {

    }
    virtual Expression* eval(Env* data) {
        return data->fromEnv(id);
    }
};

class Add: public Expression {
    Expression* left;
    Expression* right;
public:
    Add(Expression* left, Expression* right) : left(left), right(right) {}
    virtual Expression* eval(Env* data) {
        return new Val(get_value(left->eval(data)) + get_value(right->eval(data)));
    }
    virtual ~Add(){
        delete right;
        delete left;
    }
}; 

class Function : public Expression {
    string id;
    Expression* expression;
    Env env;
    friend class Call;
public:
    Function(string& id, Expression* expression) : id(id), expression(expression) {}

    virtual Expression* eval(Env* data) {
        return this;
    }

    virtual ~Function() {
        delete expression;
    }
};

class Let : public Expression {
    string id;
    Expression* e_value;
    Expression* e_body;
    friend class Call;
public:
    Let(string& id, Expression* e_value, Expression* e_body) : id(id), e_value(e_value), e_body(e_body) {}

    virtual Expression* eval(Env* data){
        data->env[id] = e_value->eval(data);
        return e_body->eval(data);
    }

    virtual ~Let()
    {
        delete e_value;
        delete e_body;
    }
};

class If : public Expression {
    Expression* left;
    Expression* right;
    Expression* e_then;
    Expression* e_else;
public:
    If(Expression* left, Expression* right, Expression* e_then, Expression* e_else): left(left), right(right), e_else(e_else), e_then(e_then) {}
    virtual Expression* eval (Env* data){
        if (get_value(left->eval(data)) > get_value(right->eval(data))){
            return e_then->eval(data);
        }
        else{
            return e_else->eval(data);
        }
    }
    virtual ~If() {
        delete left;
        delete right;
        delete e_else;
        delete e_then;
    }
};

class Call: public Expression {
    Expression* f_expr;
    Expression* arg_expr;
public:
    Call(Expression* f_expr, Expression* arg_expr) : f_expr(f_expr), arg_expr(arg_expr) {}

    virtual Expression* eval(Env* data) {
        Expression* eval_f = f_expr->eval(data);
        Function* f_expr_func = dynamic_cast<Function*>(eval_f);
        if (f_expr_func) {
                data->env[f_expr_func->id] = arg_expr->eval(data);
            return f_expr_func->expression->eval(data);
        }
        else {
            throw invalid_argument("error");
        }
    }
    virtual ~Call() {
        delete f_expr;
        delete arg_expr;
    }
};

int get_value(Expression* expression) {
    Val* int_val = dynamic_cast<Val*>(expression);
    if (int_val != nullptr) {
        return int_val->val;
    }
    else throw invalid_argument("input expression is not value");
}

vector <string> tokens(string source) {
    vector <string> result;
    string tmp;
    for (auto cur : source) {
        if (isalpha(cur) || isdigit(cur) || cur == '-') {
            tmp += cur;
        }
        else {
            if (tmp.length()) {
                result.push_back(tmp);
            }
            tmp = string();
            if (!isspace(cur)) {
                tmp += cur;
                result.push_back(tmp);
                tmp = string();
            }
        }
    }
    if (tmp.length()) {
        result.push_back(tmp);
    }
    return result;
}

Expression* build_expression(vector <string>& tokens,size_t& position) {
    position++;
    if (tokens[position] == "val") {
        Val* result;
        position++;
        result = new Val (stoi(tokens[position]));
        position += 2;
        return result;
    }
    if (tokens[position] == "var"){
        Var* result;
        position++;
        result = new Var(tokens[position]);
        position += 2;
        return result;
    }
    if (tokens[position] == "add"){
        Add* result;
        position++;
        Expression* left = build_expression(tokens, position);
        Expression* right = build_expression(tokens, position);
        result = new Add(left, right);
        position++;
        return result;
    }
    if (tokens[position] == "let") {
        Let* result;
        position++;
        string id = tokens[position];
        position += 2;
        Expression* expr = build_expression(tokens, position);
        position ++;
        Expression* body = build_expression(tokens, position);
        result = new Let(id, expr, body);
        position++;
        return result;
    }
    if (tokens[position] == "if") {
        If* result;
        position++;
        Expression* left = build_expression(tokens, position);
        Expression* right = build_expression(tokens, position);
        position++;
        Expression* then = build_expression(tokens, position);
        position++;
        Expression* e_else = build_expression(tokens, position);
        result = new If(left, right, then, e_else);
        position++;
        return result;
    }
    if (tokens[position] == "function") {
        Function* result;
        position++;
        std::string id = tokens[position];
        position++;
        Expression* expression = build_expression(tokens, position);
        result = new Function(id, expression);
        position++;
        return result;
    }
    if (tokens[position] == "call") {
        Call* result;
        position++;
        Expression* f_expr = build_expression(tokens, position);
        Expression* arg_expr = build_expression(tokens, position);
        result = new Call(f_expr, arg_expr);
        position++;
        return result;
    }
}

Expression* build(istream& data) {
    size_t position = 0;
    string content((istreambuf_iterator<char>(data)), istreambuf_iterator<char>());
    vector <string> tokens_ = tokens(content);
    return build_expression(tokens_, position);
}

int main() {
    Env env;
    ifstream input("input.txt");
    Expression* exp = build(input);
    input.close();
    ofstream output("output.txt");
    try  {
        output << "(val " << get_value(exp->eval(&env)) << ")" << endl;
    }
    catch (...) {
        output << "ERROR";
    }
    output.close();
    delete exp;
    return 0;
}
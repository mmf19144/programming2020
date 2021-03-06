#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

bool is_digit(char st) {
	return isdigit(st) || (st == '-');
}

class Function;

class Num_Fun {
private:
	int num;
	bool is_num;
	Function* fun;
public:
	Num_Fun(int num0 = 0) {
		num = num0;
		is_num = true;
	}
	Num_Fun(Function* fun0) {
		fun = fun0;
		is_num = false;
	}
	int Get_num()const{
		return num;
	}
	int Get_is_num()const {
		return is_num;
	}
	Function* Get_fun() const {
		return fun;
	}

	operator int() {
		return num;
	}
};

typedef unordered_map <string, Num_Fun> Env;

class Expression {
private:

public:
	virtual Num_Fun eval(Env& env) = 0;
	~Expression(){}
};


class Val : public Expression{
private:
	int val;
public:
	virtual Num_Fun eval(Env& env) {
		return Num_Fun(val);
	}
	Val(int val0) {
		val = val0;
	}
};

class Var : public Expression {
private:
	string var;
public:
	virtual Num_Fun eval(Env& env) {
		if (env.count(var) == 0) {
			throw exception();
		}
		return env[var];
	}
	Var(string var0) {
		var = var0;
	}
};

class Add : public Expression {
private:
	Expression* e1;
	Expression* e2;
public:
	virtual Num_Fun eval(Env& env) {
		return Num_Fun(e1->eval(env) + e2->eval(env));
	}
	Add(Expression* e10, Expression* e20) {
		e1 = e10;
		e2 = e20;
	}
	~Add() {
		delete e1;
		delete e2;
	}
};

class If : public Expression {
private:
	Expression* e1;
	Expression* e2;
	Expression* e_then;
	Expression* e_else;
public:
	virtual Num_Fun eval(Env& env) {
		if (e1->eval(env) > e2->eval(env)) {
			return e_then->eval(env);
		}
		else {
			return e_else->eval(env);
		}
	}
	If(Expression* e10, Expression* e20, Expression* e_then0, Expression* e_else0) {
		e1 = e10;
		e2 = e20;
		e_then = e_then0;
		e_else = e_else0;
	}
	~If(){
		delete e1;
		delete e2;
		delete e_then;
		delete e_else;
	}
};

class Let : public Expression {
private:
	string id;
	Expression* e_value;
	Expression* e_body;
public:
	virtual Num_Fun eval(Env& env) {
		Env env1 = env;
		env1[id] = e_value->eval(env);
		return e_body->eval(env1);
	}
	Let(string id0, Expression* e_value0, Expression* e_body0) {
		id = id0;
		e_value = e_value0;
		e_body = e_body0;
	}
	~Let(){
		delete e_value;
		delete e_body;
	}
};

class Function : public Expression {
private:
	string f_arg_id;
	Expression* f_body;
public:
	string Get_f_arg_id() const {
		return f_arg_id;
	}
	Expression* Get_f_body() const {
		return f_body;
	}
	virtual Num_Fun eval(Env& env) {
		return Num_Fun(this);
	}
	Function(string f_arg_id0, Expression* f_body0) {
		f_arg_id = f_arg_id0;
		f_body = f_body0;
	}
	~Function() {
		delete f_body;
	}
};

class Call : public Expression {
private:
	Expression* f_expr;
	Expression* arg_expr;
public:
	virtual Num_Fun eval(Env& env) {
		Num_Fun f = f_expr->eval(env);
		if (f.Get_is_num())
			throw exception();
		
		Function* fn = f.Get_fun();
		string id = fn->Get_f_arg_id();
		Expression *e = fn->Get_f_body();

		Env env1 = env;
		env1[id] = arg_expr->eval(env);
		return e->eval(env1);
	}
	Call(Expression* f_expr10, Expression* arg_expr20) {
		f_expr = f_expr10;
		arg_expr = arg_expr20;
	}
	~Call() {
		delete f_expr;
		delete arg_expr;
	}
};

vector <string> getlex(istream& in) {
	vector <string> res;
	const int N = 1024;
	char st[N];
	while (in.getline(st, N)) {
		string lexem;
		if (!isspace(st[0]))
			lexem += st[0];
		int len = strlen(st);
		for (int i = 1; i < len; ++i) {
			if (is_digit(st[i - 1]) && is_digit(st[i])) {
				lexem += st[i];
			}
			else if (isalpha(st[i - 1]) && isalpha(st[i])) {
				lexem += st[i];
			}
			else if (isspace(st[i])) {
				if (!lexem.empty())
					res.push_back(lexem);
				lexem = "";
			}
			else {
				if (!lexem.empty())
					res.push_back(lexem);
				lexem = "";
				lexem += st[i];
			}
		}
		if (!lexem.empty())
			res.push_back(lexem);
	}
	return res;
}

Expression* parse(vector<string> l, int &pos) {
	Expression* res;

	if (l[pos++] != "(") {
		throw exception();
	}
	if (l[pos] == "val") {
		pos++;
		if (!is_digit(l[pos][0])) {
			throw exception();
		}
		res = new Val(atoi(l[pos++].c_str()));
	}
	else if (l[pos] == "var") {
		pos++;
		if (!isalpha(l[pos][0])) {
			throw exception();
		}
		res = new Var(l[pos++]);
	}
	else if (l[pos] == "add") {
		pos++;
		Expression* e1 = parse(l, pos);
		Expression* e2 = parse(l, pos);
		res = new Add(e1, e2);
	}
	else if (l[pos] == "if") {
		pos++;
		Expression* e1 = parse(l, pos);
		Expression* e2 = parse(l, pos);
		if (l[pos++]!="then") {
			throw exception();
		}
		Expression* e_then = parse(l, pos);
		if (l[pos++] != "else") {
			throw exception();
		}
		Expression* e_else = parse(l, pos);
		res = new If(e1, e2, e_then, e_else);
	}
	else if (l[pos] == "let") {
		pos++;
		if (!isalpha(l[pos][0])) {
			throw exception();
		}
		string id = l[pos++];
		if (l[pos++] != "=") {
			throw exception();
		}
		Expression* e_value = parse(l, pos);
		if (l[pos++] != "in") {
			throw exception();
		}
		Expression* e_body = parse(l, pos);
		res = new Let(id, e_value, e_body);
	}
	else if (l[pos] == "function") {
		pos++;
		if (!isalpha(l[pos][0])) {
			throw exception();
		}
		string id = l[pos++];
		Expression* f_body = parse(l, pos);
		res = new Function(id, f_body);
	}
	else if (l[pos] == "call") {
		pos++;
		Expression* e1 = parse(l, pos);
		Expression* e2 = parse(l, pos);
		res = new Call(e1, e2);
	}
	else {
		throw exception();
	}
	if (l[pos++] != ")") {
		throw exception();
	}

	return res;
}

int main() {
	std::ofstream fout("output.txt");
	//auto &fout = cout;

	std::ifstream fin("input.txt");

	int pos = 0;

	Expression* e = NULL;
	try {
		e = parse(getlex(fin), pos);
	}
	catch (exception e) {
		fout << "Parsing error" << endl;
		return 0;
	}

	Env env;

	try {
		Num_Fun n = e->eval(env);
		if (!n.Get_is_num())
			throw exception();
		fout << "(val " << n.Get_num() << ")" << endl;
	}
	catch (exception e) {
		fout << "ERROR" << endl;
	}

	return 0;
}

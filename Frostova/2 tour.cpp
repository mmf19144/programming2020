#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <cassert>
using namespace std;


class Expression {
public:
	virtual void print() const = 0;
	virtual Expression* der(string X) const = 0;
	virtual Expression* copy() const = 0;
	virtual int eval(map<string, int>& values) const = 0;
	virtual ~Expression() {}
};

class Number : public Expression {
private:
	int num;
public:
	virtual void print() const {
		printf("%d", num);
	}
	virtual Expression* der(string X) const {
		return new Number(0);
	}
	virtual int eval(map<string, int>& values) const {
		return num;
	}
	virtual Expression* copy() const {
		return new Number(num);
	}
	Number(int num0) {
		num = num0;
	}
};

class Variable : public Expression {
private:
	string var;
public:
	virtual void print() const {
		printf("%s", var.c_str());
	}
	virtual Expression* der(string X) const {
		if (var == X) {
			return new Number(1);
		}
		else return new Number(0);
	}
	virtual int eval(map<string, int>& values) const {
		return values[var];
	}
	virtual Expression* copy() const {
		return new Variable(var);
	}
	Variable(string var0) {
		var = var0;
	}
};

class Add : public Expression {
private:
	Expression* s1;
	Expression* s2;
public:
	virtual void print() const {
		printf("(");
		s1->print();
		printf(" + ");
		s2->print();
		printf(")");
	}
	virtual Expression* der(string X) const {
		return new Add(s1->der(X), s2->der(X));
	}
	virtual int eval(map<string, int>& values) const {
		return s1->eval(values) + s2->eval(values);
	}
	Add(Expression* s10, Expression* s20) {
		s1 = s10;
		s2 = s20;
	}
	virtual Expression* copy() const {
		return new Add(s1->copy(), s2->copy());
	}
	~Add() {
		delete s1;
		delete s2;
	}
};

class Sub : public Expression {
private:
	Expression* s1;
	Expression* s2;
public:
	virtual void print() const {
		printf("(");
		s1->print();
		printf(" - ");
		s2->print();
		printf(")");
	}
	virtual Expression* der(string X) const {
		return new Sub(s1->der(X), s2->der(X));
	}
	virtual int eval(map<string, int>& values) const {
		return s1->eval(values) - s2->eval(values);
	}
	Sub(Expression* s10, Expression* s20) {
		s1 = s10;
		s2 = s20;
	}
	virtual Expression* copy() const {
		return new Sub(s1->copy(), s2->copy());
	}
	~Sub() {
		delete s1;
		delete s2;
	}
};

class Mul : public Expression {
private:
	Expression* s1;
	Expression* s2;
public:
	virtual void print() const {
		printf("(");
		s1->print();
		printf(" * ");
		s2->print();
		printf(")");
	}
	virtual Expression* der(string X) const {
		return new Add(new Mul(s1->der(X), s2->copy()), new Mul(s1->copy(), s2->der(X)));
	}
	virtual int eval(map<string, int>& values) const {
		return s1->eval(values) * s2->eval(values);
	}
	Mul(Expression* s10, Expression* s20) {
		s1 = s10;
		s2 = s20;
	}
	virtual Expression* copy() const {
		return new Mul(s1->copy(), s2->copy());
	}
	~Mul() {
		delete s1;
		delete s2;
	}
};

class Div : public Expression {
private:
	Expression* s1;
	Expression* s2;
public:
	virtual void print() const {
		printf("(");
		s1->print();
		printf(" / ");
		s2->print();
		printf(")");
	}
	virtual Expression* der(string X) const {
		return new Div(
			new Sub(
				new Mul(s1->der(X), s2->copy()),
				new Mul(s1->copy(), s2->der(X))
			),
			new Mul(s1->copy(), s2->copy())
		);
	}
	virtual int eval(map<string, int>& values) const {
		return s1->eval(values) * s2->eval(values);
	}
	Div(Expression* s10, Expression* s20) {
		s1 = s10;
		s2 = s20;
	}
	virtual Expression* copy() const {
		return new Mul(s1->copy(), s2->copy());
	}
	~Div() {
		delete s1;
		delete s2;
	}
};

vector <string> getlex(const string& st) {
	vector <string> res;
	string lexem;
	lexem += st[0];
	for (int i = 1; i < st.length(); ++i) {
		if (isdigit(st[i - 1]) && isdigit(st[i])) {
			lexem += st[i];
		}
		else if (isalpha(st[i - 1]) && isalpha(st[i])) {
			lexem += st[i];
		}
		else {
			res.push_back(lexem);
			lexem = "";
			lexem += st[i];
		}
	}
	res.push_back(lexem);
	return res;
}

Expression* pars(vector <string>& lexs, int& pos) {
	if (isdigit(lexs[pos][0])) {
		int n;
		sscanf(lexs[pos++].c_str(), "%d", &n);
		return new Number(n);
	}

	if (isalpha(lexs[pos][0])) {
		return new Variable(lexs[pos++]);
	}

	assert(lexs[pos] == "(");
	pos++;

	Expression* e1 = pars(lexs, pos);
	char oper = lexs[pos++][0];
	Expression* e2 = pars(lexs, pos);

	assert(lexs[pos] == ")");
	pos++;

	switch (oper) {
	case '+':
		return new Add(e1, e2);
	case '-':
		return new Sub(e1, e2);
	case '*':
		return new Mul(e1, e2);
	case '/':
		return new Div(e1, e2);
	}
}

Expression* read(const string& st) {
	auto lexs = getlex(st);
	int pos = 0;
	return pars(lexs, pos);
}


int main() {

	std::ofstream fout("output.txt");
	std::ifstream fin("input.txt");
	//Expression* e = new Variable("x");
	//e->print();
	//printf("\n");
	//
	//Expression* de_dx = e->der("x");
	//de_dx->print();
	//printf("\n");
	//
	//Expression* d = new Mul(new Variable("x"), new Variable("x"));
	//d->der("x")->print();
	//printf("\n");
	//
	//Expression* eee = new Add(
	//	new Number(3),
	//	new Mul(
	//		new Number(2),
	//		new Variable("x")));
	//
	//map<string, double> m;
	//m["x"] = 10;
	//printf("%lf\n", eee->eval(m));
	//
	//delete e;
	//delete de_dx;
	//delete eee;


	string str;
	fin >> str;
	Expression* st = read(str);

	Expression* dst = st->der("x");
	dst->print();

	delete dst;
	delete st;


	return 0;
}

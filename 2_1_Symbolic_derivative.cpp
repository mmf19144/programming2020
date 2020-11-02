#include <iostream>
#include <fstream>
#include <stack>
using namespace std;

class Expression {
public:
	virtual Expression* diff() = 0;
	virtual void print(ofstream& fout) = 0; //? try to use <<
	virtual ~Expression() { }
};

class Number: public Expression {
	int num;
public:
	Number(int n) {
		num = n;
	}

	Number* diff() {
		return new Number(0);
	}

	void print(ofstream& fout) {
		fout << num;
	}
};

class Variable : public Expression {
	char var;
public:
	Variable(char v) {
		var = v;
	}
	
	Variable() {
		var = 'x';
	}

	Number* diff() {
		return new Number(1);
	}

	void print(ofstream& fout) {
		fout << var;
	}
};

class Term : public Expression {
protected:
	Expression* e1;
	Expression* e2;
public:
	Term(Expression* e1, Expression* e2) {
		this->e1 = e1;
		this->e2 = e2;
	}

	void pr(ofstream& fout, char act) {
		fout << "(";
		e1->print(fout);
		fout << act;
		e2->print(fout);
		fout << ")";
	}
};

class Add : public Term { //eliminate copypaste between this class and the following
public:
	Add(Expression* e1, Expression* e2) : Term(e1, e2) { }

	Expression* diff() { //change to expr
		return new Add(e1->diff(), e2->diff());
	}

	void print(ofstream& fout) {
		pr(fout, '+');
	}
};

class Sub : public Term {
public:
	Sub(Expression* e1, Expression* e2) : Term(e1, e2) { }

	Expression* diff() {
		return new Sub(e1->diff(), e2->diff());
	}

	void print(ofstream& fout) {
		pr(fout, '-');
	}
};

class Mul : public Term {
public:
	Mul(Expression* e1, Expression* e2) : Term(e1, e2) { }

	Expression* diff() {
		Mul* m1 = new Mul(e1->diff(), e2);
		Mul* m2 = new Mul(e1, e2->diff());
		return new Add(m1, m2);
	}

	void print(ofstream& fout) {
		pr(fout, '*');
	}
};

class Div : public Term {
public:
	Div(Expression* e1, Expression* e2) : Term(e1, e2) { }

	Expression* diff() {
		Mul* m1 = new Mul(e1->diff(), e2);
		Mul* m2 = new Mul(e1, e2->diff());
		Sub* s = new Sub(m1, m2);
		Mul* m = new Mul(e2, e2);
		return new Div(s, m);
	}

	void print(ofstream& fout) {
		pr(fout, '/');
	}
};

Expression* getNum(stack<char> *st) {
	char tmp = st->top();
	int n = 0, k = 1;
	while (tmp <= '9' && tmp >= '0') {
		n += (tmp - '0') * k;
		k *= 10;
		st->pop();
		tmp = st->top();
		if (tmp == '(') {
			return new Number(n);
		}
	}
	return new Number(n);
}

Expression* getExp(ifstream& fin) {
	stack<char> stch;
	stack<Expression*> stexp;
	char act = 0;;
	char tmp = fin.get();
	stch.push(tmp);
	Expression* exp1 = NULL;
	Expression* exp2 = NULL;
	int flag = 0;
	


	if (tmp != '(') {
		if (tmp <= '9' && tmp >= '0') {
			int k = 10, n = tmp;
			while (fin.get(tmp)) {
				n += (tmp - '0') * k;
			}
			return new Number(n);
		}
		if (tmp >= 'a' && tmp <= 'z')
			return new Variable(tmp);
	}

	while (!stch.empty()) {		
		while (tmp != ')' && fin.get(tmp)) {
			stch.push(tmp);
		}
		int k = 0;
		while (tmp != '(') {
			stch.pop();
			tmp = stch.top();

			if (tmp <= '9' && tmp >= '0') {  //num
				flag++;
				if (k == 0) {
					exp2 = getNum(&stch);
					tmp = stch.top();
				}
				else {
					exp1 = getNum(&stch);
					tmp = stch.top();
				}
			}

			else if (tmp >= 'a'  && tmp <= 'z') { //var
				flag++;
				if (k == 0)
					exp2 = new Variable(tmp);
				else
					exp1 = new Variable(tmp);
			}

			if (tmp == '+' || tmp == '*' || tmp == '-' || tmp == '/') { 
				act = tmp;
			}

			k++;
		}
		stch.pop();

		if (flag == 0) { //exp + exp
			exp2 = stexp.top();
			stexp.pop();
			exp1 = stexp.top();
			stexp.pop();
		}

		if (flag == 1) { //exp + num or num + exp
			if (exp1 == NULL) 
				exp1 = stexp.top();
			else
				exp2 = stexp.top();

			stexp.pop();
		}

		//if num + num there's nothing to do

		if (act == '+')
			stexp.push(new Add(exp1, exp2));
		if (act == '-')
			stexp.push(new Sub(exp1, exp2));
		if (act == '*')
			stexp.push(new Mul(exp1, exp2));
		if (act == '/')
			stexp.push(new Div(exp1, exp2));

		exp1 = NULL; exp2 = NULL;
		flag = 0;
	}
	return stexp.top();
}

int main() {
	/*
	Expression *e = new Add(new Number(3), new Mul(new Number(2), new Variable('x')));
	e->print(); //  (3 + (2 * x));
	cout << "\n";
	Expression * de = e->diff();
	de->print(); //  (0 + ((0*x)+(2 * 1)));
	*/

	ifstream fin("input.txt");
	ofstream fout("output.txt");

	Expression *e = getExp(fin);
	e = e->diff();
	e->print(fout);
	fin.close();
	fout.close();

}
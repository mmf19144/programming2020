#include <string>
#include <iostream>
#include <fstream>
#include <stack>
#include <vector>
#include <sstream>

using namespace std;

class Expression 
{
public:
	Expression() {}
	virtual string to_string() const = 0;
	ostream& print(ostream& out) const 
	{
		out << this->to_string();
		return out;
	}
	virtual Expression* derivative(string var) const = 0;
	virtual int eval (string eval) const = 0;
	virtual Expression* simplification() const = 0; 
	virtual Expression* copy() const = 0;
};
class Number : public Expression 
{
	int val;
	friend Expression* build(string exp);

	friend class Div;
	friend class Add;
	friend class Sub;
	friend class Mul;

public:
	Number (const Number& that) {
		this->val = that.val;
	}
	Number(int val) : val(val){}
	virtual Expression* copy() const{
		return new Number(this->val);
	}
	virtual string to_string() const
	{
		return std::to_string(val);
	}
	virtual Expression* derivative(string var) const
	{
		return new Number(0);
	}
	virtual int eval(string eval) const
	{
		return val;
	}
	virtual Expression* simplification() const
	{
		return new Number(val);
	}
};
class Variable : public Expression 
{
	string var;
	friend Expression* build(string exp);
public:
	Variable(const Variable& that) {
		this->var = that.var;
	}
	Variable(string var) : var(var){}
	virtual Expression* copy() const {
		return new Variable(this->var);
	}
	virtual string to_string() const
	{
		return var;
	}
	virtual Expression* derivative(string var) const
	{
		if (var == this->var) 
		{
			return new Number(1);
		}
		return new Number(0);
	}
	virtual int eval(string eval) const
	{
		size_t i = eval.find(this -> var + " <- ");
		if (i == string::npos)
		{
			throw invalid_argument("incorrect eval");
		}
		if (i > 0) {
			size_t i = eval.find(" " + this->var + " <- ");
			if (i == string::npos)
			{
				throw invalid_argument("incorrect eval");
			}
			i++;
		}
		string x = "";
		size_t delimeter = eval.find(";", i + 1);
		if (delimeter == string::npos)
		{
			delimeter = eval.length();
		}
		i += strlen(" -> ") + this->var.length();
		x = eval.substr(i, delimeter - i);
		int res = stoi(x);
		return res;
	}

	virtual Expression* simplification() const
	{
		return new Variable(var);
	}
};
class Add : public Expression 
{
	Expression* left;
	Expression* right;
	Add(Expression* left, Expression* right) : left(left), right(right){}
	friend class Mul;
	friend class Div;
	friend Expression* build(string exp);
public:
	Add(const Add& that) {
		this->left = that.left->copy();
		this->right = that.right->copy();
	}
	virtual Expression* copy() const {
		return new Add(this->left->copy(), this->right->copy());
	}
	virtual string to_string() const
	{
		string str = "";
		str += "(";
		str += left->to_string();
		str += "+";
		str += right->to_string();
		str += ")";
		return str;
	}
	virtual Expression* derivative(string var) const
	{
		return new Add(left->derivative(var), right->derivative(var));
	}
	~Add()
	{
		delete left;
		delete right;
	}
	virtual int eval(string eval) const
	{
		return left -> eval(eval) + right -> eval(eval);
	}
	virtual Expression* simplification() const
	{
		Expression* left_simpl = left->simplification();
		Expression* right_simpl = right->simplification();
		Number* right_number = dynamic_cast <Number*> (right_simpl);
		Number* left_number = dynamic_cast <Number*> (left_simpl);

		if (right_number && right_number->val == 0)
		{
			delete right_simpl;
			return left_simpl;
		}
		if (left_number && left_number->val == 0)
		{
			delete left_simpl;
			return right_simpl;
		}
		if (left_number && right_number)
		{
			
			Expression* result = new Number(left_number->val + right_number->val);
			delete left_simpl;
			delete right_simpl;

			return result;
		}
		return new Add(left_simpl, right_simpl);
	}
};
class Sub : public Expression
{
	Expression* left;
	Expression* right;
	Sub(Expression* left, Expression* right) : left(left), right(right) {}
	friend class Div;
	friend Expression* build(string exp);
public:
	Sub(const Sub& that) {
		this->left = that.left->copy();
		this->right = that.right->copy();
	}
	virtual Expression* copy() const {
		return new Sub(this->left->copy(), this->right->copy());
	}
	virtual string to_string() const
	{
		string str = "";
		str += "(";
		str += left->to_string();
		str += "-";
		str += right->to_string();
		str += ")";
		return str;
	}
	~Sub()
	{
		delete left;
		delete right;
	}
	virtual Expression* derivative(string var) const
	{
		return new Sub(left->derivative(var), right->derivative(var));
	}
	virtual int eval(string eval) const
	{
		return left->eval(eval) - right->eval(eval);
	}
	virtual Expression* simplification() const
	{
		Expression* left_simpl = left->simplification();
		Expression* right_simpl = right->simplification();
		Number* right_number = dynamic_cast <Number*> (right_simpl);
		Number* left_number = dynamic_cast <Number*> (left_simpl);
		if (right_number && right_number->val == 0)
		{
			delete right_simpl;
			return left_simpl;
		}
		if (left_number && right_number)
		{
			Expression* result =  new Number(left_number->val - right_number->val);

			delete left_simpl;
			delete right_simpl;

			return result;
		}
		if (left_simpl->to_string() == right_simpl->to_string())
		{
			delete left_simpl;
			delete right_simpl;
			return new Number(0);
		}
		return new Sub(left_simpl, right_simpl);
	}
};
class Mul : public Expression
{
	Expression* left;
	Expression* right;
	Mul(Expression* left, Expression* right) : left(left), right(right) {}
	friend class Div;
	friend Expression* build(string exp);
public:
	Mul(const Mul& that) {
		this->left = that.left->copy();
		this->right = that.right->copy();
	}
	virtual Expression* copy() const {
		return new Mul(this->left->copy(), this->right->copy());
	}
	virtual string to_string() const
	{
		string str = "";
		str += "(";
		str += left->to_string();
		str += "*";
		str += right->to_string();
		str += ")";
		return str;
	}
	virtual Expression* derivative(string var) const
	{
		return new Add(new Mul(left->derivative(var),right->copy()), new Mul(left->copy(), right->derivative(var)));
	}
	~Mul()
	{
		delete left;
		delete right;
	}
	virtual int eval(string eval) const
	{
		return left->eval(eval) * right->eval(eval);
	}
	virtual Expression* simplification() const
	{
		Expression* left_simpl = left->simplification();
		Expression* right_simpl = right->simplification();
		Number* right_number = dynamic_cast <Number*> (right_simpl);
		Number* left_number = dynamic_cast <Number*> (left_simpl);
		if (left_number)
		{
			if (left_number->val == 0)
			{
				delete left_simpl;
				delete right_simpl;
				return new Number(0);
			}
			else if (left_number->val == 1)
			{
				delete left_simpl;
				return right_simpl;
			}
		}
		if (right_number)
		{
			if (right_number->val == 0)
			{
				delete left_simpl;
				delete right_simpl;
				return new Number(0);
			}
			else if (right_number->val == 1)
			{
				delete right_simpl;
				return left_simpl;
			}
		}
		if (left_number && right_number)
		{
			Expression* result =  new Number(left_number->val * right_number->val);

			delete left_simpl;
			delete right_simpl;

			return result;
		}
		return new Mul(left_simpl, right_simpl);
	}
};
class Div : public Expression 
{
	Expression* numerator;
	Expression* denominator;
	Div(Expression* left, Expression* right) : numerator(left), denominator(right) {}
	friend Expression* build(string exp);
public:
	Div(const Div& that) {
		this->numerator = that.numerator->copy();
		this->denominator = that.denominator->copy();
	}
	virtual Expression* copy() const {
		return new Div(this->numerator->copy(), this->denominator->copy());
	}
	virtual string to_string() const
	{
		string str = "";
		str += "(";
		str += numerator->to_string();
		str += "+";
		str += denominator->to_string();
		str += ")";
		return str;
	}
	virtual Expression* derivative(string var) const
	{
		return new Div(new Sub(new Mul(numerator->derivative(var),denominator->copy()), new Mul(denominator->derivative(var),numerator->copy())), new Mul (denominator->copy(), denominator->copy()));
	}
	~Div()
	{
		delete numerator;
		delete denominator;
	}
	virtual int eval(string eval) const
	{
		return numerator -> eval(eval) / denominator -> eval(eval);
	}
	virtual Expression* simplification() const
	{
		Expression* numerator_simpl = numerator->simplification();
		Expression* denominator_simpl = denominator->simplification();
		Number* denominator_number = dynamic_cast <Number*> (denominator_simpl);
		Number* numerator_number = dynamic_cast <Number*> (numerator_simpl);
		if (numerator_number && numerator_number->val == 0 && denominator && denominator_number -> val != 0)
		{
			delete numerator_simpl;
			delete denominator_simpl;
			return new Number(0);
		}
		if (denominator_number && denominator_number->val == 1)
		{
			delete denominator_simpl;
			return numerator_simpl;
		}
		if (numerator_number && denominator_number)
		{
			Expression* result = new Number(numerator_number->val / denominator_number->val);

			delete denominator_simpl;
			delete numerator_simpl;

			return result;
		}
		return new Div(numerator_simpl, denominator_simpl);
	}
};
Expression* build(string exp)
{
	if (exp.find('(') == string::npos)
	{
		if (isdigit(exp[0])) {
			return new Number(stoi(exp));
		}
		return new Variable(exp);
	}
	exp = exp.substr(1, exp.length() - 2);
	int cnt = 0;
	size_t i = 0;
	while (true)
	{
		if (exp[i] == '(')
		{
			cnt++;
		}
		if (exp[i] == ')')
		{
			cnt--;
		}
		if (cnt == 0 && (exp[i] == '+' || exp[i] == '*' || exp[i] == '/' || exp[i] == '-')) {
			break;
		}
		i++;
	}
	string left = exp.substr(0, i);
	string right = exp.substr(i + 1, exp.length() - i - 1);
	if (exp[i] == '+')
	{
		return new Add(build(left), build(right));
	}
	if (exp[i] == '*')
	{
		return new Mul(build(left), build(right));
	}
	if (exp[i] == '/')
	{
		return new Div(build(left), build(right));
	}
	return new Sub(build(left), build(right));
}

int priority(string simbol)
{
	if (simbol == "(")
		return 1;
	if (simbol == ")")
		return 2;
	if (simbol == "-" || simbol == "+")
		return 3;
	if (simbol == "/" || simbol == "*")
		return 4;
	return 5;
}

vector <string> split(string exp)
{
	string x;
	vector <string> res;
	for (size_t i = 0; i < exp.length(); i++)
	{
		x = "";
		if (isalpha(exp[i]))
		{
			x += exp[i];
			size_t j = 1;
			while (isalpha(exp[i + j]))
			{
				x += exp[i + j];
				j++;
			}
			res.push_back(x);
			i += j;
		}
		x = "";
		if (isdigit(exp[i]))
		{
			x += exp[i];
			size_t j = 1;
			while (isdigit(exp[i + j]))
			{
				x += exp[i + j];
				j++;
			}
			res.push_back(x);
			i += j ;
		}
		x = "";
		if (exp[i] == ')' || exp[i] == '(' || exp[i] == '+' || exp[i] == '-' || exp[i] == '*' || exp[i] == '/')
		{
			x = string(1, exp[i]);
			res.push_back(x);
		}
		x = "";
	}
	return res;
}

vector <string> RPN(vector <string> exp )
{
	stack <string> stack;
	string x;
	vector <string> res;
	for (size_t i = 0; i < exp.size(); i++)
	{
		x = exp[i];
		
		if (x == "(") 
		{
			stack.push(x);
			continue;
		}
		if (isalpha(x[0]))
		{

			res.push_back(x);
			continue;
		}
		if (x == ")")
		{
			while (stack.top() != "(")
			{
				res.push_back(stack.top());
				stack.pop();
			}
			stack.pop();
			continue;
		}
		if (stack.empty())  //пустой
		{
			stack.push(x);
		}
		else
		{
			while (!stack.empty() && (priority(stack.top()) >= priority(x)))
			{
				res.push_back(stack.top());
				stack.pop();
			}
			stack.push(x);
		}
	}
	while (!stack.empty())
	{
		res.push_back(stack.top());
		stack.pop();
	}
	return res;
}

string build_RPN(vector <string> exp)
{
	string res;
	size_t i = 0;
	while (exp.size() != 1)
	{
		if (exp[i] != "+" && exp[i] != "*" && exp[i] != "-" && exp[i] != "/") 
		{
			i++;
			continue;
		}
		exp[i] = "(" + exp[i - 2] + exp[i] + exp[i - 1] + ")";
		exp.erase(exp.begin() + i - 1);
		exp.erase(exp.begin() + i - 2);
		i = 0;
	}
	return exp[0];
}

Expression* build_expression(string exp)
{
	vector <string> exp_tokens = split(exp);
	vector <string> exp_tokens_RPN = RPN(exp_tokens);
	string expression_with_brackets = build_RPN(exp_tokens_RPN);
	Expression* res = build(expression_with_brackets);
	return res;
}

int main() 
{
	ifstream in("input.txt");
	string expression;
	in >> expression;
	Expression* exp = build_expression(expression);
	Expression* res = exp->derivative("x");
	in.close();
	ofstream out("output.txt");
	res->print(out);
	out.close();

	delete exp;
	delete res;

	return 0;

}


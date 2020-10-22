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
	virtual double eval (string eval) const = 0;
	virtual Expression* simplification() const = 0; 
};
class Number : public Expression 
{
	double val;
	friend Expression* build(string exp);

public:
	Number(double val) : val(val){}
	virtual string to_string() const
	{
		return std::to_string(val);
	}
	virtual Expression* derivative(string var) const
	{
		return new Number(0);
	}
	virtual double eval(string eval) const
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
	Variable(string var) : var(var){}
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
	virtual double eval(string eval) const
	{
		size_t i = eval.find(this -> var + " <- ");
		if (i == string::npos)
		{
			throw invalid_argument("incorrect eval");
		}
		string x = "";
		size_t delimeter = eval.find(";", i + 1);
		if (delimeter = string::npos)
		{
			delimeter = eval.length();
		}
		i += 4 + this-> var.length();
		x = eval.substr(i, delimeter - i);
		double res = stod(x);
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
	virtual double eval(string eval) const
	{
		return left -> eval(eval) + right -> eval(eval);
	}
	virtual Expression* simplification() const
	{
		Expression* left_simpl = left->simplification();
		Expression* right_simpl = right->simplification();
		Number* right_number = dynamic_cast <Number*> (right_simpl);
		Number* left_number = dynamic_cast <Number*> (left_simpl);
		if (right_number && right_number->eval("") == 0)
		{
			return left_simpl;
		}
		if (left_number && left_number->eval("") == 0)
		{
			return right_simpl;
		}
		if (left_number && right_number)
		{
			return new Number(left_number->eval("") + right_number->eval(""));
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
	virtual double eval(string eval) const
	{
		return left->eval(eval) - right->eval(eval);
	}
	virtual Expression* simplification() const
	{
		Expression* left_simpl = left->simplification();
		Expression* right_simpl = right->simplification();
		Number* right_number = dynamic_cast <Number*> (right_simpl);
		Number* left_number = dynamic_cast <Number*> (left_simpl);
		if (right_number && right_number->eval("") == 0)
		{
			return left_simpl;
		}
		if (left_number && right_number)
		{
			return new Number(left_number->eval("") - right_number->eval(""));
		}
		if (left_simpl->to_string() == right_simpl->to_string())
		{
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
		return new Add(new Mul(left->derivative(var),right), new Mul(left, right->derivative(var)));
	}
	~Mul()
	{
		delete left;
		delete right;
	}
	virtual double eval(string eval) const
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
			if (left_number->eval("") == 0)
			{
				return new Number(0);
			}
			else if (left_number->eval("") == 1)
			{
				return right_simpl;
			}
		}
		if (right_number)
		{
			if (right_number->eval("") == 0)
			{
				return new Number(0);
			}
			else if (right_number->eval("") == 1)
			{
				return left_simpl;
			}
		}
		if (left_number && right_number)
		{
			return new Number(left_number->eval("") * right_number->eval(""));
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
		return new Div(new Sub(new Mul(numerator->derivative(var),denominator), new Mul(denominator->derivative(var),numerator)), new Mul (denominator, denominator));
	}
	~Div()
	{
		delete numerator;
		delete denominator;
	}
	virtual double eval(string eval) const
	{
		return numerator -> eval(eval) / denominator -> eval(eval);
	}
	virtual Expression* simplification() const
	{
		Expression* numerator_simpl = numerator->simplification();
		Expression* denominator_simpl = denominator->simplification();
		Number* denominator_number = dynamic_cast <Number*> (denominator_simpl);
		Number* numerator_number = dynamic_cast <Number*> (numerator_simpl);
		if (numerator_number && numerator_number->eval("") == 0 && denominator_number -> eval("") != 0)
		{
			return new Number(0);
		}
		if (denominator_number && denominator_number->eval("") == 1)
		{
			return numerator_simpl;
		}
		if (numerator_number && denominator_number)
		{
			return new Number(numerator_number->eval("") / denominator_number->eval(""));
		}
		return new Div(numerator_simpl, denominator_simpl);
	}
};
Expression* build(string exp)
{
	if (exp.find('(') == string::npos)
	{
		try {
			return new Number(stod(exp));
		}
		catch (invalid_argument) 
		{
			return new Variable(exp);
		}
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
	out << endl;
	double res1 = res->eval("x <- 8; y <- 5; xy <- 8");
	out << res1 << endl;
	Expression* r = exp->simplification();
	r -> print(out);
	out.close();
	return 0;
}
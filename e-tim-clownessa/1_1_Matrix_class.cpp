#include <iostream>
#include <fstream>
#include <assert.h>
#include <time.h>
using namespace std;

class Matrix{
	int** data;
	int n;

	void init(int n) {
		data = new int*[n];
		for (int i = 0; i < n; i++) {
			data[i] = new int[n];
		}
	}

	void clean() {
		for (int i = 0; i < n; i++) {
			delete data[i];
		}
		delete data;
	}

	void copy(const Matrix& orig) {
		this->n = orig.n;
		init(this->n);
		for (int i = 0; i < n; i++)
			for (int j = 0; j < n; j++)
				this->data[i][j] = orig.data[i][j];
	}

public:
	//1. Constructors:
	Matrix(int n, int k) {
		this->n = n;
		init(n);
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				if (i == j)
					data[i][i] = k;
				else
					data[i][j] = 0;
			}
		}
	}

	Matrix(int n) {
		this->n = n;
		init(n);
		for (int i = 0; i < n; i++)
			for (int j = 0; j < n; j++)
				this->data[i][j] = rand();
	}

	Matrix(const Matrix& orig) {
		copy(orig);
	}

	//2. Dectructor
	~Matrix() {
		clean();
	}

	//3. Operators + and * :

	Matrix operator+ (const Matrix& that) const {
		if (this->n != that.n) {
			cout << "Error: different sizes";
			assert(0);
		}
		Matrix res(n);
		for (int i = 0; i < this->n; i++) {
			for (int j = 0; j < this->n; j++) {
				res.data[i][j] = this->data[i][j] + that.data[i][j];
			}
		}
		return res;
	}

	Matrix operator* (const Matrix& that) const {
		if (this->n != that.n) {
			cout << "Error: different sizes";
			assert(0);
		}
		Matrix res(n, 0);
		for (int i = 0; i < this->n; i++) {
			for (int j = 0; j < that.n; j++) {
				for (int k = 0; k < that.n; k++) {
					res.data[i][j] += this->data[i][k] * that.data[k][j];
				}
			}
		}
		return res;
	}

	//4. Operator =

	Matrix& operator= (const Matrix& that) {
		if (this != &that) {
			this->clean();
			this->copy(that);
		}
		return *this;
	}

	//5. Transp

	void transp() {
		int tmp;
		for (int i = 0; i < n; i++) {
			for (int j = i + 1; j < n; j++) {
				tmp = data[i][j];
				data[i][j] = data[j][i];
				data[j][i] = tmp;
			}
		}
	}

	void print() {
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				cout << data[i][j] << " ";
			}
			cout << endl;
		}
		cout << endl;
	}

	void fprint(ofstream& fout) {
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				fout << data[i][j] << " ";
			}
			fout << endl;
		}
		fout << endl;
	}

	void set(ifstream& fin) {
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				fin >> data[i][j];
			}
		}
	}
};

void main() {
	ifstream fin("input.txt");
	ofstream fout("output.txt");

	srand(time(0));
	int n, k;
	fin >> n >> k;
	Matrix a(n), b(n), c(n), d(n);
	Matrix diag(n, k);

	a.set(fin); b.set(fin); c.set(fin); d.set(fin);

	c.transp();
	d.transp();

	b = b * c;
	a = a + b + diag;
	a = a * d;

	a.fprint(fout);
	fin.close();
	fout.close();
}
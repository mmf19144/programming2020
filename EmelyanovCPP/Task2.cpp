#include <iostream>
#include <fstream>
#include <vector>
#include <memory>

using namespace std;

class IAnimal {

public:
	virtual void move(int maxx, int maxy) = 0;
	virtual void aging() = 0;
	virtual void breed(vector<unique_ptr<IAnimal>>& rabbits) = 0;
	virtual void extinction() = 0;
	virtual int getx() = 0;
	virtual int gety() = 0;
	virtual int getc() = 0;
	virtual int getst() = 0;
	virtual int getage() = 0;
	virtual void setst(int newst) = 0;
	virtual void eat(vector<unique_ptr<IAnimal>>& animals) = 0;
};

class Animal : public IAnimal {

protected:
	int x;
	int y;
	int direction;
	int	maxconstancy;
	int constancy;
	int speed;
	int age;
	int maxage;
	int st;

public:
	void move(int maxx, int maxy) override {
		
		if (direction == 0) {
			y = y - speed;
			if (y < 0)
				y = maxy + y;
		}
		if (direction == 1) {
			x = x + speed;
			if (x >= maxx)
				x = x - maxx;
		}
		if (direction == 2) {
			y = y + speed;
			if (y >= maxy)
				y = y - maxy;
		}
		if (direction == 3) {
			x = x - speed;
			if (x < 0)
				x = maxx + x;
		}

		constancy -= 1;
		if (constancy == 0) {
			constancy = maxconstancy;
			direction++;
		}
			if (direction > 3)
				direction = 0;
		
	}

	void aging() override {
		age++;
	}
	
	void extinction() override {
		if (age == maxage)
			st = 0;
	}

	int getx() override{
		return x;
	}
	int gety() override{
		return y;
	}
	int getc() override{
		return constancy;
	}
	int getst() override {
		return st;
	}
	int getage() override {
		return age;
	}
	void setst(int newst) override {
		st = newst;
	}
	void eat(vector<unique_ptr<IAnimal>>& animals) override {
	//nothing
	}
};


class Rabbit : public Animal {

public:
	Rabbit() {
		x = 0;
		y = 0;
		direction = 0;
		maxconstancy = 1;
		constancy = 0;
		speed = 1;
		age = 0;
		maxage = 10;
		st = 1;
	}

	Rabbit (int cx, int cy, int d, int p) {
		x = cx;
		y = cy;
		direction = d;
		maxconstancy = p;
		constancy = p;
		speed = 1;
		age = 0;
		maxage = 10;
		st = 1;
	}

	Rabbit(const Rabbit& R) {
		x = R.x;
		y = R.y;
		direction = R.direction;
		maxconstancy = R.maxconstancy;
		constancy = R.maxconstancy;
		speed = 1;
		age = 0;
		maxage = 10;
		st = 1;
	}

	void breed(vector<unique_ptr<IAnimal>>& rabbits) override {
		if (((age == 5)|| (age == 10)) && (st == 1)) {
			auto newrabbit = make_unique<Rabbit>(x, y, direction, maxconstancy);
			rabbits.push_back(std::move(newrabbit));
		}

	}	
};

class Wolf : public Animal {
protected:
	int satiety;
public:
	Wolf() {
		x = 0;
		y = 0;
		direction = 0;
		maxconstancy = 1;
		constancy = 0;
		speed = 1;
		age = 0;
		maxage = 15;
		satiety = 0;
		st = 1;
	}

	Wolf(int cx, int cy, int d, int p) {
		x = cx;
		y = cy;
		direction = d;
		maxconstancy = p;
		constancy = p;
		speed = 2;
		age = 0;
		maxage = 15;
		satiety = 0;
		st = 1;
	}
	Wolf(const Wolf& R) {
		x = R.x;
		y = R.y;
		direction = R.direction;
		maxconstancy = R.maxconstancy;
		constancy = R.maxconstancy;
		speed = 2;
		age = 0;
		maxage = 15;
		satiety = 0;
		st = 1;
	}

	void breed(vector<unique_ptr<IAnimal>>& wolfs) override{
		if ((satiety >= 2) && (st == 1)) {
			auto newwolf = make_unique<Wolf>(x, y, direction, maxconstancy);
			wolfs.push_back(std::move(newwolf));
			satiety = 0;
		}

	}

	void eat(vector<unique_ptr<IAnimal>>& rabbits) override {
		for (const auto& animal : rabbits) {
			if ((animal->getx() == x) && (animal->gety() == y) && (animal->getst() == 1))
			{
				animal->setst(0);
				satiety++;
			}
		}
	}
	
};

class Simulation {
private:
	size_t maxx;
	size_t maxy;
	size_t maxtime;
	size_t rnum;
	size_t wnum;
	vector<unique_ptr<IAnimal>> rabbits;
	vector<unique_ptr<IAnimal>> wolfs;

public:
	void set_simulation (size_t mx, size_t my, size_t mt, size_t rn, size_t wn ) {
		maxx = mx;
		maxy = my;
		maxtime = mt;
		rnum = rn;
		wnum = wn;
	}

	void set_rabbit(int cx, int cy, int d, int p) {
		auto newrabbit = make_unique<Rabbit>(cx, cy, d, p);
		rabbits.push_back(move(newrabbit));
	}

	void set_wolf(int cx, int cy, int d, int p) {
		auto newwolf = make_unique<Wolf>(cx, cy, d, p);
		wolfs.push_back(move(newwolf));
	}

	void start_simulation() {
		for (size_t i = 0; i < maxtime; i++) {

			for (const auto& animal : rabbits) {
				animal->move(maxx, maxy);
			}
			for (const auto& animal : wolfs) {
				animal->move(maxx, maxy);
			}

			for (const auto& animal : wolfs) {
				animal->eat(rabbits);
			}

			for (const auto& animal : rabbits) {
				animal->aging();
			}
			for (const auto& animal : wolfs) {
				animal->aging();
			}

			int nr = rabbits.size();
			for (int i = 0; i < nr; i++) {
				rabbits[i]->breed(rabbits);
			}
			int nw = wolfs.size();
			for (size_t i = 0; i < nw; i++) {
				wolfs[i]->breed(wolfs);
			}

			for (const auto& animal : rabbits) {
				animal->extinction();
			}
			for (const auto& animal : wolfs) {
				animal->extinction();
			}

			for (size_t i = 0; i < nr; i++) {
				if (rabbits[i]->getst() == 0) {
					rabbits.erase(rabbits.begin() + i);
					nr--;
				}
			}
			for (size_t i = 0; i < nw; i++) {
				if (wolfs[i]->getst() == 0) {
					wolfs.erase(wolfs.begin() + i);
					nw--;
				}
			}
		}
	}
	void set_field(int** field) {
		for (const auto& animal : rabbits) {
			if (animal->getst() != 0)
				field[animal->gety()][animal->getx()]++;
		}
		for (const auto& animal : wolfs) {
			if (animal->getst() != 0)
				field[animal->gety()][animal->getx()]--;
		}
	}

};

int main() {

	ifstream fin{ "input.txt" };
	ofstream fout{ "output.txt" };

	size_t maxx;
	size_t maxy;
	size_t maxtime;
	size_t rnum;
	size_t wnum;
	Simulation sim;
	
	fin >> maxx >> maxy >> maxtime >> rnum >> wnum;
	sim.set_simulation(maxx, maxy, maxtime, rnum, wnum);

	for (size_t i = 0; i < rnum; i++) {
		int cx,  cy,  d,  p;
		fin >> cx >> cy >> d >> p;
		
		sim.set_rabbit(cx, cy, d, p);

	}

	for (size_t i = 0; i < wnum; i++) {
		int cx, cy, d, p;
		fin >> cx >> cy >> d >> p;
		
		sim.set_wolf(cx, cy, d, p);
	}


	sim.start_simulation();

	int** field = new int*[maxy];

	for (size_t i = 0; i < maxy; i++)
		field[i] = new int [maxx];

	for (size_t i = 0; i < maxy; i++)
		for (size_t j = 0; j < maxy; j++)
			field[i][j] = 0;

	sim.set_field(field);

	for (size_t i = 0; i < maxy; i++) {
		for (size_t j = 0; j < maxy; j++){
			if (field[i][j] != 0)
				fout << field[i][j];
			else
				fout << "#";
		}
		fout << "\n";
	}
	for (size_t i = 0; i < maxy; i++)
		delete field[i];
	delete[] field;

	return 0;
}
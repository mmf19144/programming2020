#include <iostream>
#include <list>
#include <fstream>

enum class Direction { up, right, down, left };

Direction rotate(Direction & d)
{
	switch (d)
	{
	case Direction::up:
		d = Direction::right;
		break;
	case Direction::right:
		d = Direction::down;
		break;
	case Direction::down:
		d = Direction::left;
		break;
	case Direction::left:
		d = Direction::up;
		break;
	}
	return d;
}

class IAnimal
{
public:
	virtual void move(size_t N, size_t M) = 0;
	virtual bool breed() = 0;
	virtual bool isCorpse() const = 0;
	virtual ~IAnimal()
	{	}
};


class AnimalImpl : public IAnimal
{
protected:
	int x, y;
	Direction direction;
	size_t speed;
	size_t age;
	size_t constancy;
	size_t age_max;

public:
	AnimalImpl(int x, int y, size_t speed, int direction, size_t constancy, size_t age_max)
		: x{ x }
		, y{ y }
		, direction{ static_cast<Direction>(direction) }
		, speed{ speed }
		, age{ 0 }
		, constancy{ constancy }
		, age_max{ age_max }
	{	}

	AnimalImpl(const AnimalImpl & other)
		: x{ other.x }
		, y{ other.y }
		, direction{ other.direction }
		, speed{ other.speed }
		, age{ other.age }
		, constancy{ other.constancy }
		, age_max{ other.age_max }
	{	}

	void aging()
	{
		age++;
	}

	const std::pair<int, int> getCoordinates() const
	{
		return { x, y };
	}

	bool isCorpse() const override 
	{
		return age == age_max;
	}

	// N - количество строк, M - количество столбцов
	void move(size_t N, size_t M)
	{
		switch (direction)
		{
		case Direction::up:
			y -= speed;
			if (y < 0)
			{
				y += N;
			}
			break;
		case Direction::right:
			x += speed;
			if (x >= M)
			{
				x -= M;
			}
			break;
		case Direction::down:
			y += speed;
			if (y >= N)
			{
				y -= N;
			}
			break;
		case Direction::left:
			x -= speed;
			if (x < 0)
			{
				x += M;
			}
			break;
		}

		if (constancy == 1)
		{
			rotate(direction);
		}
		else
		{
			// if (age != 1 && (age - 1) % constancy == 0)
			if (age != 0 && (age + 1) % constancy == 0)
			{
				rotate(direction);
			}
		}
	}
};


class Rabbit : public AnimalImpl
{
public:
	Rabbit(int x, int y, int direction, size_t constancy) : AnimalImpl{ x, y, 1, direction, constancy, 10 }
	{	}

	Rabbit(const Rabbit & other) : AnimalImpl{ other }
	{	}

	Rabbit & operator=(const Rabbit & other)
	{
		x = other.x;
		y = other.y;
		direction = other.direction;
		constancy = other.constancy;
		age = other.age;
		return *this;
	}

	bool breed() 
	{
		return age != 0 && age % 5 == 0;
	}

	Rabbit getChild() const
	{
		Rabbit child{ *this };
		child.age = 0;
		return child;
	}
};


class Wolf : public AnimalImpl
{
	size_t satiety;

public:
	Wolf(int x, int y, int direction, size_t constancy) : AnimalImpl{ x, y, 2, direction, constancy, 15 }, satiety{ 0 }
	{	}

	Wolf(const Wolf & other) : AnimalImpl{ other }, satiety{ other.satiety }
	{	}

	Wolf & operator=(const Wolf & other)
	{
		x = other.x;
		y = other.y;
		direction = other.direction;
		constancy = other.constancy;
		age = other.age;
		satiety = other.satiety;
		return *this;
	}

	void eating()
	{
		satiety++;
	}

	bool breed()
	{
		if (satiety > 1)
		{
			satiety = 0;
			return true;
		}
		return false;
	}

	Wolf getChild() const
	{
		Wolf child{ *this };
		child.age = child.satiety = 0;
		return child;
	}
};


class Simulation
{
	const size_t N;
	const size_t M;
	std::list<Wolf> wolves;
	std::list<Rabbit> rabbits;

public:
	Simulation(size_t N, size_t M, std::list<Wolf> & wolves, std::list<Rabbit> & rabbits)
		: N{ N }
		, M{ M }
		, wolves{ wolves }
		, rabbits{ rabbits }
	{	}

	void start(const size_t T)
	{
		for (size_t i = 0; i < T; i++)
		{
			move();
			eating();
			aging();
			reproduction();
			extinction();
		}
	}

	void move()
	{
		for (auto & wolf : wolves)
		{
			wolf.move(N, M);
		}

		for (auto & rabbit : rabbits)
		{
			rabbit.move(N, M);
		}
	}

	void eating()
	{
		for (auto & wolf : wolves)
		{
			auto predicat = [wolf](Rabbit rabbit) {
				return wolf.getCoordinates() == rabbit.getCoordinates(); };
			
			std::find_if(rabbits.begin(), rabbits.end(), predicat);
			for (auto it = std::find_if(rabbits.begin(), rabbits.end(), predicat); it != rabbits.end();
				it = std::find_if(rabbits.begin(), rabbits.end(), predicat))
			{
				wolf.eating();
				rabbits.erase(it);
			}
		}
	}

	void aging()
	{
		for (auto & wolf : wolves)
		{
			wolf.aging();
		}
		for (auto & rabbit : rabbits)
		{
			rabbit.aging();
		}
	}

	void reproduction()
	{
		for (auto & wolf : wolves)
		{
			if (wolf.breed())
			{
				wolves.push_back(wolf.getChild());
			}
		}
		for (auto & rabbit : rabbits)
		{
			if (rabbit.breed())
			{
				auto temp = rabbit.getChild();
				rabbits.push_back(temp);
			}
		}
	}

	void extinction()
	{
		auto removeItWolves = std::remove_if(wolves.begin(), wolves.end(), [](Wolf wolf) {return wolf.isCorpse(); });
		wolves.erase(removeItWolves, wolves.end());

		auto removeItRabbits = std::remove_if(rabbits.begin(), rabbits.end(), [](Rabbit rabbit) {return rabbit.isCorpse(); });
		rabbits.erase(removeItRabbits, rabbits.end());
	}

	friend std::ostream & operator<<(std::ostream & out, Simulation & sim)
	{
		int ** table = new int * [sim.N];
		for (size_t i = 0; i < sim.N; i++)
		{
			table[i] = new int[sim.M];
		}

		for (size_t i = 0; i < sim.N; i++)
		{
			for (size_t j = 0; j < sim.M; j++)
			{
				table[i][j] = 0;
			}
		}

		for (const auto & wolf : sim.wolves)
		{
			auto coord = wolf.getCoordinates();
			table[coord.second][coord.first]--;
		}
		for (const auto & rabbit : sim.rabbits)
		{
			auto coord = rabbit.getCoordinates();
			table[coord.second][coord.first]++;
		}

		for (size_t i = 0; i < sim.N; i++)
		{
			for (size_t j = 0; j < sim.M; j++)
			{
				if (table[i][j] == 0)
				{
					out << '#';
				}
				else
				{
					out << table[i][j];
				}
			}
			out << std::endl;
		}

		for (size_t i = 0; i < sim.N; i++)
		{
			delete[] table[i];
		}
		delete[] table;
		return out;
	}
};


int main()
{
	std::ifstream fin{ "input.txt" };
	std::ofstream fout{ "output.txt" };

	if (!fin || !fout)
	{
		std::cout << "НЕту ФаЙла!" << std::endl;
		std::terminate();
	}

	size_t N, M, T;
	fin >> N >> M >> T;

	size_t count_wolves, count_rabbits;
	fin >> count_rabbits >> count_wolves;

	std::list<Wolf> wolves;
	std::list<Rabbit> rabbits;

	for (size_t i = 0; i < count_rabbits; i++)
	{
		int x, y, d;
		size_t k;
		fin >> x >> y >> d >> k;
		rabbits.push_back(Rabbit{ x, y, d, k });
	}

	for (size_t i = 0; i < count_wolves; i++)
	{
		int x, y, d;
		size_t k;
		fin >> x >> y >> d >> k;
		wolves.push_back(Wolf{ x, y, d, k });
	}

	Simulation simulation{ N, M, wolves, rabbits };
	simulation.start(T);
	fout << simulation;
}

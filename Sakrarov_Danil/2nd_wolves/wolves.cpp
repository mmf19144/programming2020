#include <iostream>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <algorithm>

enum class Destination {
    Right, Up, Left, Down
};
enum class Type {
    Wolf, Rabbit
};


Destination make_dist(size_t d) {
    switch (d) {
        case 0:
            return Destination::Up;
        case 1:
            return Destination::Right;
        case 2:
            return Destination::Down;
        case 3:
            return Destination::Left;
        default:
            break;
    }
}

Destination operator++(Destination &dest) {
    switch (dest) {
        case Destination::Right:
            return Destination::Up;
        case Destination::Up:
            return Destination::Left;
        case Destination::Left:
            return Destination::Down;
        case Destination::Down:
            return Destination::Right;
    }
}

struct Coords {
    size_t x;
    size_t y;
};

Coords make_cord(size_t a, size_t b) {
    Coords tmp{};
    tmp.x = a;
    tmp.y = b;
    return tmp;
}

class IAnimal {

public:

    virtual Coords get_position() = 0;

    virtual size_t get_age() = 0;

    virtual Coords moving() = 0;

    virtual void change_direction() = 0;

    virtual bool must_die() = 0;

    virtual bool is_dead() = 0;

    virtual void aging() = 0;

    virtual Type get_type() = 0;

    virtual size_t age_of_father() = 0;

    virtual void dying() = 0;

    virtual bool must_multiply() = 0;

    int selectedInStep = -1;
};


class Simulation {
private:
    std::vector<IAnimal *> animals;

    std::vector<std::vector<std::vector<IAnimal *>>> field;
    const size_t height;
    const size_t width;
    size_t cur_step = 0;
    const size_t max_steps = 0;
public:
    Simulation(const size_t height_, const size_t width_, const size_t max_step) : height(height_), width(width_),
                                                                                   max_steps(max_step) {
        field.resize(height_);
        for (int i = 0; i < height_; i++)
            field[i].resize(width_);
    }

    Coords board{};

    friend std::ostream &operator<<(std::ostream &os, const Simulation &m) {
        for (size_t y = 0; y < m.height; y++) {
            for (size_t x = 0; x < m.width; x++) {

                int size = 0;
                for (auto j : m.field[y][x]) {
                    switch (j->get_type()) {

                        case Type::Rabbit:
                            size++;
                            break;
                        case Type::Wolf:
                            size--;
                            break;
                    }

                }
                if (size == 0) os << '#';
                else os << size;
            }
            if (y < m.height - 1)
                os << std::endl;
        }
        return os;
    }
    void move();
    void eat();
    void age_and_multiply_and_die();
    void step();


    void run() {
        for (int s = 0; s < max_steps; s++) {
            step();
        }

    }

    void spawn_animal(IAnimal *animal) {
        animals.push_back(animal);
        Coords pos = animal->get_position();
        field[pos.y][pos.x].push_back(animal);
    }

    size_t get_current_step() const {
        return cur_step;
    }

    size_t get_width() const {
        return width;
    }

    size_t get_height() const {
        return height;
    }

    const IAnimal *get_parent(const size_t id) const {
        return animals[id];
    }

};


class Animal : public IAnimal {
protected:
    Simulation &simul;
    Coords cord;
    Destination destination;
    size_t constancy;
    bool _is_dead = false;
    bool _must_multiply = false;
    size_t age = 0;
    size_t max_age;
    size_t range_of_movement;
    Type animal_type;
    size_t father_s_age;
    const size_t birth_step;
public:
    Animal(Simulation &_simul, size_t _range_of_movement, Destination _destination, size_t _constancy,
           Coords _cord, size_t _max_age, Type _type) : simul(_simul), cord(_cord), destination(_destination),
                                                        constancy(_constancy),
                                                        max_age(_max_age),
                                                        range_of_movement(_range_of_movement), animal_type(_type),
                                                        father_s_age(0), birth_step(0) {};

    Animal(Simulation &_simul, Animal *parent) : simul(_simul), cord(parent->cord), destination(parent->destination),
                                                 constancy(parent->constancy),
                                                 max_age(parent->max_age), range_of_movement(parent->range_of_movement),
                                                 animal_type(parent->animal_type), father_s_age(parent->age), birth_step(simul.get_current_step() ){};



    bool operator<(Animal &other) {
        if (this->get_type() != other.get_type()) {
            if (this->get_type() == Type::Rabbit && other.get_type() == Type::Wolf)
            return 1;
            else
                return 0;
        }
        if (this->age > other.age)
            return false;
        else if (this->age < other.age)
            return true;
        else {
            return this->age_of_father() <
                   other.age_of_father();
        }
    }

    size_t get_age() override {
        return age;
    }

    bool must_multiply() override {
        return _must_multiply;
    }

    Coords moving() override {
        switch (destination) {
            case Destination::Up:
                cord.y -= range_of_movement;
                break;
            case Destination::Down:
                cord.y += range_of_movement;
                break;
            case Destination::Right:
                cord.x += range_of_movement;
                break;
            case Destination::Left:
                cord.x -= range_of_movement;
                break;
        }
        cord.x += simul.get_width();
        cord.x %= simul.get_width();
        cord.y += simul.get_height();
        cord.y %= simul.get_height();
        if ((simul.get_current_step() - birth_step) > 0 && (simul.get_current_step() - birth_step) % constancy == 0) {
            switch (destination) {
                case Destination::Left: {
                    destination = Destination::Up;
                    break;
                }

                case Destination::Up: {
                    destination = Destination::Right;
                    break;
                }
                case Destination::Down: {
                    destination = Destination::Left;
                    break;
                }

                case Destination::Right: {
                    destination = Destination::Down;
                    break;
                }


            }
        }
        return get_position();
    }



    void aging() override {
        age++;
    }

    void change_direction() override {
        if (age % constancy == 0) {
            switch (destination) {
                case Destination::Right:
                    destination = Destination::Up;
                    break;
                case Destination::Up:
                    destination = Destination::Left;
                    break;
                case Destination::Left:
                    destination = Destination::Down;
                    break;
                case Destination::Down:
                    destination = Destination::Right;
                    break;
            }
        }

    }


    Coords get_position() override {
        return cord;
    }

    bool is_dead() override {
        return _is_dead;
    }

    bool must_die() override {
        _is_dead = age >= max_age;
        return is_dead();
    }

    Type get_type() override {
        return animal_type;
    }

    size_t age_of_father() override {
        return father_s_age;
    }

    void dying() override {
        _is_dead = true;
    }


};

class Rabbit : public Animal {
private:
    const size_t reproduction_step = 5;

public:
    Rabbit(Animal *parent, Simulation &simul) : Animal(simul, parent) {};

    Rabbit(Coords cords, Destination _destination, size_t _constancy, Simulation &_simul) :
            Animal(_simul, 1, _destination, _constancy, cords, 10, Type::Rabbit) {};

    bool must_multiply() override {
        return !_is_dead && age % reproduction_step == 0;
    }


};

class Wolf : public Animal {
private:
    const size_t need_kills = 2;
    size_t iteration_kills = 0;
    bool is_hungry = true;
public:
    Wolf(Animal *parent, Simulation &simul) : Animal(simul, parent) {};

    Wolf(Coords cords, Destination _destination, size_t _constancy, Simulation &_simul) :
            Animal(_simul, 2, _destination, _constancy, cords, 15, Type::Wolf) {};

    void increase_kills() {
        iteration_kills++;
    }

    void kill() {
        increase_kills();
        if (iteration_kills >= need_kills) {
            is_hungry = false;
        }
    }

    bool must_multiply() override {
        return !is_hungry;
    }

    void hungry_again() {
        iteration_kills = 0;
        is_hungry = true;
    }


};




std::istream &operator>>(std::istream &os, Simulation &_simul) {
    int r = 0;
    int w = 0;
    os >> r >> w;

    for (int t = 0; t < 2; t++) {
        int max_s = 0;
        if (t == 0)
            max_s = r;
        else if (t == 1)
            max_s = w;


        for (int i = 0; i < max_s; i++) {
            int x, y, d, k;
            Destination dist;
            Coords cord;
            os >> x >> y >> d >> k;

            cord = make_cord(x, y);
            dist = make_dist(d);

            if (t == 0)
                _simul.spawn_animal(new Rabbit(cord, dist, k, _simul));
            else if (t == 1)
                _simul.spawn_animal(new Wolf(cord, dist, k, _simul));
        }
    }
    return os;
}


void Simulation::move() {
    for (auto *animal: animals) {
        if (animal->is_dead())
            continue;
        Coords old_position = animal->get_position();

        for (int i = 0; i < field[old_position.y][old_position.x].size(); i++) {
            if (field[old_position.y][old_position.x][i] == animal) {
                field[old_position.y][old_position.x].erase(field[old_position.y][old_position.x].begin() + i);
                break;
            }
        }
        Coords new_pos = animal->moving();
        field[new_pos.y][new_pos.x].push_back(animal);
    }
}

void Simulation::eat() {
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            while (true) {
                size_t field_size = field[y][x].size();
                if (!field_size || field_size == 1) break;
                sort(field[y][x].begin(), field[y][x].end(), [](const IAnimal *lhs, const IAnimal *rhs) {
                    return *(Animal *) rhs < *(Animal *) lhs;
                });
                Animal *oldest_animal = nullptr;
                bool has_hungry_animal = false;
                for (int j = 0; j < field_size; j++) {
                    if (field[y][x][j]->selectedInStep == cur_step) continue;
                    switch (field[y][x][j]->get_type()) {
                        case Type::Rabbit: {
                            has_hungry_animal = false;
                        }
                        case Type::Wolf: {
                            if (field[y][x][j]->get_type() == Type::Wolf) {
                                oldest_animal = (Animal *) field[y][x][j];
                                has_hungry_animal = true;
                            } else {
                                has_hungry_animal = false;
                            }
                        }
                    }
                    if (has_hungry_animal) break;
                }
                if (oldest_animal == nullptr)
                    break;
                oldest_animal->selectedInStep = cur_step;
                bool has_kill = false;
                for (int j = 0; j < field[y][x].size(); j++) {
                    if (oldest_animal == field[y][x][j]) continue;
                    if (oldest_animal->get_type() == Type::Rabbit) break;
                    switch (oldest_animal->get_type()) {
                        case Type::Rabbit:
                            break;
                        case Type::Wolf: {
                            if (field[y][x][j]->get_type() == Type::Rabbit) {
                                field[y][x][j]->dying();
                                field[y][x].erase(field[y][x].begin() + j);
                                j--;
                                ((Wolf *) oldest_animal)->kill();
                                has_kill = true;
                            }
                            break;
                        }

                    }
                }
                if (!has_kill) break;
            }

        }
    }
}

void Simulation::age_and_multiply_and_die() {
    for (auto *animal: animals) {
        if (animal->is_dead()) continue;
        animal->aging();
        if (animal->must_multiply()) {
            switch (animal->get_type()) {
                case Type::Rabbit: {
                    spawn_animal(new Rabbit((Animal *) animal, *this));
                    break;
                }
                case Type::Wolf: {
                    spawn_animal(new Wolf((Animal *) animal, *this));
                    ((Wolf *) animal)->hungry_again();
                    break;
                }
            }
        }
        // dying
        if (animal->must_die()) {
            Coords old_position = animal->get_position();
            for (int i = 0; i < field[old_position.y][old_position.x].size(); i++) {
                if (field[old_position.y][old_position.x][i] == animal) {
                    field[old_position.y][old_position.x].erase(field[old_position.y][old_position.x].begin() + i);
                    break;
                }
            }
            animal->dying();
        }
    }
}

void Simulation::step() {
    cur_step++;
    move();
    eat();
    age_and_multiply_and_die();
}



int main() {
    std::ofstream fout("output.txt");
    std::ifstream fin("input.txt");

    size_t H, W, steps;

    fin >> H >> W >> steps;

    Simulation test(H, W, steps);

    fin >> test;
    test.run();
    fout << test << std::endl;
    return 0;
}

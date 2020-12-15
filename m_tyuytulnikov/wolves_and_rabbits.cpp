#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

enum Animal_type {
    WOLF,
    RABBIT,
    HYENA
};

enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

class Animal {
    const size_t birth_tick;
    size_t steps_done;
    const size_t velocity;
    const size_t animal_id;
    Animal *const parent;
    size_t age;

protected:
    int x;
    int y;
    const Animal_type animal_type; //0 for wolf, 1 for rabbit, 2 for hyena
    size_t saturation;
    Direction direction;
    const size_t constancy;
    size_t last_duplication;

public:


    size_t getAge() {
        return age;
    }

    Animal_type getType() {
        return animal_type;
    }

    void resetSaturation() {
        saturation = 0;
    }

    size_t getSaturation() {
        return saturation;
    }

    size_t getBirthTick() {
        return birth_tick;
    }

    Animal(int x, int y, Direction dir, size_t constancy, size_t birth, size_t vel, size_t vector_id,
           Animal_type animal_type,
           Animal *parent)
            : x(x), y(y), direction(dir), constancy(constancy), birth_tick(birth), velocity(vel),
              steps_done(0), animal_id(vector_id),
              animal_type(animal_type), parent(parent),
              saturation(0), age(0), last_duplication(0) {
    }

    ~Animal()= default;

    pair<size_t, size_t> move(size_t x_size, size_t y_size) {
        age++;
        switch (direction) {
            case UP:
                y -= velocity;
                break;
            case RIGHT:
                x += velocity;
                break;
            case DOWN:
                y += velocity;
                break;
            case LEFT:
                x -= velocity;
                break;
        }
        if (x >= (int) x_size) {
            x -= x_size;
        } else if (x < 0) {
            x += x_size;
        }

        if (y >= (int) y_size) {
            y -= y_size;
        } else if (y < 0) {
            y += y_size;
        }
        steps_done++;
        if (steps_done >= constancy) {
            steps_done = 0;
            switch (direction) {
                case UP:
                    direction = RIGHT;
                    break;
                case RIGHT:
                    direction = DOWN;
                    break;
                case DOWN:
                    direction = LEFT;
                    break;
                case LEFT:
                    direction = UP;
                    break;
            }
        }
        auto p = make_pair<size_t, size_t>((size_t) x, (size_t) y);
        return p;
    }

    virtual void eat(vector<Animal *> &animals) = 0;

    virtual void duplicate(vector<Animal *> &animals, size_t tick) = 0;

    virtual bool needToBeKilled() = 0;

    bool operator>(const Animal &other) {
        if (birth_tick < other.birth_tick) return true;
        if (birth_tick > other.birth_tick) return false;

        if (parent != nullptr && other.parent != nullptr) {
            return (parent > other.parent);
        } else {
            return (animal_id < other.animal_id);
        }
    }

    bool operator<(const Animal &other) {
        return !((*this) > other);
    }

    static bool compareAnimalPointers(Animal *l, Animal *r) {
        return *l > *r; //uses to descending sort vector of animal pointer
    }

};


class Wolf : public Animal {
public:
    Wolf(int x, int y, Direction dir, size_t constancy, size_t birth, size_t vector_id, Animal *parent)
            : Animal(x, y, dir, constancy, birth, 2, vector_id, WOLF, parent) {
    }

    void eat(vector<Animal *> &animals) override {
        auto iter = animals.begin();
        while (iter != animals.end()) {
            if ((*iter)->getType() == RABBIT) {
                saturation++;
                iter = animals.erase(iter);
            } else {
                ++iter;
            }
        }
    }

    void duplicate(vector<Animal *> &animals, size_t tick) override {
        if (getSaturation() >= 2 && last_duplication < tick) { //WOLF
            resetSaturation();
            last_duplication = tick;
            auto *new_wolf = new Wolf(this->x, y, direction, constancy, tick, 0, this);
            animals.emplace_back(new_wolf);
        }
    }
    bool needToBeKilled() override{
        return getAge() >= 15;
    }
};

class Rabbit : public Animal {
public:
    Rabbit(int x, int y, Direction dir, size_t constancy, size_t birth, size_t vector_id, Animal *parent)
            : Animal(x, y, dir, constancy, birth, 1, vector_id, RABBIT, parent) {
    }

    void eat(vector<Animal *> &animals) override {}

    void duplicate(vector<Animal *> &animals, size_t tick) override {
        if (last_duplication < tick && (getAge() == 5 || getAge() == 10)) {
            last_duplication = tick;
            auto *new_rabbit = new Rabbit(x, y, direction, constancy,
                                          tick, 0, this);
            animals.emplace_back(new_rabbit);
        }
    }

    bool needToBeKilled() override{
        return getAge() >= 10;
    }
};

class Hyena : public Animal {
public:
    Hyena(int x, int y, Direction dir, size_t constancy, size_t birth, size_t vector_id, Animal *parent)
            : Animal(x, y, dir, constancy, birth, 2, vector_id, HYENA, parent) {
    }

    void eat(vector<Animal *> &animals) override {
        auto iter = animals.begin();
        for (size_t i = 0; i < 2; i++) {
            if ((*iter) != this) {
                iter = animals.erase(iter);
                saturation++;
            } else if (iter != animals.end()) {
                iter = animals.erase(iter + 1);
                saturation++;
            } else {
                return;
            }
        }
    }

    void duplicate(vector<Animal *> &animals, size_t tick) override {
        if (getSaturation() >= 2 && last_duplication < tick) {
            resetSaturation();
            last_duplication = tick;
            auto *new_hyena = new Hyena(x, y, direction, constancy,
                                        tick, 0, this);
            animals.emplace_back(new_hyena);
        }
    }

    bool needToBeKilled() override{
        return getAge() >= 15;
    }

};

class Simulation {
    const size_t x_size;
    const size_t y_size;
    vector<Animal *> *field;
    vector<Animal *> animals; //contains all animals till the deconstruction of simulation
public:
    Simulation(size_t x_size, size_t y_size, size_t wolves_num, size_t rabbits_num, size_t hyenas_num,
               size_t *wolves_x, size_t *wolves_y, size_t *wolves_dir, size_t *wolves_const,
               size_t *rabbits_x, size_t *rabbits_y, size_t *rabbits_dir, size_t *rabbits_const,
               size_t *hyenas_x, size_t *hyenas_y, size_t *hyenas_dir, size_t *hyenas_const)
            : x_size(x_size), y_size(y_size), field(new vector<Animal *>[x_size * y_size]) {
        for (size_t i = 0; i < wolves_num; i++) {
            auto *new_wolf = new Wolf(wolves_x[i], wolves_y[i], (Direction) wolves_dir[i],
                                      wolves_const[i], 0, i, nullptr);
            animals.emplace_back(new_wolf);
            field[wolves_x[i] + x_size * wolves_y[i]].emplace_back(new_wolf);
        }
        for (size_t i = 0; i < rabbits_num; i++) {
            auto *new_rabbit = new Rabbit(rabbits_x[i], rabbits_y[i], (Direction) rabbits_dir[i],
                                          rabbits_const[i], 0, wolves_num + i, nullptr);
            animals.emplace_back(new_rabbit);
            field[rabbits_x[i] + x_size * rabbits_y[i]].emplace_back(new_rabbit);
        }
        for (size_t i = 0; i < hyenas_num; i++) {
            auto *new_hyena = new Hyena(hyenas_x[i], hyenas_y[i], (Direction) hyenas_dir[i],
                                        hyenas_const[i], 0,
                                        wolves_num + rabbits_num + i, nullptr);
            animals.emplace_back(new_hyena);
            field[rabbits_x[i] + x_size * rabbits_y[i]].emplace_back(new_hyena);
        }

    }

    void print_field(std::ostream &output) {
        for (size_t y = 0; y < y_size; y++) {
            for (size_t x = 0; x < x_size; x++) {
                if (!field[x + y * x_size].empty()) {
                    int multiply = 1;
                    if (field[x + y * x_size].back()->getType() == WOLF) multiply = -1;
                    output << multiply * (int) field[x + y * x_size].size();
                } else {
                    output << "#";
                }
            }
            output << endl;
        }
    }

    void calculate(size_t total_ticks, std::ostream &output) {
        for (size_t tick = 1; tick <= total_ticks; tick++) {
            move_animals(tick);
            eat_animals();
            duplicate_animals(tick);
            kill_animals(tick);
        }
        print_field(output);
    }

    ~Simulation() {
        for (auto const &animal: animals) { //deletes ALL animals
            delete (animal);
        }
        delete[] field;
    }

private:
    void move_animals(size_t tick) {
        for (size_t x = 0; x < x_size; x++) {
            for (size_t y = 0; y < y_size; y++) {
                vector<Animal *> &cur_cell = field[x + y * x_size];
                auto iter = cur_cell.begin();
                while (iter != cur_cell.end()) {
                    if ((*iter)->getAge() < tick - (*iter)->getBirthTick()) {
                        pair<size_t, size_t> new_coords =
                                (*iter)->move(x_size, y_size); //Get new coords of animal after moving
                        if (new_coords.first == x && new_coords.second == y) ++iter;
                        else {
                            //Moving animal from one vector to another by removing and pushing it
                            field[new_coords.first + new_coords.second * x_size].emplace_back((*iter));
                            iter = cur_cell.erase(iter);
                        }
                    } else {
                        ++iter;
                    }
                }
            }
        }
    }


    void eat_animals() {
        for (size_t x = 0; x < x_size; x++) {
            for (size_t y = 0; y < y_size; y++) {
                vector<Animal *> &cur_cell = field[x + y * x_size];
                sort(cur_cell.begin(), cur_cell.end(), Animal::compareAnimalPointers);
                //DESCENDING sort
                auto eater = cur_cell.begin();
                while (eater != cur_cell.end()) {
                    Animal *cur_animal = (*eater);
                    (*eater)->eat(cur_cell);
                    eater = std::find(cur_cell.begin(), cur_cell.end(), cur_animal);
                    ++eater;
                }
            }
        }
    }

    void duplicate_animals(size_t tick) {
        for (size_t x = 0; x < x_size; x++) {
            for (size_t y = 0; y < y_size; y++) {
                vector<Animal *> &cur_cell = field[x + y * x_size];
                auto iter = cur_cell.begin();
                while (iter != cur_cell.end()) {
                    Animal* cur_animal = *iter;
                    (*iter)->duplicate(cur_cell, tick);
                    iter = std::find(cur_cell.begin(), cur_cell.end(), cur_animal);
                    ++iter;
                }
            }
        }
    }

    void kill_animals(size_t tick) {
        for (size_t x = 0; x < x_size; x++) {
            for (size_t y = 0; y < y_size; y++) {
                auto iter = field[x + y * x_size].begin();
                while (iter != field[x + y * x_size].end()) {
                    if ((*iter)->needToBeKilled()){
                        iter = field[x + y * x_size].erase(iter);
                    } else
                        ++iter;
                }
            }
        }
    }


};

int main() {
    std::ifstream input("input.txt");
    std::ofstream output("output.txt");

    int x_size, y_size, ticks;
    input >> x_size >> y_size >> ticks;
    int r, w;
    input >> r >> w;
    auto *rabbits_x = new size_t[r];
    auto *rabbits_y = new size_t[r];
    auto *rabbits_d = new size_t[r];
    auto *rabbits_k = new size_t[r];

    for (size_t i = 0; i < r; i++) {
        input >> rabbits_x[i] >> rabbits_y[i] >> rabbits_d[i] >> rabbits_k[i];
    }

    auto *wolves_x = new size_t[r];
    auto *wolves_y = new size_t[r];
    auto *wolves_d = new size_t[r];
    auto *wolves_k = new size_t[r];

    for (size_t i = 0; i < w; i++) {
        input >> wolves_x[i] >> wolves_y[i] >> wolves_d[i] >> wolves_k[i];
    }

    Simulation simulation(x_size, y_size, w, r, 0,
                          wolves_x, wolves_y, wolves_d, wolves_k,
                          rabbits_x, rabbits_y, rabbits_d, rabbits_k,
                          nullptr, nullptr, nullptr, nullptr);

    simulation.calculate(ticks, output);

    input.close();
    output.close();
    return 0;
}


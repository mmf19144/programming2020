#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

class Animal {
    int x;
    int y;
    const size_t birth_tick;
    size_t steps_done;
    const size_t velocity;
    const size_t animal_id;
    Animal *const parent;
    size_t age;

protected:
    const size_t animal_type; //0 for wolf, 1 for rabbit, 2 for hyena
    size_t saturation;

public:
    size_t direction;
    const size_t constancy;
    size_t last_duplication;

    size_t getAge() {
        return age;
    }

    size_t getType() {
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

    Animal(int x, int y, size_t dir, size_t constancy, size_t birth, size_t vel, size_t vector_id, size_t animal_type,
           Animal *parent)
            : x(x), y(y), direction(dir), constancy(constancy), birth_tick(birth), velocity(vel),
              steps_done(0), animal_id(vector_id),
              animal_type(animal_type), parent(parent),
              saturation(0), age(0), last_duplication(0) {
    }

    pair<size_t, size_t> move(size_t x_size, size_t y_size) {
        age++;
        switch (direction) {
            case 0:
                //UP
                y -= velocity;
                break;
            case 1:
                //RIGHT
                x += velocity;
                break;
            case 2:
                //DOWN
                y += velocity;
                break;
            case 3:
                //LEFT
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
            direction++;
            if (direction > 3) {
                direction -= 4;
            }
        }
        pair<size_t, size_t> p;
        p.first = (size_t) x;
        p.second = (size_t) y;
        return p;
    }

    virtual void eat(Animal *animal) = 0;

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

};

class Wolf : public Animal {
public:
    Wolf(int x, int y, size_t dir, size_t constancy, size_t birth, size_t vector_id, Animal *parent)
            : Animal(x, y, dir, constancy, birth, 2, vector_id, 0, parent) {
    }

    void eat(Animal *animal) override {
        if (animal->getType() == 1) {
            saturation++;
        }
    }

};

class Rabbit : public Animal {
public:
    Rabbit(int x, int y, size_t dir, size_t constancy, size_t birth, size_t vector_id, Animal *parent)
            : Animal(x, y, dir, constancy, birth, 1, vector_id, 1, parent) {
    }

    void eat(Animal *animal) override {
    }
};

class Hyena : public Animal {
public:
    Hyena(int x, int y, size_t dir, size_t constancy, size_t birth, size_t vector_id, Animal *parent)
            : Animal(x, y, dir, constancy, birth, 2, vector_id, 2, parent) {
    }

    void eat(Animal *animal) override {
        saturation++;
    }
};

class Simulation {
    const size_t x_size;
    const size_t y_size;
    vector<Animal *> *field;
public:
    Simulation(size_t x_size, size_t y_size, size_t wolves_num, size_t rabbits_num, size_t hyenas_num,
               size_t *wolves_x, size_t *wolves_y, size_t *wolves_dir, size_t *wolves_const,
               size_t *rabbits_x, size_t *rabbits_y, size_t *rabbits_dir, size_t *rabbits_const,
               size_t *hyenas_x, size_t *hyenas_y, size_t *hyenas_dir, size_t *hyenas_const)
            : x_size(x_size), y_size(y_size), field(new vector<Animal *>[x_size * y_size]) {
        for (size_t x = 0; x < x_size; x++) {
            for (size_t y = 0; y < y_size; y++) {
                field[x + y * x_size].clear();
            }
        }
        for (size_t i = 0; i < wolves_num; i++) {
            auto *new_wolf = new Wolf(wolves_x[i], wolves_y[i], wolves_dir[i],
                                      wolves_const[i], 0, i, nullptr);
            field[wolves_x[i] + x_size * wolves_y[i]].emplace_back(new_wolf);
        }
        for (size_t i = 0; i < rabbits_num; i++) {
            auto *new_rabbit = new Rabbit(rabbits_x[i], rabbits_y[i], rabbits_dir[i],
                                          rabbits_const[i], 0, i, nullptr);
            field[rabbits_x[i] + x_size * rabbits_y[i]].emplace_back(new_rabbit);
        }
        for (size_t i = 0; i < hyenas_num; i++) {
            auto *new_hyena = new Hyena(hyenas_x[i], hyenas_y[i], hyenas_dir[i],
                                          hyenas_const[i], 0, i, nullptr);
            field[rabbits_x[i] + x_size * rabbits_y[i]].emplace_back(new_hyena);
        }

    }

    void print_field(std::ostream &output) {
        for (size_t y = 0; y < y_size; y++) {
            for (size_t x = 0; x < x_size; x++) {
                if (!field[x + y * x_size].empty()) {
                    int multiply = 1;
                    if (field[x + y * x_size].back()->getType() == 0) multiply = -1;
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

private:
    void move_animals(size_t tick) {
        for (size_t x = 0; x < x_size; x++) {
            for (size_t y = 0; y < y_size; y++) {
                auto iter = field[x + y * x_size].begin();
                while (iter != field[x + y * x_size].end()) {
                    if ((*iter)->getAge() < tick - (*iter)->getBirthTick()) {
                        pair<size_t, size_t> new_coords =
                                (*iter)->move(x_size, y_size); //Get new coords of animal after moving
                        if (new_coords.first == x && new_coords.second == y) ++iter;
                        else {
                            //Moving animal from one vector to another by removing and pushing it
                            field[new_coords.first + new_coords.second * x_size].emplace_back((*iter));
                            iter = field[x + y * x_size].erase(iter);
                        }
                    } else {
                        ++iter;
                    }
                }
            }
        }
    }


    static Animal *find_eldest_animal_by_type(vector<Animal *> &animals, size_t type) {
        int max_id = -1;
        for (size_t id = 0; id < animals.size(); id++) {
            if (animals[id]->getType() == type) {
                max_id = id;
                break;
            }
        }
        for (size_t id = max_id + 1; id < animals.size(); id++) {
            if (animals[id]->getType() == type && animals[id] > animals[max_id]) {
                max_id = id;
            }
        }
        if (max_id > -1) return animals[max_id];
        else return nullptr;
    }


    void eat_animals() {
        for (size_t x = 0; x < x_size; x++) {
            for (size_t y = 0; y < y_size; y++) {
                sort(field[x + y * x_size].begin(), field[x + y * x_size].end());
                Animal *eldest_hyena = find_eldest_animal_by_type(field[x + y * x_size], 0);
                if (eldest_hyena != nullptr) {
                    for (size_t i = 0; i < 2; i++) { //eat 2 oldest animals
                        if (eldest_hyena != field[x + y * x_size].back()) {
                            eldest_hyena->eat(field[x + y * x_size].back());
                            field[x + y * x_size].pop_back();
                        } else if (field[x + y * x_size].size() > 1) {
                            eldest_hyena->eat(field[x + y * x_size][field[x + y * x_size].size() - 2]);
                            field[x + y * x_size].erase(field[x + y * x_size].end() - 2);
                        }
                    }
                }

                Animal *eldest_wolf = find_eldest_animal_by_type(field[x + y * x_size], 0); //type 0 for wolf
                if (eldest_wolf != nullptr) {
                    auto iter = field[x + y * x_size].begin();
                    while (iter != field[x + y * x_size].end()) {
                        if ((*iter)->getType() == 1) {
                            eldest_wolf->eat((*iter));
                            iter = field[x + y * x_size].erase(iter);
                        } else {
                            ++iter;
                        }
                    }
                }
            }
        }
    }

    void duplicate_animals(size_t tick) {
        for (size_t x = 0; x < x_size; x++) {
            for (size_t y = 0; y < y_size; y++) {
                auto iter = field[x + y * x_size].begin();
                while (iter != field[x + y * x_size].end()) {
                    if ((*iter)->getType() == 0 && (*iter)->getSaturation() >= 2 &&
                        (*iter)->last_duplication < tick) { //WOLF
                        (*iter)->resetSaturation();
                        (*iter)->last_duplication = tick;
                        auto *new_wolf = new Wolf(x, y, (*iter)->direction, (*iter)->constancy, tick, 0, (*iter));
                        field[x + y * x_size].emplace_back(new_wolf);
                        iter = field[x + y * x_size].begin();
                    } else if ((*iter)->getType() == 1 && (*iter)->last_duplication < tick &&
                               ((*iter)->getAge() == 5 || (*iter)->getAge() == 10)) {
                        (*iter)->last_duplication = tick;
                        auto *new_rabbit = new Rabbit(x, y, (*iter)->direction, (*iter)->constancy, tick, 0, (*iter));
                        field[x + y * x_size].emplace_back(new_rabbit);
                        iter = field[x + y * x_size].begin();
                    } else if ((*iter)->getType() == 2 && (*iter)->getSaturation() >= 2 &&
                               (*iter)->last_duplication < tick) {
                        (*iter)->resetSaturation();
                        (*iter)->last_duplication = tick;
                        auto *new_hyena = new Hyena(x, y, (*iter)->direction, (*iter)->constancy, tick, 0, (*iter));
                        field[x + y * x_size].emplace_back(new_hyena);
                        iter = field[x + y * x_size].begin();
                    } else {
                        ++iter;
                    }

                }
            }
        }
    }

    void kill_animals(size_t tick) {
        for (size_t x = 0; x < x_size; x++) {
            for (size_t y = 0; y < y_size; y++) {
                auto iter = field[x + y * x_size].begin();
                while (iter != field[x + y * x_size].end()) {
                    if (((*iter)->getType() == 0 || (*iter)->getType() == 2) && (*iter)->getAge() == 15) {
                        iter = field[x + y * x_size].erase(iter);
                    } else if ((*iter)->getType() == 1 && (*iter)->getAge() == 10) {
                        iter = field[x + y * x_size].erase(iter);
                    } else ++iter;
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


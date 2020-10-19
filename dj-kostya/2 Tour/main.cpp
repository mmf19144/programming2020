#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>

enum class MovRotation {
    UP, RIGHT, DOWN, LEFT
};

enum class AnimalType {
    Rabbit, Wolf
};

MovRotation operator++(MovRotation &color) {
    switch (color) {
        case MovRotation::UP:
            return MovRotation::RIGHT;
        case MovRotation::RIGHT:
            return MovRotation::DOWN;
        case MovRotation::DOWN:
            return MovRotation::LEFT;
        case MovRotation::LEFT:
            return MovRotation::UP;
    }
}

typedef struct sPoint {
    size_t x;
    size_t y;
} Point;

class IAnimal {
public:
    virtual Point move() = 0;

    virtual Point get_pos() = 0;

    virtual void aging() = 0;

    virtual void dying() = 0;

    virtual bool must_die() = 0;

    virtual bool is_dead() = 0;

    virtual bool must_multiply() = 0;

    virtual AnimalType get_animal_type() = 0;
};

class Simulation {
    const size_t height;
    const size_t width;
    size_t cur_step = 0;
    const size_t max_steps = 0;
    std::vector<IAnimal *> animals;
    std::vector<std::vector<std::vector<IAnimal *>>> field;
public:

    Simulation(const size_t height_, const size_t width_, const size_t max_step) : height(height_), width(width_),
                                                                                   max_steps(max_step) {
        field.resize(height_);
        for (int i = 0; i < height_; i++)
            field[i].resize(width_);
    }

    size_t get_new_id() const {
        return animals.size();
    }

    size_t get_current_step() const {
        return cur_step;
    }

    size_t get_height() const {
        return height;
    }

    size_t get_width() const {
        return width;
    }

    const IAnimal *get_parent(const size_t id) const {
        return animals[id];
    }

    void spawn_animal(IAnimal *animal) {
        animals.push_back(animal);
        Point pos = animal->get_pos();
        field[pos.y][pos.x].push_back(animal);
    }

    void step();

    friend std::ostream &operator<<(std::ostream &os, const Simulation &m) {
        for (size_t y = 0; y < m.height; y++) {
            for (size_t x = 0; x < m.width; x++) {

                int size = 0;
                for (auto j : m.field[y][x]) {
                    if (j->get_animal_type() == AnimalType::Wolf)
                        size--;
                    else if (j->get_animal_type() == AnimalType::Rabbit)
                        size++;
                }
                if (size == 0) os << '#';
                else os << size;
            }
            if (y < m.height - 1)
                os << std::endl;
        }
        return os;
    }

    friend std::istream &operator>>(std::istream &os, Simulation &m);

    void run() {
        for (int s = 0; s < max_steps; s++) {
            step();
        }

    }
};


class BaseAnimal : public IAnimal {

    size_t x;
    size_t y;
    const size_t speed;

    const size_t max_age;
    const size_t constancy;
    MovRotation rot;
    const AnimalType type;
protected:
    const bool is_root_parent;
    const size_t id;
    const size_t parent_id;
    Simulation &sim;
    const size_t birth_step;
    bool is_die;
    size_t age = 0;

public:
    BaseAnimal(BaseAnimal *parent, Simulation &sim_) : id(sim_.get_new_id()),
                                                       parent_id(parent->id),
                                                       x(parent->x),
                                                       y(parent->y),
                                                       sim(sim_),
                                                       birth_step(sim_.get_current_step()),
                                                       constancy(parent->constancy),
                                                       rot(parent->rot),
                                                       is_die(false),
                                                       speed(parent->speed),
                                                       max_age(parent->max_age),
                                                       is_root_parent(false),
                                                       type(parent->type) {}

    BaseAnimal(size_t x_,
               size_t y_,
               MovRotation start_rot,
               const size_t constancy_,
               const size_t animal_speed,
               const size_t animal_max_age,
               const AnimalType type_,
               Simulation &sim_) : id(sim_.get_new_id()),
                                   parent_id(-1),
                                   x(x_),
                                   y(y_),
                                   sim(sim_),
                                   birth_step(0),
                                   constancy(constancy_),
                                   rot(start_rot),
                                   is_die(false),
                                   speed(animal_speed),
                                   max_age(animal_max_age),
                                   is_root_parent(true),
                                   type(type_) {}

    Point get_pos() override {
        return {x, y};
    }

    Point move() override {
        switch (rot) {
            case MovRotation::UP:
                y -= speed;
                break;
            case MovRotation::DOWN:
                y += speed;
                break;
            case MovRotation::RIGHT:
                x += speed;
                break;
            case MovRotation::LEFT:
                x -= speed;
                break;
        }
        x += sim.get_width();
        x %= sim.get_width();
        y += sim.get_height();
        y %= sim.get_height();
        if ((sim.get_current_step() - birth_step) > 0 && (sim.get_current_step() - birth_step) % constancy == 0) {
            ++rot;
        }
        return get_pos();
    }

    void aging() override {
        age++;
    }

    bool must_die() override {
        is_die = age >= max_age;
        return is_dead();
    };

    bool is_dead() override {
        return is_die;
    };

    void dying() override {
        is_die = true;
    }


    AnimalType get_animal_type() override {
        return type;
    }

};

class Rabbit : public BaseAnimal {
private:
    const size_t reproduction_step = 5;
public:
    Rabbit(BaseAnimal *parent, Simulation &sim_) : BaseAnimal(parent, sim_) {}

    Rabbit(size_t x_, size_t y_, MovRotation start_rot, const size_t constancy_, Simulation &sim_) :
            BaseAnimal(x_, y_, start_rot, constancy_, 1, 10, AnimalType::Rabbit, sim_) {}

    bool must_multiply() override {
        return !is_die && age % reproduction_step == 0 &&
               age != 0;
    }


};

class Wolf : public BaseAnimal {
    size_t kills = 0;
    const size_t reproduction_kills = 2;
public:
    Wolf(BaseAnimal *parent, Simulation &sim_) : BaseAnimal(parent, sim_) {}

    Wolf(size_t x_, size_t y_, MovRotation start_rot, const size_t constancy_, Simulation &sim_) :
            BaseAnimal(x_, y_, start_rot, constancy_, 2, 10, AnimalType::Wolf, sim_) {}

    bool must_multiply() override {
        return !is_die && kills >= reproduction_kills;
    }

    void reset_kills() {
        if (!is_die && kills == reproduction_kills)
            kills = 0;
    }

    void kill() {
        kills++;
    }

    bool operator<(Wolf &other) {
        if (this->age > other.age)
            return false;
        else if (this->age < other.age)
            return true;
        else {
            if (is_root_parent && other.is_root_parent)
                return id > other.id;
            else if (is_root_parent)
                return false;
            else if (other.is_root_parent)
                return true;
            return *(Wolf *) (sim.get_parent(this->parent_id)) < *(Wolf *) (sim.get_parent(other.parent_id));
        }
    }
};


void Simulation::step() {
    cur_step++;
    for (auto *animal: animals) {
        if (animal->is_dead())
            continue;
        Point old_position = animal->get_pos();

        for (int i = 0; i < field[old_position.y][old_position.x].size(); i++) {
            if (field[old_position.y][old_position.x][i] == animal) {
                field[old_position.y][old_position.x].erase(field[old_position.y][old_position.x].begin() + i);
                break;
            }
        }
        Point new_pos = animal->move();
        field[new_pos.y][new_pos.x].push_back(animal);
    }

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            size_t field_size = field[y][x].size();
            if (!field_size) continue;
            Wolf *oldest_wolf = nullptr;
            for (int j = 0; j < field_size; j++) {
                if (field[y][x][j]->get_animal_type() == AnimalType::Wolf) {
                    if (oldest_wolf == nullptr || *oldest_wolf < *(Wolf *) field[y][x][j])
                        oldest_wolf = (Wolf *) field[y][x][j];
                }
            }
            if (oldest_wolf == nullptr)
                continue;

            for (int j = 0; j < field[y][x].size(); j++) {
                if (field[y][x][j]->get_animal_type() == AnimalType::Rabbit) {
                    field[y][x][j]->dying();
                    field[y][x].erase(field[y][x].begin() + j);
                    j--;
                    oldest_wolf->kill();
                }
            }
        }
    }
    for (auto *animal: animals) {
        if (animal->is_dead()) continue;
        animal->aging();
        if (animal->must_multiply()) {
            switch (animal->get_animal_type()) {
                case AnimalType::Rabbit: {
                    spawn_animal(new Rabbit((BaseAnimal *) animal, *this));
                    break;
                }
                case AnimalType::Wolf: {
                    spawn_animal(new Wolf((BaseAnimal *) animal, *this));
                    ((Wolf *) animal)->reset_kills();
                    break;
                }
            }
        }
        if (animal->must_die()) {
            Point old_position = animal->get_pos();
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


std::istream &operator>>(std::istream &os, Simulation &m) {
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
            MovRotation rot = MovRotation::UP;
            os >> x >> y >> d >> k;
            switch (d) {
                case 0:
                    rot = MovRotation::UP;
                    break;
                case 1:
                    rot = MovRotation::RIGHT;
                    break;
                case 2:
                    rot = MovRotation::DOWN;
                    break;
                case 3:
                    rot = MovRotation::LEFT;
                    break;
                default:
                    break;
            }
            if (t == 0)
                m.spawn_animal(new Rabbit(x, y, rot, k, m));
            else if (t == 1)
                m.spawn_animal(new Wolf(x, y, rot, k, m));
        }
    }
    return os;
}


int main() {
    std::ofstream fout("output.txt");
    std::ifstream fin("input.txt");

    int H, W, steps;
    fin >> H >> W >> steps;
    Simulation test(H, W, steps);
    fin >> test;
    test.run();
    fout << test;

    return 0;
}
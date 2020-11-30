#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <map>

//#define DEBUG

//#define HYENA_AVAILABLE
#ifdef HYENA_AVAILABLE
#define ANIMAL_CNT 3
#else
#define ANIMAL_CNT 2
#endif

#define Point std::pair<size_t , size_t>
enum class MovRotation {
    UP, RIGHT, DOWN, LEFT
};

enum class AnimalType {
    Rabbit, Wolf, Hyena
};


class IAnimal {
public:
    int selectedInStep = -1;

    virtual Point move() = 0;

    virtual Point get_pos() = 0;

    virtual void aging() = 0;

    virtual void dying() = 0;

    virtual bool must_die() = 0;

    virtual bool is_dead() = 0;

    virtual IAnimal *multiply() = 0;

    virtual AnimalType get_animal_type() = 0;

    virtual bool kill(IAnimal *an) = 0;

    virtual bool can_kill() const {
        return false;
    }
};

class Simulation {
    const size_t height;
    const size_t width;
    size_t cur_step = 0;
    const size_t max_steps = 0;
    std::vector<IAnimal *> animals;
    std::map<std::pair<size_t, size_t>, std::vector<IAnimal *>> field;
public:

    Simulation(const size_t height_, const size_t width_, const size_t max_step) : height(height_), width(width_),
                                                                                   max_steps(max_step) {
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
        field[pos].push_back(animal);
    }

    void step();

    friend std::ostream &operator<<(std::ostream &os, const Simulation &m) {
        for (size_t y = 0; y < m.height; y++) {
            for (size_t x = 0; x < m.width; x++) {

                int size = 0;


                auto curPos = m.field.find(Point(y, x));
                if (curPos == m.field.end()) {
                    os << "#";
                } else {
                    for (auto j : curPos->second) {
                        switch (j->get_animal_type()) {

                            case AnimalType::Rabbit:
                                size++;
                                break;
                            case AnimalType::Wolf:
                            case AnimalType::Hyena:
                                size--;
                                break;
                        }

                    }
                    if (size == 0)
                        os << "#";
                    else
                        os << size;
                }


            }
            if (y < m.height - 1)
                os << std::endl;
        }
        return os;
    }

    friend std::istream &operator>>(std::istream &os, Simulation &m);

    void run() {
        for (int s = 0; s < max_steps; s++) {
#ifdef DEBUG
            std::cout << *this << std::endl << std::endl;
#endif
            step();

        }

    }
};


class BaseAnimal : public IAnimal {

    int x;
    int y;
    const size_t speed;

    const size_t max_age;
    const size_t constancy;
    MovRotation rot;
    const AnimalType type;


protected:
    Simulation &sim;
    const bool is_root_parent;
    const size_t id;
    const size_t parent_id;

    const size_t birth_step;
    bool is_die;
    size_t age = 0;

public:

    explicit BaseAnimal(BaseAnimal *parent) : id(sim.get_new_id()),
                                              parent_id(parent->id),
                                              x(parent->x),
                                              y(parent->y),
                                              sim(parent->sim),
                                              birth_step(sim.get_current_step()),
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
        return {y, x};
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
        if (x < 0)
            x += sim.get_width();
        else if (x >= sim.get_width())
            x -= sim.get_width();
        if (y < 0)
            y += sim.get_height();
        else if (y >= sim.get_height())
            y -= sim.get_height();

        if ((sim.get_current_step() - birth_step) > 0 && (sim.get_current_step() - birth_step) % constancy == 0) {
            switch (rot) {
                case MovRotation::UP: {
                    rot = MovRotation::RIGHT;
                    break;
                }
                case MovRotation::RIGHT: {
                    rot = MovRotation::DOWN;
                    break;
                }
                case MovRotation::DOWN: {
                    rot = MovRotation::LEFT;
                    break;
                }
                case MovRotation::LEFT: {
                    rot = MovRotation::UP;
                    break;
                }
            }
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

    bool operator<(BaseAnimal &other) {
        if (this->can_kill() != other.can_kill()) {
            return this->can_kill() < other.can_kill();
        }
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
            return *(BaseAnimal *) (sim.get_parent(this->parent_id)) <
                   *(BaseAnimal *) (sim.get_parent(other.parent_id));
        }
    }

};

class Rabbit : public BaseAnimal {
private:
    const size_t reproduction_step = 5;
public:
    Rabbit(BaseAnimal *parent) : BaseAnimal(parent) {}

    Rabbit(size_t x_, size_t y_, MovRotation start_rot, const size_t constancy_, Simulation &sim_) :
            BaseAnimal(x_, y_, start_rot, constancy_, 1, 10, AnimalType::Rabbit, sim_) {}


    bool kill(IAnimal *an) override {
        return false;
    }

    IAnimal *multiply() override {
        if (!is_die && age % reproduction_step == 0 && age != 0)
            return new Rabbit(this);
        else
            return nullptr;
    }


};

class Wolf : public BaseAnimal {
protected:
    size_t kills = 0;
    const size_t reproduction_kills = 2;
protected:
    Wolf(size_t x_, size_t y_, MovRotation start_rot, const size_t constancy_, Simulation &sim_, AnimalType anim_type) :
            BaseAnimal(x_, y_, start_rot, constancy_, 2, 10, anim_type, sim_) {}

public:
    Wolf(BaseAnimal *parent) : BaseAnimal(parent) {}

    Wolf(size_t x_, size_t y_, MovRotation start_rot, const size_t constancy_, Simulation &sim_) :
            BaseAnimal(x_, y_, start_rot, constancy_, 2, 10, AnimalType::Wolf, sim_) {}

    IAnimal *multiply() override {
        if (!is_die && kills >= reproduction_kills) {
            kills = 0;
            return new Wolf(this);
        } else
            return nullptr;
    }

    bool kill(IAnimal *an) override {
        switch (an->get_animal_type()) {
            case AnimalType::Rabbit: {
                an->dying();
                kills++;
                return true;
            }

            case AnimalType::Wolf:
            case AnimalType::Hyena:
                return false;
        }
    }

    bool can_kill() const override {
        return true;
    }
};

class Hyena : public Wolf {
private:
    bool can_kill_ = true;

public:
    Hyena(size_t x_, size_t y_, MovRotation start_rot, const size_t constancy_, Simulation &sim_) :
            Wolf(x_, y_, start_rot, constancy_, sim_, AnimalType::Hyena) {}

    Hyena(BaseAnimal *parent) : Wolf(parent) {}

    IAnimal *multiply() override {
        if (!is_die && kills >= reproduction_kills) {
            kills = 0;
            can_kill_ = false;
            return new Wolf(this);
        } else
            return nullptr;
    }

//    void reset_kill() {
//        if (!is_die && kills == reproduction_kills) {
//            kills = 0;
//            can_kill_ = false;
//        }
//    }
    bool kill(IAnimal *an) override {
        an->dying();
        kills++;
        return true;
    }

    bool can_kill() const override {
        return can_kill_ && kills < reproduction_kills;
    }

};

void Simulation::step() {

    cur_step++;
    // moving
    for (auto *animal: animals) {
        if (animal->is_dead())
            continue;
        Point old_position = animal->get_pos();

        for (int i = 0; i < field[old_position].size(); i++) {
            if (field[old_position][i] == animal) {
                field[old_position].erase(field[old_position].begin() + i);
                break;
            }
        }
        Point new_pos = animal->move();
        field[new_pos].push_back(animal);
    }
    // eating

    for (const auto &curPos: field) {
        sort(field[curPos.first].begin(), field[curPos.first].end(), [](const IAnimal *lhs, const IAnimal *rhs) {
            return *(BaseAnimal *) rhs < *(BaseAnimal *) lhs;
        });
        bool has_kill = true;
        while (has_kill) {
            has_kill = false;
            IAnimal *oldestAnimal = nullptr;
            for (auto animal: curPos.second) {
                if (animal->selectedInStep == cur_step) continue;
                if (animal->can_kill()) {
                    oldestAnimal = animal;
                    break;
                }
            }
            if (!oldestAnimal)
                continue;

            oldestAnimal->selectedInStep = cur_step;
            for (auto i : curPos.second) {
                if (oldestAnimal == i)
                    continue;
                if (!oldestAnimal->can_kill())
                    break;

                if (oldestAnimal->kill(i)) {
                    field[curPos.first].erase(std::find(field[curPos.first].begin(), field[curPos.first].end(), i));
                    has_kill = true;
                }
            }
        }

    }


    // aging and multiplying
    for (auto *animal: animals) {
        if (animal->is_dead()) continue;
        animal->aging();
        IAnimal *children = animal->multiply();
        if (children)
            spawn_animal(children);

        // dying
        if (animal->must_die()) {
            Point old_position = animal->get_pos();
            for (int i = 0; i < field[old_position].size(); i++) {
                if (field[old_position][i] == animal) {
                    field[old_position].erase(field[old_position].begin() + i);
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
#ifdef HYENA_AVAILABLE
    int h;
    os >> h;
#endif
    for (int t = 0; t < ANIMAL_CNT; t++) {
        int max_s = 0;
        if (t == 0)
            max_s = r;
        else if (t == 1)
            max_s = w;
#ifdef HYENA_AVAILABLE
        else if (t == 2)
            max_s = h;
#endif

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
#ifdef HYENA_AVAILABLE
            else if (t == 2)
                m.spawn_animal(new Hyena(x, y, rot, k, m));
#endif
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
    fout << test << std::endl;


    return 0;
}
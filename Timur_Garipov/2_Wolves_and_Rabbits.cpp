#include <iostream>
#include <fstream>
#include <stack>
#include <vector>

enum direct { up, right, down, left };

class coordinate {
public:
    int x;
    int y;

    coordinate() {
        this->x = -2;
        this->y = -2;
    };
    coordinate(int x_, int y_) {
        this->x = x_;
        this->y = y_;
    }
    bool operator==(coordinate k) {
        if (k.x == this->x && k.y == this->y)
            return true;
        else
            return false;
    }
    void checkBorder(int N, int M) {

        if (this->x >= N) {
            this->x = this->x - N;
        }

        if (this->y >= M) {
            this->y = this->y - M;
        }

        if (this->x < 0) {
            this->x = this->x + N;
        }

        if (this->y < 0) {
            this->y = this->y + M;
        }
    }
};

class Field {
private:
    int dimension_N;
    int dimension_M;
public:
    int getXdim() {
        return dimension_N;
    }

    int getYdim() {
        return dimension_M;
    }

    void setXdim(int x) {
        dimension_N = x;
    }

    void setYdim(int y) {
        dimension_M = y;
    }
};

class Cell {
public:
    int value;
    std::vector<int> arr_order;
    Cell() {
        value = 0;
    };
    void push(int value) {
        arr_order.push_back(value);
    }
};

class Pole {
public:
    int N, M;
    Cell** massive;
    Pole();
    Pole(int N_, int M_) {
        this->N = N_;
        this->M = M_;

        massive = new Cell * [M];
        for (int i = 0; i < M; i++) {
            massive[i] = new Cell[N];
        }
    }
    ~Pole() {
        for (int i = 0; i < M; i++) {
            delete[] massive[i];
        }
        delete[] massive;
    }
    void clear() {
        for (int i = 0; i < M; i++)
            for (int j = 0; j < N; j++) {
                massive[i][j].arr_order.clear();
                massive[i][j].value = 0;
            }
    }
};

class Animal {
protected:
    bool statDie;
    coordinate coordinates;
    int dim_of_step;
    direct direction;
    int constancy;
    int age;
    int count_step;
    void makeMove(int N, int M) {
        makeStep(N, M);
        checkConstancy();
    }
    void growUp() {
        this->age++;
    }
    void checkConstancy() {

        if (this->count_step == this->constancy) {
            this->count_step = 0;

            switch (direction) {
                case up:
                    direction = right;
                    break;
                case right:
                    direction = down;
                    break;
                case down:
                    direction = left;
                    break;
                case left:
                    direction = up;
                    break;
            }
        }
    }
    void makeStep(int N, int M) {
        int step = dim_of_step;

        switch (this->direction) {
            case up:
                this->coordinates.y -= step;
                break;
            case right:
                this->coordinates.x += step;
                break;
            case down:
                this->coordinates.y += step;
                break;
            case left:
                this->coordinates.x -= step;
                break;
        }

        this->coordinates.checkBorder(N, M);
        this->count_step++;
    }
public:
    Animal() {};
    virtual void mustDie() {};
    coordinate getCoord() {
        return this->coordinates;
    }
    int getAge() {
        return this->age;
    }
    void setStatDie(int z) {
        statDie = z;
    }
    int getStatDie() {
        return statDie;
    }
    virtual ~Animal() {};
};

class Rabbit : public Animal {
public:
    Rabbit(coordinate coor, int dir, int k) {
        this->statDie = 0;
        this->age = 0;
        this->constancy = k;
        this->coordinates = coor;
        this->count_step = 0;
        this->direction = (direct)dir;

        this->dim_of_step = 1;
    }
    void reproduction(std::vector<Rabbit>& str) {
        if (this->age == 5 || this->age == 10) {
            str.push_back(Rabbit(this->coordinates, this->direction, this->constancy));
        }
    }
    void mustDie() override {
        if (this->age == 10) {
            statDie = 1;
        }
    }
    void iterLoop(int N, int M, std::vector<Rabbit>& str) {
        growUp();
        makeMove(N, M);
        mustDie();
        reproduction(str);
    }
};

class Wolf : public Animal {
private:
    int killstreak;
public:
    Wolf(coordinate coor, int dir, int k) {
        this->statDie = 0;
        this->age = 0;
        this->constancy = k;
        this->coordinates = coor;
        this->count_step = 0;
        this->direction = (direct)dir;
        this->killstreak = 0;

        this->dim_of_step = 2;
    }
    void eating(std::vector<Rabbit> &rabbits) {
        int countFoods = 0;
        int count = rabbits.size();

        for (int i = 0; i < count; i++)
            if (rabbits[i].getCoord() == coordinates) {
                rabbits[i].setStatDie(1);
                countFoods++;
            }

        this->killstreak += countFoods;
    }
    void reproduction(std::vector<Wolf>& str) {
        if (this->killstreak >= 2) {
            this->killstreak = 0;
            str.push_back(Wolf(this->coordinates, this->direction, this->constancy));
        }

    }
    void mustDie() override {
        if (this->age == 15) {
            statDie = 1;
        }
    }
    void iterLoop(int N, int M, std::vector<Rabbit>& rabbits, std::vector<Wolf>& wolves) {
        growUp();
        makeMove(N, M);
    }
};

class Simulation {
private:
    std::string input_file;
    std::string output_file;
    int count_rabbits,
            count_wolves;
    int timeSim;
    Field field;

    std::vector<Wolf> vect_wolf;
    std::vector<Rabbit> vect_rabbit;

    Pole* ground_rabbits,
            * ground_wolves;
public:
    int getCountRab() {
        return count_rabbits;
    }
    int getCountWolf() {
        return count_wolves;
    }
    void setCountRab(int k) {
        count_rabbits = k;
    }
    void getCountWolf(int k) {
        count_wolves = k;
    }
    ~Simulation() {
        delete ground_rabbits;
        delete ground_wolves;

        vect_wolf.clear();
        vect_rabbit.clear();
    }
    Simulation(std::string input, std::string output) {
        this->input_file = input;
        this->output_file = output;

        std::ifstream fin(input);

        int N, M, T;
        fin >> N >> M >> T;
        field.setXdim(N);
        field.setYdim(M);

        ground_wolves = new Pole(N, M);
        ground_rabbits = new Pole(N, M);

        this->timeSim = T;

        int R, W;
        fin >> R >> W;
        this->count_rabbits = R;
        this->count_wolves = W;

        int x, y, d, k;
        for (int i = 0; i < R; i++) {
            fin >> x >> y >> d >> k;
            vect_rabbit.push_back(Rabbit(coordinate(x, y), d, k));
        }
        for (int i = 0; i < W; i++) {
            fin >> x >> y >> d >> k;
            vect_wolf.push_back(Wolf(coordinate(x, y), d, k));
        }
    }
    void updatePole(int countWolves, int countRab) {
        ground_rabbits->clear();
        ground_wolves->clear();
        for (int j = 0; j < countWolves; j++) {
            coordinate f = vect_wolf[j].getCoord();
            ground_wolves->massive[f.y][f.x].value += 1;
            ground_wolves->massive[f.y][f.x].push(j);
        }
        for (int j = 0; j < countRab; j++) {
            coordinate f = vect_rabbit[j].getCoord();
            ground_rabbits->massive[f.y][f.x].value += 1;
            ground_rabbits->massive[f.y][f.x].push(j);
        }
    }
    void startSimulation() {
        coordinate z(field.getXdim(), field.getYdim());
        int countRab = vect_rabbit.size(),
                countWolves = vect_wolf.size();

        for (int i = 0; i < this->timeSim; i++) {
            countRab = vect_rabbit.size();
            countWolves = vect_wolf.size();


            updatePole(countWolves, countRab);
            for (int j = 0; j < countRab; j++)
                vect_rabbit[j].iterLoop(z.x, z.y, vect_rabbit);
            for (int j = 0; j < countWolves; j++)
                vect_wolf[j].iterLoop(z.x, z.y, vect_rabbit, vect_wolf);


            updatePole(countWolves, countRab);
            int flag_for_reproduction_wolves = 0;
            for (int i = 0; i < z.y; i++)
                for (int j = 0; j < z.x; j++)
                    if (ground_rabbits->massive[i][j].value * ground_wolves->massive[i][j].value != 0) {
                        flag_for_reproduction_wolves = true;


                        int size_for_sort = ground_wolves->massive[i][j].arr_order.size();
                        for(int i1 = 0; i1 < size_for_sort; i1++)
                            for (int j1 = i1 + 1; j1 < size_for_sort; j1++)
                                if (vect_wolf[ground_wolves->massive[i][j].arr_order[i1]].getAge() <
                                    vect_wolf[ground_wolves->massive[i][j].arr_order[j1]].getAge()) {
                                    int support = ground_wolves->massive[i][j].arr_order[i1];
                                    ground_wolves->massive[i][j].arr_order[i1] = ground_wolves->massive[i][j].arr_order[j1];
                                    ground_wolves->massive[i][j].arr_order[j1] = support;
                                }


                        int prev_size = vect_wolf.size();
                        vect_wolf[ground_wolves->massive[i][j].arr_order[0]].eating(vect_rabbit);

                        ground_wolves->massive[i][j].value = ground_wolves->massive[i][j].value +
                                                             vect_wolf.size() - prev_size;
                        ground_rabbits->massive[i][j].value = 0;
                    }

            int size = vect_wolf.size();
            for (int i = 0; i < size; i++) {

                if (flag_for_reproduction_wolves) {
                    vect_wolf[i].reproduction(vect_wolf);
                }

                vect_wolf[i].mustDie();
            }


            countRab = vect_rabbit.size();
            countWolves = vect_wolf.size();
            for (int j = 0; j < countRab; j++)
                if (vect_rabbit[j].getStatDie() == 1) {
                    vect_rabbit.erase(vect_rabbit.begin() + j);
                    j--;
                    countRab--;
                }
            for (int j = 0; j < countWolves; j++)
                if (vect_wolf[j].getStatDie() == 1) {
                    vect_wolf.erase(vect_wolf.begin() + j);
                    countWolves--;
                    j--;
                }
        }
    }
    void printResult() {
        std::ofstream fout("output.txt");
        int N = field.getXdim(),
                M = field.getYdim();
        updatePole(vect_wolf.size(), vect_rabbit.size());
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                if (ground_rabbits->massive[i][j].value == 0
                    && ground_wolves->massive[i][j].value == 0) {
                    fout << "#";
                    continue;
                }

                if (ground_rabbits->massive[i][j].value > 0) {
                    fout << ground_rabbits->massive[i][j].value;
                    continue;
                }

                if (ground_wolves->massive[i][j].value > 0) {
                    fout << -ground_wolves->massive[i][j].value;
                    continue;
                }
            }
            fout << std::endl;
        }
    }
};

int main() {
    Simulation sim("input.txt ", "output.txt");
    sim.startSimulation();
    sim.printResult();
    return 0;
}
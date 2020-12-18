
#include <iostream>
#include <fstream>
#include <cstdio>
#include <stdio.h>
#include <vector>
//#include <algorithm>
using namespace std;
struct Coord{
    int x;
    int y;
};

class Animal{
protected:
    Coord cord;
    int dir;
    int stab;
    int age;
    int feed;
    int birthtime;
    int type;
public:
    Animal(){
        this->cord.x = 0;
        this->cord.y = 0;
        this->dir = 0;
        this->stab = 0;
        this->age = 0;
        this->birthtime = 0;
        this->type = 0;
        this->feed =0 ;
    }

    ~Animal(){

    }
    Animal( int c1,int c2 , int di, int st,int btime) {
        this->cord.x = c1;
        this->cord.y = c2;
        this->dir = di;
        this->stab = st;
        this->birthtime = btime;
        this->age = 0;
        this->feed = 0;

    }
    virtual void Move (int N,int M){}
    virtual void eat(){}
    virtual void duplicate(){}

    Coord getcord(){
        return this->cord;
    }
    int getdir(){
        return this->dir;
    }
    int getstab(){
        return this->stab;
    }
    int getage() {
        return this->age;
    }
    int getfeed(){
        return this->feed;
    }
    int getbirthtime(){
        return this->birthtime;
    }
    int gettype(){
        return this->type;
    }
};



class Wolf : public Animal{

public:
    Wolf():  Animal(){

        this->type = 1;
    }

    Wolf(int c1,int c2 , int di, int st,int btime): Animal(c1,c2,di,st,btime){
        this->type = 1;
        this->feed =0;
    }

    void Move(int b1,int b2) override{
        if (this ->dir ==0){
            if (this->cord.x>=2){this->cord.x = this->cord.x - 2;}
            else { this->cord.x = (this->cord.x-2)+b1;}
        }
        if (this ->dir ==1){
            if (this->cord.y<=(b1-1)-2){this->cord.y = this->cord.y + 2;}
            else { this->cord.y = (this->cord.y+2)-b2;}
        }
        if (this ->dir ==2){
            if (this->cord.x<=(b1-1)-2){this->cord.x = this->cord.x + 2;}
            else { this->cord.x = (this->cord.x+2)-b1;}
        }
        if (this ->dir ==3){
            if (this->cord.y>=2){this->cord.y = this->cord.y - 2;}
            else { this->cord.y = (this->cord.x+2)+b2;}
        }

        this->age+=1;

        if (this->age % this->stab == 0) this->dir +=1;
        if (this->dir == 4) this->dir = 0;

    }

    void eat() override{

        this->feed += 1;

    }
    void duplicate() override {
        this->feed=0;
    }
};



class Rabbit: public Animal{
public:
    Rabbit(): Animal(){
        this->type=-1;
    }

    Rabbit(int c1,int c2 , int di, int st,int btime): Animal(c1,c2,di,st,btime){
        this->type = -1;
    }
    void Move(int b1,int b2) override{
        if (this ->dir ==0){
            if (this->cord.x>=1){this->cord.x = this->cord.x - 1;}
            else { this->cord.x = (this->cord.x-1)+b2;}
        }
        if (this ->dir ==1){
            if (this->cord.y<=(b2-1)-1){this->cord.y = this->cord.y + 1;}
            else { this->cord.y = (this->cord.y+1)-b2;}
        }
        if (this ->dir ==2){
            if (this->cord.x<=(b1-1)-1){this->cord.x = this->cord.x + 1;}
            else { this->cord.x = (this->cord.x+1)-b1;}
        }
        if (this ->dir ==3){
            if (this->cord.y>=1){this->cord.y = this->cord.y - 1;}
            else { this->cord.y = (this->cord.y-1)+b2;}
        }

        this->age+=1;

        if (this->age % this->stab == 0) this->dir +=1;
        if (this->dir == 4) this->dir = 0;
    }

};

class Simulation {
public:
    const int xsize;
    const int ysize;
    vector<Animal *> *table;
public:
    Simulation( int WN, int RN, int x_size, int y_size, Coord *wC, int *wD, int *wS, //�������� �����
                Coord *rC, int *rD, int *rS) : xsize(x_size), ysize(y_size),
                                               table(new vector<Animal *>[x_size * y_size]) {

        for (size_t x = 0; x < x_size; x++) {
            for (size_t y = 0; y < y_size; y++) {
                table[y + x * y_size].clear();
            }
        }
        for (int i = 0; i < WN; i++) {
            Wolf *wolf = new Wolf(wC[i].x, wC[i].y, wD[i], wS[i], 0);
            table[wC[i].x + xsize * wC[i].y].emplace_back(wolf);

        }
        for (int i = 0; i < RN; i++) {
            Rabbit *rabb = new Rabbit(rC[i].x, rC[i].y, rD[i], rS[i], 0);
            table[rC[i].x + rC[i].y * xsize].emplace_back(rabb);
        }

    }

    ~Simulation(){
        for (int i = 0; i < ysize; i++) {
            for (int j = 0; j < xsize; j++) {
                for (int k = 0; k < table[j * ysize + i].size(); k++) {
                    if (table[j * ysize + i][k] == nullptr) continue;
                    delete table[j * ysize + i][k];
                }
            }
        }
        delete table;
    }

    void print_table(std::ostream &out){   //�������� �����
        int nw = 0;
        int nr = 0;

        for (int i = 0; i < ysize; i++) {
            for (int j = 0; j < xsize; j++) {
                for (int k = 0; k < table[j * ysize + i].size(); k++) {
                    if (table[j * ysize + i][k] == nullptr) continue;
                    if (table[j * ysize + i][k]->gettype()==-1) {nr +=1; }
                    if (table[j * ysize + i][k]->gettype()== 1) {nw -=1; }
                }
                if (nw != 0){out << nw;}
                if (nr != 0){out<<nr;}
                if(nw==0 && nr==0) out << "#";
                nr=0;
                nw=0;
            }
            out << endl;
        }
    }

    void run_simulation(int all_time,std::ostream &out){
        for(int i = 0;i<all_time;i++){
            //cout << "Step " << i;
            Moving(i);
            //cout << endl << "Moving" << endl;
            //print_table(out);
            eat_animals();
            //cout << endl << "Eat" << endl;
            //print_table(out);
            make_animals(i+1);
            //cout << endl << "Make" << endl;
            //print_table(out);
            kill_animals();
            //cout << endl << "Kill" << endl;
            //print_table(out);
        }

        print_table(out);
    }


    void Moving(int time) {
        for (int i = 0; i < xsize; i++) {
            for (int j = 0; j < ysize; j++) {
                for (int k = 0; k < table[j * xsize + i].size(); k++) {

                    if (table[j * xsize + i][k] != nullptr) {

                        if (table[j * xsize + i][k]->getage() == time - table[j * xsize + i][k]->getbirthtime()) {

                            Coord oldcords = table[j * xsize + i][k]->getcord();
                            table[j * xsize + i][k]->Move(xsize, ysize);
                            Coord newcords = table[j * xsize + i][k]->getcord();

                            if (newcords.x == oldcords.x && newcords.y == oldcords.y) continue;
                            else {
                                table[newcords.y * xsize + newcords.x].emplace_back(table[j * xsize + i][k]);
                                table[j * xsize + i][k] = nullptr;
                            }
                        } else continue;
                    }
                }
            }
        }
    }

    static  Animal *find_eldest_wolf(vector<Animal *> &animals) {
        int max_id = -1;
        if (animals.empty()) return nullptr;
        for (size_t id = 0; id < animals.size(); id++) {
            if (animals[id] == nullptr) continue;
            if (animals[id]->gettype() == 1) {
                max_id = id;
                break;
            }

        }

        if (max_id ==-1) return nullptr;
        for (size_t id = max_id + 1; id < animals.size(); id++) {
            if (animals[id] == nullptr) continue;
            if (animals[id]->gettype() == 1 && animals[id]->getage() > animals[max_id]->getage()) {
                max_id = id;
            }
        }

        return animals[max_id];

    }


    void eat_animals() {

        for (int i = 0; i < xsize; i++) {
            for (int j = 0; j < ysize; j++) {

                Animal *eldestW = find_eldest_wolf(table[j * xsize + i]);

                if (eldestW != nullptr) {
                    for (int k = 0; k < table[j * xsize + i].size(); k++) {
                        if (table[j * xsize + i][k] == nullptr) continue;
                        if (table[j * xsize + i][k]->gettype() == -1) {
                            eldestW->eat();
                            delete (table[j * xsize + i][k]);
                            table[j * xsize + i][k] = nullptr;
                        }
                    }
                }
                else continue;

            }
        }
    }

    void make_animals(int time) {
        for (int i = 0; i < ysize; i++) {
            for (int j = 0; j < xsize; j++) {
                for(int k=0;k<table[j * ysize + i].size();k++){
                    if (table[j * ysize + i][k]== nullptr) continue;
                    if (table[j * ysize + i][k]->gettype() == -1 && (table[j * ysize + i][k]->getage()==5 ||
                                                                     table[j * ysize + i][k]->getage() == 10)) {
                        Rabbit *rabb = new Rabbit(i, j, table[j * ysize + i][k]->getdir(),
                                                  table[j * ysize + i][k]->getstab(), time);
                        table[j * ysize + i].emplace_back(rabb);
                    }
                    else  if (table[j * ysize + i][k]->gettype() == 1 && table[j * ysize + i][k]->getfeed()>=2){
                        Wolf * wolf = new Wolf(i,j,table[j * ysize + i][k]->getdir(),table[j * ysize + i][k]->getstab(), time);
                        table[j * ysize + i][k]->duplicate();
                        table[j*ysize+i].emplace_back(wolf);
                    }
                }

            }
        }
    }

    void kill_animals() {
        for (int i = 0; i < ysize; i++) {
            for (int j = 0; j < xsize; j++) {
                for (int k = 0; k < table[j * ysize + i].size(); k++) {
                    if (table[j * ysize + i][k] == nullptr) continue;
                    if (table[j * ysize + i][k]->gettype() == -1 && table[j * ysize + i][k]->getage() == 10) {
                        delete (table[j * ysize + i][k]);
                        table[j * ysize + i][k] = nullptr;
                        continue;
                    }
                    if (table[j * ysize + i][k]->gettype() == 1 && table[j * ysize + i][k]->getage() == 15) {
                        delete (table[j * ysize + i][k]);
                        table[j * ysize + i][k] = nullptr;
                        continue;
                    }
                }
            }

        }
    }
};

int main(){
    ifstream fin("input.txt");
    ofstream fout("output.txt");
    int N,M,T,R,W;
    fin >> N >> M >>T >> R >> W;
    Coord* rabbits_cord = new Coord[R];
    int* rabbits_dir = new int[R];
    int* rabbits_st = new int[R];

    Coord* wolves_cord = new Coord[W];
    int* wolves_dir = new int[W];
    int* wolves_st = new int[W];

    for(int i =0; i<R;i++){
        fin >> rabbits_cord[i].y >> rabbits_cord[i].x >> rabbits_dir[i] >> rabbits_st[i];
    }
    for (int i = 0;i<W;i++){
        fin >> wolves_cord[i].y >> wolves_cord[i].x >> wolves_dir[i] >> wolves_st[i];
    }

    //Wolf wolf(0,2,1,3,0);
    //wolf.Move(3,3);
    //fout << wolf.getcord().x << " " <<wolf.getcord().y;

    Simulation simulation(W,R,N,M,wolves_cord,wolves_dir,wolves_st,rabbits_cord,rabbits_dir,rabbits_st);
    simulation.run_simulation(T,fout);
    //Animal* eldest = simulation.find_eldest_wolf(simulation.table[i]);
    //fout << eldest->getdir();
    //fout << simulation.find_eldest_wolf(simulation.table[5]);
    // simulation.print_table(fout);

    delete rabbits_cord;
    delete rabbits_dir;
    delete rabbits_st;
    delete wolves_cord;
    delete wolves_dir;
    delete wolves_st;
    return 0;
}



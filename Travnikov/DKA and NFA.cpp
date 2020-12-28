#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

class Mult{

public:
    vector<int> states;
    vector<Mult *> transitions[256];
    bool start = false;
    bool finish = false;

    Mult(){

    }

    Mult(int s){
        this->addState(s);
    }

    void addState(int state){
        vector<int>::iterator it;
        for (it = this->states.begin(); it != this->states.end(); ++it){
            if(*it == state) return;
        }
        this->states.push_back(state);
        sort(this->states.begin(), this->states.end());
    }

    vector<int> getStates(){
        return this->states;
    }

    bool equals(Mult * other){
        if(this->states.size() != other->states.size()) return false;
        sort(this->states.begin(), this->states.end());
        sort(other->states.begin(), other->states.end());
        int n = this->states.size();
        for(int i=0; i<n; i++){
            if(this->states[i] != other->states[i]) return false;
        }
        return true;
    }

    void addTransition(char c, Mult * m){
        int n = this->transitions[c].size();
        for(int i=0; i<n; i++){
            if(this->transitions[c][i]->equals(m)) return;
        }
        this->transitions[c].push_back(m);
    }

    vector<Mult *>* getTransitions(){
        return this->transitions;
    }

    Mult * nextMult(char c){
        if(this->transitions[c].size() == 0) return nullptr;
        return this->transitions[c][0];
    }

    void setStart(){
        this->start = true;
    }

    void setFinish(){
        this->finish = true;
    }

    void print(){
        cout << endl << "MULT: ";
        int n = this->states.size();
        for(int i=0; i<n; i++){
            cout << this->states[i] << " ";
        }
        cout << endl;
    }

};


class Automat{
public:

    vector<Mult *> mults;
    int qit;

    Automat(){

    }

    void addMult(Mult * m){
        this->mults.push_back(m);
    }

    Mult * getMult(Mult * m){
        vector<Mult *>::iterator it;
        for (it = this->mults.begin(); it != this->mults.end(); ++it){
            if((*it)->equals(m)) return (*it);
        }
        return nullptr;
    }

    Mult * getStart(){
        vector<Mult *>::iterator it;
        for (it = this->mults.begin(); it != this->mults.end(); ++it){
            if((*it)->start) return (*it);
        }
        return nullptr;
    }

    bool process(string s){
        Mult * cur = this->getStart();
        for(int i=0; i<s.length(); i++){
            cur = cur->nextMult(s[i]);
            if(cur == nullptr) return false;
        }
        if(cur->finish) return true;
        return false;
    }

    void clearQueue(){
        this->qit = -1;
    }

    Mult * getNext(){
        this->qit++;
        if(this->mults.size() == this->qit) return nullptr;
        return this->mults[this->qit];
    }
};

Automat * createDKA(Automat * a){
    Automat * an = new Automat();
    Mult * start = a->getStart();
    an->addMult(start);

    an->clearQueue();


    Mult * cur = an->getNext();

    while(cur != nullptr) {

        for(int c=0; c<256; c++){ // Each transition
            int n = cur->transitions[c].size();
            if(n == 0) continue;
            Mult * newmult = new Mult();
            for(int i=0; i<n; i++){ // Each mult
                vector<int> states = cur->transitions[c][i]->getStates();
                int s = states.size();
                for(int j=0; j<s; j++){
                    newmult->addState(states[j]);
                }
                vector<Mult *> * transitions = cur->transitions[c][i]->getTransitions();
                for(int ct=0; ct<256; ct++){
                    int t = transitions[ct].size();
                    if(t == 0) continue;
                    for(int j=0; j<t; j++){
                        newmult->addTransition(ct, transitions[ct][j]);
                    }
                }
                if(cur->transitions[c][i]->start) newmult->setStart();
                if(cur->transitions[c][i]->finish) newmult->setFinish();
            }
            if(an->getMult(newmult) == nullptr) { // No such new mult, yet
                an->addMult(newmult);

            }
            cur->transitions[c].clear();
            cur->addTransition((char)c, newmult);

        }
        cur = an->getNext();
    }
    return an;
}

int main()
{


    ifstream ifile ("input.txt");

    int n, k, f, p, t;
    ifile >> n >> k >> f;

    Automat * a = new Automat();
    Mult * m;
    for(int i=0; i<n; i++) {
        m = new Mult();
        m->addState(i);
        if(i == k) m->setStart();
        a->addMult(m);
    }
    for(int i=0; i<f; i++) {
        int ks;
        ifile >> ks;
        a->getMult(new Mult(ks))->setFinish();
    }

    ifile >> p;

    for(int i=0; i<p; i++) {
        int from, to;
        char symb;
        ifile >> from >> to >> symb;
        a->getMult(new Mult(from))->addTransition(symb, a->getMult(new Mult(to)));
    }

    ifile >> t;

    Automat * dka = createDKA(a);

    ofstream ofile ("output.txt");

    for(int i=0; i<t; i++) {
        string str;
        ifile >> str;
        if(dka->process(str)) ofile << "YES" << endl;
        else ofile << "NO" << endl;
    }




    return 0;
}

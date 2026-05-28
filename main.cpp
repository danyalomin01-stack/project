#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <ctime>
#include <utility>
#include <fstream>
using namespace std;
int f(int x){
    return 2*x*x*x*x + 12;
}
vector<int> population_odealo(int size_population){
    vector<int> a(size_population);
    for(int i=0;i<size_population;i++){
        a[i] = i%5;
    }
    return a;
}
vector<int> population_fokus(int size_population){
    vector<int> a(size_population);
    for(int i=0;i<size_population;i++){
        a[i] = 2+rand()%3;
    }
    return a;
}
int selectParent(const vector<int>& population){
    int sum=0;
    for(int i=0;i<population.size();i++) sum+=f(population[i]);
    int r = rand()%sum;
    int cur = 0;
    for(int i =0;i<population.size();i++){
        cur += f(population[i]);
        if(cur>r) return population[i];
    }
    return population[population.size()-1];
}
int selectRandom(const vector<int>& population){
    return population[rand() % population.size()];
}
int selectElite(const vector<int>& population){
    int best = population[0];
    for(int i = 1; i < population.size(); i++){
        if(f(population[i]) > f(best)){
            best = population[i];
        }
    }
    return best;
}

int worstIndex(const vector<int>& population){
    int worst = 0;
    for(int i = 1; i < population.size(); i++){
        if(f(population[i]) < f(population[worst])){
            worst = i;
        }
    }
    return worst;
}

string toBinary(int x) {
    string s = "";
    for (int i = 2; i >= 0; i--) {
        s += char(((x >> i) & 1) + '0');
    }
    return s;
}
int toDecimal(const string& s) {
    int x = 0;
    for (int i = 0; i < 3; i++) {
        x = x * 2 + (s[i] - '0');
    }
    return x;
}
int repair(int x) {
    x %= 5;
    if (x < 0) x += 5;
    return x;
}
pair<int, int> crossoverOnePoint(int p1, int p2) {
    string a = toBinary(p1);
    string b = toBinary(p2);
    int point = 1 + rand() % 2;
    string c1 = a.substr(0, point) + b.substr(point);
    string c2 = b.substr(0, point) + a.substr(point);
    int child1 = repair(toDecimal(c1));
    int child2 = repair(toDecimal(c2));

    return {child1, child2};
}
pair<int, int> crossoverTwoPoint(int p1, int p2) {
    string a = toBinary(p1);
    string b = toBinary(p2);
    int left = 1;
    int right = 2;
    string c1 = a;
    string c2 = b;
    for (int i = left; i < right; i++) {
        swap(c1[i], c2[i]);
    }
    int child1 = repair(toDecimal(c1));
    int child2 = repair(toDecimal(c2));
    return {child1, child2};
}
pair<int, int> crossoverPMX(int p1, int p2) {
    string a = toBinary(p1);
    string b = toBinary(p2);
    int point = 1 + rand() % 2; 
    string c1 = a.substr(0, point) + b.substr(point);
    string c2 = b.substr(0, point) + a.substr(point);

    int child1 = toDecimal(c1);
    int child2 = toDecimal(c2);

    if (child1 > 4) child1 = 4;
    if (child2 > 4) child2 = 4;

    return {child1, child2};
}
pair<int, int> makeCrossover(int p1, int p2) {
    int type = rand() % 3;
    if (type == 0) return crossoverOnePoint(p1, p2);
    if (type == 1) return crossoverTwoPoint(p1, p2);
    return crossoverPMX(p1, p2);
}
string inv(string p){
    string res="";
    for(int i=p.size()-1;i>=0;i--){
        res+=p[i];
    }
    return res;
}
string trans(string p){
    char sim = p[p.size()-1];
    p.erase(p.size()-1,1);
    string res = "";
    res += sim;
    res+=p;
    return res;
}
void mutate(int &p){
    string res;
    string a = toBinary(p);
    int r = rand()%2;
    if(r==0) res = inv(a); //инверсия
    else res = trans(a); //транслокация
    p = repair(toDecimal(res));
}
int main(){
    srand(time(0));

    int size_population,count_gener,probability_cross, probability_mutation,sposob;
    cout<<"Введите размер популяции:"; cin>>size_population;
    cout<<"Введите число генераций:"; cin>>count_gener;
    cout<<"Введите вероятность кроссинговера:"; cin>>probability_cross;
    cout<<"Введите вероятность мутации:"; cin>>probability_mutation;
    cout<<"Введите способ начальной популяции (1 - одеяло, 2 - фокусировка):"; cin>>sposob;

    ofstream file("graph.csv");
    file << "generation;best_x;best_fx\n";

    //выбор популяции
    vector<int> pop;
    if(sposob==1) pop = population_odealo(size_population);
    else pop = population_fokus(size_population);
    
    cout<<"Начальная популяция = ";
    for(int i=0;i<pop.size();i++) cout<<pop[i]<<" ";
    cout<<endl;

    int best0 = pop[0];
    for(int j = 1; j < pop.size(); j++){
        if(f(pop[j]) > f(best0)) best0 = pop[j];
    }
    file << 0 << ";" << best0 << ";" << f(best0) << "\n";

    for(int i=0;i<count_gener;i++){
        vector<int> childrenPool;

        for(int pairNumber = 0; pairNumber < 2; pairNumber++){
            int parent1, parent2;

            if(pairNumber == 0){
                parent1 = selectRandom(pop);
                parent2 = selectRandom(pop);
            }
            else{
                parent1 = selectElite(pop);
                parent2 = selectRandom(pop);
            }

            pair<int,int> children;
            if (rand() % 100 < probability_cross) children = makeCrossover(parent1, parent2);
            else children = {parent1, parent2};

            int child1 = children.first;
            int child2 = children.second;

            if (rand()%100 < probability_mutation) mutate(child1);
            if (rand()%100 < probability_mutation) mutate(child2);

            child1 = repair(child1);
            child2 = repair(child2);

            childrenPool.push_back(child1);
            childrenPool.push_back(child2);
        }

        int bestChild = selectElite(childrenPool);

        file << i + 1 << ";" << bestChild << ";" << f(bestChild) << "\n";

        int worst = worstIndex(pop);
        if(f(bestChild) > f(pop[worst])){
            pop[worst] = bestChild;
        }

        cout<<i+1<<" генерация = ";
        for(int j=0;j<pop.size();j++) cout<<pop[j]<<" ";
        cout<<endl;

    }
    int best = pop[0];
    for(int i = 1; i < pop.size(); i++){
        if(f(pop[i]) > f(best)){
            best = pop[i];
        }
    }
    cout << "Лучший = " << best << endl;
    cout << "f(x) = " << f(best) << endl;
    file.close();
}
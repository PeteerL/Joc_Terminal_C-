#pragma once

#include<vector>
#include<iostream>
#include<cstdlib>
#include<stdlib.h>
#include<ctime>
#include<fstream>
#include<string>

#include <windows.h>



using namespace std;
class Scene;
class GameObject{
    private:
        int x_, y_;
    public: 
        const int &x, &y;
        void setPosition(int x, int y);
        static bool canMove(Scene* scene, int x, int y);
        GameObject(): x_(0), x(x_), y_(0), y(y_) {}
        GameObject(int x, int y): x_(x), x(x_), y_(y), y(y_) {}
};

class Border: public GameObject{
    public:
        Border(){};
        Border(int x, int y);
};

class Enemy: public GameObject{
    public:
        vector<GameObject*> detectionRange;
        Enemy(){};
        Enemy(int x, int y);
        void Move(int relative_x, int relative_y);
        void printRange();
};

class Player: public GameObject{
    private:
        int energy, level;
        bool game_state;
    public:
        Player():energy(3), level(1), game_state(true) {};
        Player(int energy, int level, bool game_state, int x, int y);

        void setLevel(int val){this->level = val;}
        int getLevel(){return this->level;}
        void setEnergy(int val){this->energy = val;}
        int getEnergy(){return this->energy;}
        void setState(bool val){this->game_state = val;}
        bool getState(){return this->game_state;}

        void Move(Scene* scene, const string& dir);
};

class Energy: public GameObject{
    public:
        Energy(){};
        Energy(int x, int y);
};

class Scene{
    private:
        float enemyChance, energyChance;
        int rows, collumns;
        char screen[50][50];
    public:
        Scene(){};
        vector<Border*> borderList;
        vector<Enemy*> enemyList;
        vector<Energy*> energyList;
        Player player;

        void setEnemyC(int val){this->enemyChance = val;}
        float getEnemyC(){return this->enemyChance;}
        void setEnergyC(int val){this->energyChance = val;}
        float getEnergyC(){return this->energyChance;}
        void setRows(int val){this->rows = val;}
        int getRows(){return this->rows;}
        void setCollumns(int val){this->collumns = val;}
        int getCollumns(){return this->collumns;}

        void Reset(int enemyChance, int energyChance, int rows, int collumns);
        void Sync();
        void setEntities(int enemyChance, int energyChance);
        void GameState(bool);
        void MoveEnemy();

};



Enemy::Enemy(int x, int y){
    this->setPosition(x, y);
    this->detectionRange.push_back(new GameObject(x+1, y+1));
    this->detectionRange.push_back(new GameObject(x+1, y));
    this->detectionRange.push_back(new GameObject(x+1, y-1));
    this->detectionRange.push_back(new GameObject(x, y-1));
    this->detectionRange.push_back(new GameObject(x, y+1));
    this->detectionRange.push_back(new GameObject(x-1, y-1));
    this->detectionRange.push_back(new GameObject(x-1, y));
    this->detectionRange.push_back(new GameObject(x-1, y+1));
};

void Scene::MoveEnemy(){
    for(auto & enemy:this->enemyList){
        int direction = rand() % 5;
        switch (direction)
        {
            case 1:
                if(GameObject::canMove(this, enemy->x+1, enemy->y)){
                    enemy->setPosition(enemy->x+1, enemy->y);
                    for(auto & visual:enemy->detectionRange){
                        visual->setPosition(visual->x+1, visual->y);
                    }
                }
                break;
            case 2:
                if(GameObject::canMove(this, enemy->x, enemy->y+1)){
                    enemy->setPosition(enemy->x, enemy->y+1);
                    for(auto & visual:enemy->detectionRange){
                        visual->setPosition(visual->x, visual->y+1);
                    }
                }
                break;
            case 3:
                if(GameObject::canMove(this, enemy->x-1, enemy->y)){
                    enemy->setPosition(enemy->x-1, enemy->y);
                    for(auto & visual:enemy->detectionRange){
                        visual->setPosition(visual->x-1, visual->y);
                    }
                }
                break;
            case 4:
                if(GameObject::canMove(this, enemy->x, enemy->y-1)){
                    enemy->setPosition(enemy->x, enemy->y-1);
                    for(auto & visual:enemy->detectionRange){
                        visual->setPosition(visual->x, visual->y-1);
                    }
                }
                break;
        }
    }
    this->Sync();
    if(this->energyList.size() != 0)
        this->GameState(false);

}


void GameObject::setPosition(int x, int y){
    this->x_ = x;
    this->y_ = y;
}

bool GameObject::canMove(Scene *s, int x, int y){
    for(auto &element: s->enemyList){
        if(element->x == x and element->y == y)
            return false;
    }

    for(auto &element: s->borderList){
        if(element->x == x and element->y == y)
            return false;
    }

    return true;
}


Border::Border(int x, int y){
    this->setPosition(x, y);
}

Energy::Energy(int x, int y){
    this->setPosition(x, y);
}

Player::Player(int energy, int level, bool gameState, int x, int y){
    this->energy = energy;
    this->level = level;
    this->game_state = gameState;
    this->setPosition(x, y);
}

void Scene::Reset(int enemyChance, int energyChance, int rows, int collumns){
    ofstream fout("log.out");
    fout<<"rows:"<<rows<<" collumns:"<<collumns;
    this->borderList.clear();
    this->enemyList.clear();
    this->energyList.clear();
    for(int i = 0; i < this->rows; i++){
        for(int j = 0; j < this->collumns; j++){
            this->screen[i][j] = ' ';
        }
    }

    this->enemyChance = enemyChance;
    this->energyChance = energyChance;
    this->rows = rows;
    this->collumns = collumns;

    for(int i = 0; i < this->rows; i++){
        for(int j = 0; j < this->collumns; j++){
            if(i==0 || j==0 || i ==this->rows-1 || j == this->collumns-1){
                borderList.push_back(new Border(i, j));
                fout<<"x:"<<i<<" y:"<<j<<"\n";
            }
        }
    }
    this->player.setPosition(this->rows-2, (int)this->collumns/2);
    this->setEntities(energyChance, enemyChance);
    int count = -1;
    for(auto & element:this->energyList){
        count++;
        if(element->x == this->rows-2 and element->y == (int)this->collumns/2){
            energyList.erase(energyList.begin() + count);
        }
    }
    this->Sync();
    
}

void Scene::Sync(){
    HANDLE  hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    system("cls");
    for(int i = 0; i < this->rows; i++){
        for(int j = 0; j < this->collumns; j++){
            this->screen[i][j] = ' ';
        }
    }
    for(auto & enemy:this->enemyList){
        this->screen[enemy->x][enemy->y] = '%';
        for(auto & range:enemy->detectionRange){
            this->screen[range->x][range->y] = '~';
        }
    }

    for(auto & element:this->energyList){
        this->screen[element->x][element->y] = '*';
    }

    for(auto & element:this->borderList){
        this->screen[element->x][element->y] = '#';
    }

    this->screen[this->player.x][this->player.y] = '@';



    for(int i = 0; i < this->rows; i++){
        for(int j = 0; j < this->collumns; j++){
            switch (this->screen[i][j])
            {
            case '*':
                SetConsoleTextAttribute(hConsole, 6);
                break;

            case '%':
                SetConsoleTextAttribute(hConsole, 4);
                break;

            case '~':
                SetConsoleTextAttribute(hConsole, 7);
                break;
            
            case '#':
                SetConsoleTextAttribute(hConsole, 8);
                break;

            case '@':
                SetConsoleTextAttribute(hConsole, 2);
                break;
            
            default:
                break;
            }
            cout<<this->screen[i][j]<<" ";
        }
        cout<<endl;
    }
}

void Scene::setEntities(int enemyChance, int energyChance){
    
    srand(time(NULL));
    ofstream log("log.txt");

    cout << rand() << endl;
    int random = rand();
    for(int i = 1; i < this->rows-4; i++){
        for(int j = 1; j < this->collumns-1; j++){
            random = rand() % 201;
            log<<random<<endl;
            if(random <= enemyChance && GameObject::canMove(this, i, j)){
                this->enemyList.push_back(new Enemy(i, j));
            }

            random = rand() % 101;
            if(random <= energyChance){
                this->energyList.push_back(new Energy(i, j));
            }
        }
        cout<<endl;
    }
}

void Player::Move(Scene* scene, const string& dir) {
    auto moveInstructions = [&]() {
        if (dir == "w") {
            if (GameObject::canMove(scene, this->x - 1, this->y)) {
                this->setPosition(this->x-1, this->y);
            }
        }

        if (dir == "s") {
            if (GameObject::canMove(scene, this->x + 1, this->y)) {
                this->setPosition(this->x + 1, this->y);
            }
	    }

        if (dir == "a") {
            if (GameObject::canMove(scene, this->x, this->y - 1)) {
                this->setPosition(this->x, this->y - 1);
            }
        }

        if (dir == "d") {
            if (GameObject::canMove(scene, this->x, this->y + 1)) {
                this->setPosition(this->x, this->y + 1);
            }
        }

        if (dir == "ew" && this->energy > 0) {
            if (GameObject::canMove(scene, this->x - 4, this->y)) {
                this->setPosition(this->x - 4, this->y);
                this->setEnergy(getEnergy()-1);
            }
        }

        if (dir == "es" && this->energy > 0) {
            if (GameObject::canMove(scene, this->x + 4, this->y)) {
                this->setPosition(this->x + 4, this->y);
                this->setEnergy(getEnergy()-1);
            }
        }

        if (dir == "ea" && this->energy > 0) {
            if (GameObject::canMove(scene, this->x, this->y - 4)) {
                this->setPosition(this->x, this->y - 4);
                this->setEnergy(getEnergy()-1);
            }
        }

        if (dir == "ed" && this->energy > 0) {
            if (GameObject::canMove(scene, this->x, this->y + 4)) {
                this->setPosition(this->x, this->y + 4);
                this->setEnergy(getEnergy()-1);
            }
        }
    };

    if(this->game_state) {
        moveInstructions();
    }
    scene->MoveEnemy();
    scene->GameState(true);
    scene->Sync();
}

void Scene::GameState(bool calledByPlayer) {
    for (auto& enemy : this->enemyList) {
        for (auto&visual : enemy->detectionRange) {
            if (this->player.x == visual->x && this->player.y == visual->y) {
                this->player.setState(false);
                break;
            }
        }
    }

    for (auto it = this->energyList.begin(); it != this->energyList.end(); ++it) {
        if (this->player.x == (*it)->x && this->player.y == (*it)->y) {
            this->player.setEnergy(this->player.getEnergy() + 1);
            this->energyList.erase(it);
            break;
        }
    }

    if(this->energyList.size() == 0 and calledByPlayer){
        srand(time(NULL));
        this->player.setLevel(this->player.getLevel()+1);
        this->Reset(this->getEnemyC()+0.5, this->getEnergyC()+0.5, rand()%11 + 15, rand()%11 + 22);
    }
}
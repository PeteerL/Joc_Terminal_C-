#include"objects.h"

Scene scene;

int Start(){
    srand(time(NULL));
    scene.setEnemyC(1);
    scene.setEnergyC(1);
    scene.setRows(rand()%11 + 15);
    scene.setCollumns(rand()%11 + 22);
    scene.Reset(scene.getEnemyC(), scene.getEnergyC(), scene.getRows(), scene.getCollumns());
}

int Loop(){
    char skip;
    while(scene.player.getState()){
        string input;
        cout<<"Energy:"<<scene.player.getEnergy()<<" Level: "<<scene.player.getLevel()<<" energy left: "<<scene.energyList.size()<<endl;
        cin>>input;
        scene.player.Move(&scene, input);
    }
}

int End(){
    if (!scene.player.getState()) {
        system("cls");
        cout << "Game Over!" << endl;
        cout << "Energy collected: " << scene.player.getEnergy() << endl;

    }
}


int main(){
    Start();
    Loop();
    End();
}
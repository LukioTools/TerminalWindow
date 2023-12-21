#include <iostream>

enum sit{
    WAR,
    PEACE
};

class People{
    public:
    sit situation;
};

void MakePeace(){}
void GiveHappiness(){}

class World{
    People people;

    void UpdateGlobalStatus(){
        if (people.situation == WAR)
        {
            MakePeace();
            std::cout << "☮︎";
        }
        else{
            GiveHappiness();
            
        }
    }
};
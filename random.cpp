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

#include <iostream>
#include <map>

enum EVENT {
    NONE,
    HOVER,
    ACTIVE
};

class example {
public:
    std::string data;
};

int main() {
    std::map<EVENT, example> eventMap = {
        {NONE, {"NoneData"}},
        {HOVER, {"HoverData1"}},
        {ACTIVE, {"ActiveData"}},
        {HOVER, {"HoverData2"}}
    };

    EVENT targetEvent = HOVER;

    auto it = eventMap.find(targetEvent);
    if (it != eventMap.end()) {
        const example& foundExample = it->second;
        std::cout << "Event: " << targetEvent << ", Data: " << foundExample.data << std::endl;
    } else {
        std::cout << "Example not found for event: " << targetEvent << std::endl;
    }

    return 0;
}
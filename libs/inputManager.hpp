#pragma once

#include <atomic>
#include <iostream>
#include <stdio.h>
#include <termios.h>
#include <thread>
#include <unistd.h>
#include <vector>

#include "./queue"
#include "queue.hpp"

namespace InputManager
{

#define SET_X10_MOUSE               9
#define SET_VT200_MOUSE             1000
#define SET_VT200_HIGHLIGHT_MOUSE   1001
#define SET_BTN_EVENT_MOUSE         1002
#define SET_ANY_EVENT_MOUSE         1003

#define SET_FOCUS_EVENT_MOUSE       1004

#define SET_ALTERNATE_SCROLL        1007

#define SET_EXT_MODE_MOUSE          1005
#define SET_SGR_EXT_MODE_MOUSE      1006
#define SET_URXVT_EXT_MODE_MOUSE    1015
#define SET_PIXEL_POSITION_MOUSE    1016

#define USE_MOUSE SET_ANY_EVENT_MOUSE


#define enable_mouse(type) ("\e[?"+     std::to_string(type)    +"h")
#define disable_mouse(type) ("\e[?"+    std::to_string(type)    +"l")

class vector2{
    public:
    int x, y;
    vector2(){
        x = 0;
        y = 0;
    }

    vector2(int x, int y){
        this->x = x;
        this->y = y;
    }

    bool operator==(const vector2 other){
        return this->x == other.x && this->y == other.y;
    }
    
    bool operator!=(const vector2 other){
        return this->x != other.x || this->y != other.y;
    }

    vector2 operator+(const vector2 other){
        return vector2(this->x + other.x, this->y + other.y);
    }

    vector2 operator-(const vector2 other){
        return vector2(this->x - other.x, this->y - other.y);
    }
};

Queues::AtomicQueue<int> keyPressed;
std::atomic<vector2> mousePos;

std::atomic<bool> running(true);

int key;

std::thread t1;

void tick(){
    if (keyPressed.size() > 0)
    {
        key = keyPressed.pop_front();
    }else{
         key = 0;
    }

   
}

void noecho() {
    struct termios oldt, newt;

    // Get the current terminal settings
    tcgetattr(STDIN_FILENO, &oldt);

    // Copy the current settings to a new structure
    newt = oldt;

    // Turn off echoing in the local mode
    newt.c_lflag &= ~ECHO;

    // Disable line buffering
    newt.c_lflag &= ~ICANON;

    // Set the new terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

}

void restore_terminal_settings() {
    struct termios oldt;

    // Get the original terminal settings
    tcgetattr(STDIN_FILENO, &oldt);

    // Set the original terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

void UpdateInputManager(){

    while (running.load())
    {
        unsigned int btn = 0, x=0,y=0;
        int c, ch;
        
        ch = std::cin.get();

        if(ch != '\x1b'){
            keyPressed.push_back(ch);
            continue;
        }

        c = std::cin.get();

        ch+=c<<8;

        if(c != '\x5b'){
            continue;
        }

        c = std::cin.get();

        if(c!='\x4d'){
            ch+=c<<16;
            continue;
        }
        
        btn = std::cin.get();

        x = std::cin.get()-'\x21';
        y = std::cin.get()-'\x21';

        ch = '\xFF' + (btn<<8) + (x<<16) + (y<<24);

        mousePos.store(vector2(x,y));
        //keyPressed.store(btn);



    }
        
    //End:
    //    restore_terminal_settings();
    
}

void InitializeInputManager(){
    noecho();
    std::cout << enable_mouse(SET_X10_MOUSE) << std::flush;
    t1 = std::thread(UpdateInputManager);
}

void EndInputManager(){
    running.store(false);
    restore_terminal_settings();
    std::cout << disable_mouse(SET_X10_MOUSE) << std::flush;
}

bool GetKeyDown(int k){
    return key == k  ? true : false;
    
}

int GetKey(){
    return key;
}

vector2 GetMousePosition(){
    return mousePos;
}

};

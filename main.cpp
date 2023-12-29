#include "./libs/inputManager.hpp"
#include "./libs/WindowMangerV2.hpp"
#include "./libs/FileManager.hpp"
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <vector>


void PrintOnClick(){
    clog << "button pressed" << std::endl;
}


int main(int argc, char const *argv[])
{

    WindowManager::FullCss = FileManager::readFile("element.css");

    //{position: relative; top: 0px; left: 0px}   
    InputManager::vector2 lastMousePos(0,0);
    InputManager::InitializeInputManager();
    WindowManager::InitializeWindowManager();

    
    
    WindowManager::Window window = WindowManager::Window();

    WindowManager::Text* text = new WindowManager::Text(1, "text_1");
    text->SetText("teksti 1");
    text->id = "text_1";

    WindowManager::Button* button = new WindowManager::Button(1, "text_2");
    button->SetText("teksti 2");
    button->id = "text_2";
    button->setOnClick(PrintOnClick);

    WindowManager::Checkbox* toggle = new WindowManager::Checkbox(1, "text_1");
    toggle->SetText("toggle");
    toggle->id = "text_1";
    toggle->setOnClick(PrintOnClick);

    WindowManager::Checkbox* toggle2 = new WindowManager::Checkbox(1, "text_2");
    toggle2->SetText("toggle 2");
    toggle2->id = "text_2";
    toggle2->setOnClick(PrintOnClick);
    toggle2->AddCustomState(2, ".hehe{background-color: (255,255,255); height: 50%;}");
 
    WindowManager::Screen screen;

    toggle->friends.push_back(toggle2);
    toggle2->friends.push_back(toggle);
    
    window.id = "window";

    //window.AddElement(text);
    //window.AddElement(button);
    window.AddElement(toggle);
    window.AddElement(toggle2);

    screen.push_back(&window);

    while (true)
    {
        screen.Render(WindowManager::ScreenSize());

        InputManager::tick();

        if (InputManager::GetKeyDown('q'))
        {
            InputManager::EndInputManager();
            WindowManager::EndWindowManager();
            return 0;
        }

        if(InputManager::GetKeyDown('d')){
            //text->SetPosition(text->GetPosition() + InputManager::vector2(1,0));
        }

        if(InputManager::GetKeyDown('a')){
            //text->SetPosition(text->GetPosition() - InputManager::vector2(1,0));
        }


        if(InputManager::GetKeyDown('w')){
            //text->SetPosition(text->GetPosition() - InputManager::vector2(0,1));
        }

        if(InputManager::GetKeyDown('s')){
            //text->SetPosition(text->GetPosition() + InputManager::vector2(0,1));
        }

        if(InputManager::GetKeyDown('t')){
            CLEAR_TERMINAL()
        }

        if(InputManager::GetKey() != 0){
        }
        
        if(lastMousePos.x != InputManager::GetMousePosition().x && lastMousePos.y != InputManager::GetMousePosition().y){
            lastMousePos = InputManager::GetMousePosition();
        } 
        //clog << WindowManager::vector2(InputManager::GetMousePosition()) << std::endl;      
        //clog << InputManager::GetMouseKey() << std::endl;
        
        if(InputManager::GetMouseKey() != 0){
             
        } 
    }
    return 0;
}

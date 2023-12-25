#include "./libs/inputManager.hpp"
#include "./libs/WindowMangerV2.hpp"
#include "./libs/FileManager.hpp"
#include <fstream>
#include <iostream>
#include <unistd.h>


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

    WindowManager::Button* text2 = new WindowManager::Button(1, "text_2");
    text2->SetText("teksti 2");
    text2->id = "text_2";
    text2->setOnClick(PrintOnClick);

    WindowManager::Screen screen;
    
    window.id = "window";

    window.AddElement(text);
    window.AddElement(text2);

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

#include "./libs/inputManager.hpp"
#include "./libs/WindowMangerV2.hpp"
#include <fstream>
#include <iostream>
#include <unistd.h>

#define HIDE_CURSOR() std::cout << "\033[?25l";
#define SHOW_CURSOR() std::cout << "\033[?25h" << std::flush;

int main(int argc, char const *argv[])
{

    std::cout << enable_mouse(SET_X10_MOUSE) << std::flush;

    std::string t1_css = "{position: relative; top: 5px; left: 10px}";
    
    HIDE_CURSOR()

    InputManager::InitializeInputManager();

    InputManager::vector2 lastMousePos(0,0);

    WindowManager::Window window = WindowManager::Window();

    WindowManager::Text* text = new WindowManager::Text(1, "t1");
    text->SetCss(t1_css);
    text->SetText("moi");
    text->SetPosition(InputManager::vector2(20,2));
    text->SetSize(InputManager::vector2(20,3));

    WindowManager::Screen screen;

    //WindowManager::Text* text2 = new WindowManager::Text(1);
    //text2->SetText("ma2");
    //text2->SetPosition(InputManager::vector2(10,5));
    //text2->SetSize(InputManager::vector2(20,3));


    //window.AddElement(text2);
    window.AddElement(text);
    window.css = t1_css;

    screen.push_back(&window);

    while (true)
    {
        screen.Render(WindowManager::vector2(20,20));

        InputManager::tick();

        if (InputManager::GetKeyDown('q'))
        {
            InputManager::running.store(false);
            InputManager::restore_terminal_settings();
            std::cout << disable_mouse(SET_X10_MOUSE) << std::flush;
            SHOW_CURSOR()
            return 0;
        }

        if(InputManager::GetKeyDown('d')){
            text->SetPosition(text->GetPosition() + InputManager::vector2(1,0));
        }

        if(InputManager::GetKeyDown('a')){
            text->SetPosition(text->GetPosition() - InputManager::vector2(1,0));
        }


        if(InputManager::GetKeyDown('w')){
            text->SetPosition(text->GetPosition() - InputManager::vector2(0,1));
        }

        if(InputManager::GetKeyDown('s')){
            text->SetPosition(text->GetPosition() + InputManager::vector2(0,1));
        }

        if(InputManager::GetKeyDown('t')){
            CLEAR_TERMINAL()
        }

        if(InputManager::GetKey() != 0){
        }
        
        if(lastMousePos.x != InputManager::GetMousePosition().x && lastMousePos.y != InputManager::GetMousePosition().y){
            lastMousePos = InputManager::GetMousePosition();
        }        
    }
    return 0;
}

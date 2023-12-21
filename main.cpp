#include "./libs/inputManager.hpp"
#include "./libs/WindowMangerV2.hpp"
#include <fstream>
#include <iostream>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    std::string t1_css = "{top: 0px; left: 0px; width: 50%; height: 100%;}";
    std::string t2_css = "{top: 0px; left: 50%; width: 50%; height: 100%;}";
    std::string window_css = "{top: 0px; left: 0px; width: 100%; height: 100%;}";
    //{position: relative; top: 0px; left: 0px}   

    InputManager::InitializeInputManager();
    WindowManager::InitializeWindowManager();

    InputManager::vector2 lastMousePos(0,0);

    WindowManager::Window window = WindowManager::Window();

    WindowManager::Text* text = new WindowManager::Text(1, "t1");
    text->SetCss(t1_css);
    text->SetText("teksti 1");
    text->id = "id 1";
    text->SetCss(t1_css);
    text->bg = Color::Rgb(00,255,00);

    WindowManager::Text* text2 = new WindowManager::Text(1, "t1");
    text2->SetCss(t2_css);
    text2->SetText("teksti 2");
    text2->id = "id 2";
    text2->bg =Color::Rgb(255,00,00);

    WindowManager::Screen screen;
    
    window.id = "window";
    window.SetCss(window_css);
    window.bg = Color::Rgb(255,255,255);

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

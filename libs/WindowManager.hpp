#include "inputManager.hpp"

#include <string>
#include <variant>
#include <vector>
#include <iostream>
#include <cstdio>
#include <sstream>
#include "../clog.hpp"
// ANSI escape code for text color
#define COLOR_FG(r,g,b) "\e[38;2;" + std::to_string(r) + ';' + std::to_string(g) +  ';' + std::to_string(b) + 'm'
#define COLOR_BG(r,g,b) "\e[48;2;" + std::to_string(r) + ';' + std::to_string(g) +  ';' + std::to_string(b) + 'm'

// ANSI escape code for resetting colors
#define RESET_COLOR "\033[0m"

#define CLEAR_TERMINAL() std::cout << "\033c" << std::endl;
#define CLEAR_TERMINAL_CLOG() clog << "\033c" << std::endl;

#define ESC "\e"
#define MOVE_TO_COORDINATES(x,y) "\e[" + std::to_string(y + 1) + ";" + std::to_string(x + 1) + "H"

namespace Color{
    class Rgb{
        public:
        int r,g,b;

        Rgb(int r, int g, int b){
            this->r = r;
            this->g = g;
            this->b = b;
        }
    };
}

namespace WindowManager
{
    class RenderElement{
        public:
        std::vector<std::string> screen;

        InputManager::vector2 size;

        int PosToIdx(InputManager::vector2 pos){
            return pos.y*size.x + pos.x;
        }

        RenderElement(InputManager::vector2 size){
            this->size = size;
            for (int i = 0; i < size.x*size.y; i++)
            {
                screen.push_back(COLOR_BG(100,100,100) + " " + RESET_COLOR);
            }   
        }

        void SetPixel(InputManager::vector2 pos, std::string value){
            screen[PosToIdx(pos)] = value;
        }

        std::string GetPixel(InputManager::vector2 pos){
            return screen[PosToIdx(pos)];
        }
    };

    class Element{
        public:
        virtual float GetPreferedAspectRatio() const {
            return 0.0f;
        }

        virtual RenderElement Render(InputManager::vector2 size) const {}

        virtual void SetPosition(InputManager::vector2 pos){}

        virtual void SetSize(InputManager::vector2 pos){}

        virtual InputManager::vector2 GetPosition() const {}

        virtual InputManager::vector2 GetSize() const {}

        virtual ~Element();
    };  

    Element::~Element() {}

    class Text : public Element{
        float PreferedAspectRatio = 0;
        std::string text;
        Color::Rgb bg;

        InputManager::vector2 position, size;

        public:
        Text(float pa) : bg(0, 0, 0) {
            PreferedAspectRatio = pa;
        }

        void SetText(std::string t){
            text = t;
        }

        void SetPosition(InputManager::vector2 pos) override{
            position = pos;
        }

        void SetSize(InputManager::vector2 s) override{
            size = s;
        }

        InputManager::vector2 GetPosition() const override{
            return position;
        }

        InputManager::vector2 GetSize() const override{
            return size;
        }

        float GetPreferedAspectRatio() const override {
            return PreferedAspectRatio;
        }

        RenderElement Render(InputManager::vector2 size) const override{
            RenderElement RE(size);
            InputManager::vector2 textStartPos((size.x/2)-(text.length()/2), size.y/2);
            
            for (int y = 0; y < size.y; y++)
            {
                for (int x = 0; x < size.x; x++)
                {
                    std::string r;
                    r += COLOR_BG(bg.r, bg.g, bg.b);

                    if(y==textStartPos.y && x >= textStartPos.x && x < textStartPos.x + text.length()){
                        r+= text[x-textStartPos.x];
                    }else{
                        r += "+";
                    }

                    r+=RESET_COLOR;

                    RE.SetPixel(InputManager::vector2(x,y), r);
                }   
            }
            return RE; 
        }
    };


    class Button : public Element{
        float PreferedAspectRatio = 0;
        std::string text;

        public:
        Button(float pa){
            PreferedAspectRatio = pa;
        }
        
        float GetPreferedAspectRatio() const override {
            return PreferedAspectRatio;
        }

        void Render(InputManager::vector2 size){

        }
    };

    RenderElement CombineRenderElements(RenderElement bottom, RenderElement top, InputManager::vector2 pos){

        if (pos.x > bottom.size.x && pos.y > bottom.size.y)
        {
            return bottom;
        }

        for (int y = pos.y; y < top.size.y + pos.y; y++)
        {
            for (int x = pos.x; x < top.size.x + pos.x; x++)
            {
                if(x >= bottom.size.x){
                    continue;
                }
                if ( y >= bottom.size.y)
                {
                    goto End;
                }
                bottom.SetPixel(InputManager::vector2(x,y), top.GetPixel(InputManager::vector2(x-pos.x, y-pos.y)));
            }
        }
        End:
        return bottom;
    }

    InputManager::vector2 IdxToPos(int idx, InputManager::vector2 size){
        InputManager::vector2 pos;
        pos.y = idx / size.x;
        pos.x = idx % size.x;
        return pos;
    }

    void RenderWithOptimationClog(RenderElement o, RenderElement n){
        for (int y = 0; y < o.size.y; y++)
        {
            for (int x = 0; x < o.size.x; x++)
            {
                //if(o.GetPixel((InputManager::vector2(x,y))) != n.GetPixel(InputManager::vector2(x,y))){
                    clog << MOVE_TO_COORDINATES(x,y);
                    clog << n.GetPixel(InputManager::vector2(x,y));
                    o.SetPixel(InputManager::vector2(0,0), n.GetPixel(InputManager::vector2(x,y)));
                    /*auto nw = n.GetPixel(InputManager::vector2(x,y));
                    auto ol = o.GetPixel(InputManager::vector2(x,y));
                    while (true) {
                        auto i = nw.find_first_of('\e');
                        if(i == std::variant_npos)
                            break;
                        nw.replace(i, 1, "E");
                    }

                    while (true) {
                        auto i = ol.find_first_of('\e');
                        if(i == std::variant_npos)
                            break;
                        ol.replace(i, 1, "E");
                    }

                    clog << nw << " : " << ol << std::endl;
                    //clog << x << " : " << y << std::endl;*/

                //}
                
            }
        }
        std::cout << std::flush;
    }

    
    void RenderWithOptimation(RenderElement& o, RenderElement& n){
        for (int y = 0; y < o.size.y; y++)
        {
            for (int x = 0; x < o.size.x; x++)
            {
                if(o.GetPixel((InputManager::vector2(x,y))) != n.GetPixel(InputManager::vector2(x,y))){
                    std::cout << MOVE_TO_COORDINATES(x,y);
                    std::cout << n.GetPixel(InputManager::vector2(x,y));

                    /*auto nw = n.GetPixel(InputManager::vector2(x,y));
                    auto ol = o.GetPixel(InputManager::vector2(x,y));
                    while (true) {
                        auto i = nw.find_first_of('\e');
                        if(i == std::variant_npos)
                            break;
                        nw.replace(i, 1, "E");
                    }

                    while (true) {
                        auto i = ol.find_first_of('\e');
                        if(i == std::variant_npos)
                            break;
                        ol.replace(i, 1, "E");
                    }

                    clog << nw << " : " << ol << std::endl;
                    */

                    //clog << x << " : " << y << std::endl;
                }
                
            }
        }
        std::cout << std::flush;
    }

    class Window{
        // this stores elements and handles the placemant and position of them
        std::vector<Element *> elements;
        std::vector<RenderElement> renderElements;
        RenderElement window = RenderElement(InputManager::vector2(0,0));
        RenderElement oldWindow = RenderElement(InputManager::vector2(0,0));
        public:

        Window(){

        }

        void AddElement(Element *element) {
            elements.push_back(element);
        }//used: .AddElement(new WindowManager::Text());

        void Render(InputManager::vector2 size){
            

            if (window.size != size)
            {
                CLEAR_TERMINAL();
                CLEAR_TERMINAL_CLOG();
                window = RenderElement(size);
                /*
                for (int i = 0; i < elements.size(); i++)
                {
                    RenderElement e1 = elements[0]->Render(elements[0]->GetSize());
                    window = CombineRenderElements(window, e1, InputManager::vector2(elements[0]->GetPosition()));
                }

                std::string finalStr;

                for (int y = 0; y < size.y; y++)
                {
                    for (int x = 0; x < size.x; x++)
                    {
                        finalStr += window.GetPixel(InputManager::vector2(x,y));
                    }

                    finalStr += "\n";
                    
                }

                std::cout << finalStr;
                clog << finalStr;**/
            }

            RenderElement window2 = RenderElement(size);

            for (int i = 0; i < elements.size(); i++)
            {
                RenderElement e1 = elements[0]->Render(elements[0]->GetSize());
                window2 = CombineRenderElements(window2, e1, InputManager::vector2(elements[0]->GetPosition()));
            }

            RenderWithOptimation(oldWindow, window2);
            //RenderWithOptimationClog(oldWindow, window2);
            //oldWindow = window2;
        }
    };  

    class Screen
    {
        std::vector<Window> windows;

        public:

        void AddWindow(Window w){
            windows.push_back(w);
        }
    };
    
} 

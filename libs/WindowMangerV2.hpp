#include "inputManager.hpp"
#include "../clog.hpp"
#include "./bitmask.hpp"

#include <curses.h>
#include <map>
#include <regex>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <typeindex>

#define COLOR_FG(r,g,b) "\e[38;2;" + std::to_string(r) + ';' + std::to_string(g) +  ';' + std::to_string(b) + 'm'
#define COLOR_BG(r,g,b) "\e[48;2;" + std::to_string(r) + ';' + std::to_string(g) +  ';' + std::to_string(b) + 'm'


#define COLOR_C_FG(c) "\e[38;2;" + std::to_string(c.r) + ';' + std::to_string(c.g) +  ';' + std::to_string(c.b) + 'm'
#define COLOR_C_BG(c) "\e[48;2;" + std::to_string(c.r) + ';' + std::to_string(c.g) +  ';' + std::to_string(c.b) + 'm'


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

        bool operator==(const Rgb other) const {
            return this->r == other.r && this->g == other.g && this->b == other.b;
        }
        bool operator!=(const Rgb other) const{
            return this->r != other.r || this->g != other.g || this->b != other.b;
        }
    };
}


namespace WindowManager
{
    enum POSITIONS : u_char{
        STATIC,
        RELATIVE,
        ABSOLUTE,
    };

    enum UNIT : u_char{
        ABSOLUT,
        PERCENTAGE,
    };

    class Unit{


        public:

        double v;
        UNIT u = UNIT::ABSOLUT;

        Unit(){}

        Unit(double i){
            v = i;
        };
        
        Unit(int i){
            v = (double)i;
        };

        Unit(double i, UNIT ut){
            v = i;
            u = ut;
        };
        
        Unit(int i, UNIT ut){
            v = (double)i;
            u = ut;
        };


        bool operator==(const Unit o){
            return this->v == o.v && this->u == o.u;
        }

        bool operator!=(const Unit o){
            return this->v != o.v || this->u != o.u;
        }

        Unit operator+(Unit o){
            return this->v + o.v;
        }  

        Unit operator-(Unit o){
            return this->v - o.v;
        }  

        Unit operator*(Unit o){
            return this->v * o.v;
        } 


        Unit operator/(Unit o){
            return this->v / o.v;
        } 

        bool operator>(Unit other){
            return this->v > other.v;
        }

        bool operator<(Unit other){
            return this->v < other.v;
        }

        bool operator>=(Unit other){
            return this->v >= other.v;
        }

        bool operator<=(Unit other){
            return this->v <= other.v;
        }
    };

    bool operator<(int lhs, const Unit& rhs) {
            return static_cast<double>(lhs) < rhs.v;
        }

    bool operator>(int lhs, const Unit& rhs) {
            return static_cast<double>(lhs) > rhs.v;
        }

    bool operator<=(int lhs, const Unit& rhs) {
            return static_cast<double>(lhs) <= rhs.v;
        }

    bool operator>=(int lhs, const Unit& rhs) {
            return static_cast<double>(lhs) >= rhs.v;
        }

    bool operator==(int lhs, const Unit& rhs) {
            return lhs == rhs.v;
        }

    double operator-(double lhs, const Unit& rhs) {
            return lhs - rhs.v;
        }

    class vector2{
        public:
        Unit x, y;

        vector2(){
            x = Unit(0);
            y = Unit(0);
        }

        vector2(Unit x, Unit y){
            this->x = x;
            this->y = y;
        }

        vector2(InputManager::vector2 v){
            this->x = v.x;
            this->y = v.y;
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

    class Pixel
    {
        public:
        char ch;
        Color::Rgb bg =  Color::Rgb(0,0,0);
        Color::Rgb fg =  Color::Rgb(0,0,0);;
        enum Attr : unsigned char{
            BOLD,
            DIM,
            ITALIC,
            UNDERLINE,
            BLINK,
            STRIKETROUGH,
        };
        
        BitMask<1> attr;
        
        Pixel(){
            bg = Color::Rgb(0,0,0);
            fg = Color::Rgb(0,0,0);
        }

        Pixel(char c){
            bg = Color::Rgb(0,0,0);
            fg = Color::Rgb(0,0,0);
            ch = c;
        }

        Pixel(char c, Color::Rgb bg){
            this->bg = bg;
            fg = Color::Rgb(0,0,0);
            ch = c;
        }

        friend std::ostream& operator<<(std::ostream& os, Pixel p){
            os << COLOR_C_BG(p.bg) << COLOR_C_FG(p.fg) /*do the attributes sometime*/ << p.ch << RESET_COLOR;
            return os;
        }

        bool operator==(const Pixel &p) {
            return p.ch == this->ch && p.bg == this->bg && p.fg == this->fg && p.bg == this->bg && p.fg == this->fg;
        }

        bool operator!=(const Pixel &p) {
            return p.ch != this->ch || p.bg != this->bg || p.fg != this->fg || p.bg != this->bg || p.fg != this->fg;
        }

    };

    class RenderElement{
        public:
        std::vector<Pixel> screen;

        vector2 size;

        Unit PosToIdx(vector2 pos){
            return pos.y*size.x + pos.x;
        }

        RenderElement(vector2 size){
            this->size = size;
            for (int i = 0; i < (size.x*size.y); i++)
            {
                screen.push_back(Pixel());
            }   
        }

        RenderElement(vector2 size, char defaultChar){
            this->size = size;
            for (int i = 0; i < size.x*size.y; i++)
            {
                screen.push_back(Pixel(defaultChar, Color::Rgb(100,100,100)));
            }   
        }

        RenderElement(RenderElement& r){
            screen = r.screen;
            size = r.size;
        }

        void SetPixel(vector2 pos, Pixel value){
            screen[PosToIdx(pos).v] = value;
        }

        Pixel GetPixel(vector2 pos){
            return screen[PosToIdx(pos).v];
        }
    };

    class Element{
        public:

        double aspectRatio = 0;

        std::string id;
        std::string css;

        virtual double GetPreferedAspectRatio() const {
            return aspectRatio;
        }

        virtual void SetCss(std::string cs){
            css = cs;
        }

        static std::map<std::string, void(*)(std::string, Element *)> map;

        POSITIONS position_type = STATIC;

        virtual RenderElement Render(vector2 size) const {}

        virtual void SetPosition(vector2 pos){}

        virtual void SetPosition_X(Unit x){}

        virtual void SetPosition_Y(Unit y){}

        virtual void SetHeight(Unit h){}

        virtual void SetWidth(Unit w){}

        virtual void SetSize(vector2 pos){}

        virtual vector2 GetPosition() const {}

        virtual vector2 GetSize() const {}

        virtual void ExtraParse(std::string css){}

        virtual void ParseCss(std::string css){
            std::vector<std::string> lines;
            
            {
                auto a = css.find_first_of('{');
                css = css.substr(a, css.find_last_of('}')-a);
            }

            size_t pos = 0;
            while (true)
            {
                size_t c = css.find(";", pos);
                
                if(c == std::variant_npos)
                    break;

                lines.push_back(css.substr(pos, c-pos));
                pos = c + 1;
            }

            std::vector<std::thread> thr;

            for (int i = 0; i < lines.size(); i++)
            {
                auto cb = map[lines[i]];
                if(cb) 
                    thr.emplace_back(cb, lines[i], this);
            }

            for(auto& t : thr){
                if (t.joinable())
                {
                    t.join();
                }   
            }         
        }

        virtual ~Element();
        };  

    Element::~Element() {}

    std::map<std::string, void(*)(std::string, Element *)>  Element::map = std::map<std::string, void(*)(std::string, Element *)> ({
        std::make_pair("position", [](std::string css, Element * E){
            std::regex relative(".*relative;?");
            std::regex absolute(".*absolute;?");
            if(std::regex_match(css, relative)){
                E->position_type = POSITIONS::RELATIVE;
            } 
            else if(std::regex_match(css, absolute)){
                E->position_type = POSITIONS::ABSOLUTE;
            }
        }),

        std::make_pair("top", [](std::string css, Element * E){
             Unit u = 0;

             std::regex px(".*px;?");
             std::regex per(".*%;?");

            size_t dd = css.find_first_of(':');
        
            if(std::regex_match(css, px)){
                size_t end = css.find("px");
                u = Unit(std::stod(css.substr(dd+1, end-1)), UNIT::ABSOLUT);
            }
            else if(std::regex_match(css, per)){
                size_t end = css.find_first_of('%');
                u = Unit(std::stod(css.substr(dd+1, end-1)), UNIT::PERCENTAGE);
            }

            E->SetPosition_Y(u);
        }),

        std::make_pair("left", [](std::string css, Element * E){
             Unit u = 0;

             std::regex px(".*px;?");
             std::regex per(".*%;?");

            size_t dd = css.find_first_of(':');
        
            if(std::regex_match(css, px)){
                size_t end = css.find("px");
                u = Unit(std::stod(css.substr(dd+1, end-1)), UNIT::ABSOLUT);
            }
            else if(std::regex_match(css, per)){
                size_t end = css.find_first_of('%');
                u = Unit(std::stod(css.substr(dd+1, end-1)), UNIT::PERCENTAGE);
            }
            
            E->SetPosition_X(u);
        }),

        std::make_pair("height", [](std::string css, Element * E){
             Unit u = 0;

             std::regex px(".*px;?");
             std::regex per(".*%;?");

            size_t dd = css.find_first_of(':');
        
            if(std::regex_match(css, px)){
                size_t end = css.find("px");
                u = Unit(std::stod(css.substr(dd+1, end-1)), UNIT::ABSOLUT);
            }
            else if(std::regex_match(css, per)){
                size_t end = css.find_first_of('%');
                u = Unit(std::stod(css.substr(dd+1, end-1)), UNIT::PERCENTAGE);
            }
            
            E->SetHeight(u);
        }),

        std::make_pair("width", [](std::string css, Element * E){
             Unit u = 0;

             std::regex px(".*px;?");
             std::regex per(".*%;?");

            size_t dd = css.find_first_of(':');
        
            if(std::regex_match(css, px)){
                size_t end = css.find("px");
                u = Unit(std::stod(css.substr(dd+1, end-1)), UNIT::ABSOLUT);
            }
            else if(std::regex_match(css, per)){
                size_t end = css.find_first_of('%');
                u = Unit(std::stod(css.substr(dd+1, end-1)), UNIT::PERCENTAGE);
            }
            
            E->SetWidth(u);
        }),
 
        std::make_pair("aspect-ratio", [](std::string css, Element * E){
            std::regex getNumber(":(\\d+)");
            std::smatch match;

            size_t i = css.find_first_of('/');
            size_t kp = css.find_first_of(':');
            size_t sc = css.find_first_of(';');
            double u = std::stod(css.substr(kp+1, i));
            double d = std::stod(css.substr(i +1 , sc));

            E->aspectRatio = u/d;
        }),
    });

    class Text : public Element{
        std::string text;
        Color::Rgb bg;

        vector2 position, size;

        public:
        Text(float pa, std::string eid) : bg(0, 0, 0){
            aspectRatio = pa;
            id = eid;
        }

        void SetText(std::string t){
            text = t;
        }

        void SetPosition(vector2 pos) override{
            position = pos;
        }

        void SetSize(vector2 s) override{
            size = s;
        }

        void SetPosition_X(Unit x) override{
            position.x = x;
        }

        void SetPosition_Y(Unit y) override{}

        void SetHeight(Unit h) override{}

        void SetWidth(Unit w) override{}

        vector2 GetPosition() const override{
            return position;
        }

        vector2 GetSize() const override{
            return size;
        }

        RenderElement Render(vector2 size) const override{
            RenderElement RE(size);
            vector2 textStartPos((size.x/2)-(double)(text.length()/2), size.y/2);
            
            for (int y = 0; y < size.y; y++)
            {
                for (int x = 0; x < size.x; x++)
                {
                    Pixel r;
                    r.bg = Color::Rgb(bg.r, bg.g, bg.b);//COLOR_BG(bg.r, bg.g, bg.b);

                    if(y==textStartPos.y && x >= textStartPos.x && x < textStartPos.x + (double)text.length()){
                        r.ch = text[x-textStartPos.x];
                    }else{
                        r.ch = ' ';
                    }

                    RE.SetPixel(vector2(x,y), r);
                }   
            }
            return RE; 
        }
    };

    RenderElement CombineRenderElements(RenderElement bottom, RenderElement top, vector2 pos){

        if (pos.x > bottom.size.x && pos.y > bottom.size.y)
        {
            return bottom;
        }

        for (int y = pos.y.v; y < top.size.y + pos.y; y++)
        {
            for (int x = pos.x.v; x < top.size.x + pos.x; x++)
            {
                if(x >= bottom.size.x){
                    continue;
                }
                if ( y >= bottom.size.y)
                {
                    goto End;
                }
                bottom.SetPixel(vector2(x,y), top.GetPixel(vector2(x-pos.x, y-pos.y)));
            }
        }
        End:
        return bottom;
    }

    void RenderToScreen(RenderElement &oldScreen, RenderElement &newScreen){

        for (int y = 0; y < oldScreen.size.y; y++)
        {
            for (int x = 0; x < oldScreen.size.x; x++)
            {
                if (!(oldScreen.GetPixel(vector2(x,y)) == newScreen.GetPixel(vector2(x,y))))
                {
                    std::cout << MOVE_TO_COORDINATES(x,y);
                    std::cout << newScreen.GetPixel(vector2(x,y));

                    oldScreen.SetPixel(vector2(x,y), newScreen.GetPixel(vector2(x,y)));
                }
                clog2 << MOVE_TO_COORDINATES(x,y);
                clog2 << newScreen.GetPixel(vector2(x,y));
            }
        }

        std::cout << std::flush;
    }

    class Window{
        // this stores elements and handles the placemant and position of them
        std::vector<Element *> elements;
        std::vector<RenderElement> renderElements;

        std::string css;

        Color::Rgb bg;

        vector2 position, size;

        public:
        Window() : bg(0,0,0){

        }

        void AddElement(Element *element) {
            elements.push_back(element);
        }

        void SetCss(std::string cs){
            css = cs;
        }

        RenderElement Render(vector2 size){
            RenderElement window = RenderElement(size, ' ');

            // Remember to make optimations for this, so if nothing moves it should not generate everything from start, only thigns tthat changes

            for (int i = 0; i < elements.size(); i++)
            {
                RenderElement e1 = elements[i]->Render(elements[i]->GetSize());
                window = CombineRenderElements(window, e1, elements[0]->GetPosition());
            }
            return window;
        }

    };

    class Screen : public std::vector<Window>
    {
    private:

    public:
        Screen(/* args */) {}
        ~Screen() {}

        void Render(){
            std::vector<Window> windows = *this;
        }
    };
}
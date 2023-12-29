#pragma once

#include "inputManager.hpp"
#include "../clog.hpp"
#include "./bitmask.hpp"

#include <map>
#include <regex>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <typeindex>
#include <sys/ioctl.h>
#include <functional>

#define COLOR_FG(r,g,b) "\e[38;2;" + std::to_string(r) + ';' + std::to_string(g) +  ';' + std::to_string(b) + 'm'
#define COLOR_BG(r,g,b) "\e[48;2;" + std::to_string(r) + ';' + std::to_string(g) +  ';' + std::to_string(b) + 'm'


#define COLOR_C_FG(c) "\e[38;2;" + std::to_string(c.r) + ';' + std::to_string(c.g) +  ';' + std::to_string(c.b) + 'm'
#define COLOR_C_BG(c) "\e[48;2;" + std::to_string(c.r) + ';' + std::to_string(c.g) +  ';' + std::to_string(c.b) + 'm'


// ANSI escape code for resetting colors
#define RESET_COLOR "\033[0m"

#define CLEAR_TERMINAL() std::cout << "\e[2J" << std::endl;
#define CLEAR_TERMINAL_CLOG() clog << "\033c" << std::endl;

#define ESC "\e"
#define MOVE_TO_COORDINATES(x,y) "\e[" + std::to_string(y + 1) + ";" + std::to_string(x + 1) + "H"

#define ENABLE_ALT_BUF() std::cout <<  "\e[?1049h"
#define DISABLE_ALT_BUF() std::cout <<  "\e[?1049l"

#define DISABLE_WARP() std::cout <<  "\e[?7l"
#define ENABLE_WARP() std::cout <<  "\e[?7h"

#define HIDE_CURSOR() std::cout << "\033[?25l";
#define SHOW_CURSOR() std::cout << "\033[?25h" << std::flush;

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

    std::string FullCss;

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

        friend std::ostream& operator<<(std::ostream& os, Unit const & tc) {
            return os << tc.v;
        }

        operator double() const { return v;}


        bool operator==(const Unit o){
            return this->v == o.v && this->u == o.u;
        }

        bool operator!=(const Unit o){
            return this->v != o.v || this->u != o.u;
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

        friend std::ostream& operator<< (std::ostream& stream, const vector2& vec){
            return stream << vec.x << " : " << vec.y;
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

    std::vector<size_t> findAllFromString(std::string& full, std::string term){
        std::vector<size_t> positions;

        size_t pos = 0;
        while ((pos = full.find(term, pos)) != std::string::npos) {
            positions.push_back(pos);
            pos = pos + term.length();
        }

        return positions;
    }

    // Finds css for specific id from complete css file
    std::vector<std::string> findCssById(std::string &css, std::string id){

        std::vector<std::string> allCss;

        std::vector<size_t> All = findAllFromString(css, id);

        for (int i = 0; i < All.size(); i++)
        {
            size_t pos = All[i] + id.length();

            allCss.push_back(css.substr(pos, css.find_first_of('}', pos) - pos + 1));
        }
        return allCss;
    }

    std::string trim(std::string str) {
        auto start = std::find_if_not(str.begin(), str.end(), [](unsigned char c) {
            return std::isspace(c);
        });
        
        auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char c) {
            return std::isspace(c);
        }).base();
        
        return (start < end ? std::string(start, end) : std::string());
    }

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

        RenderElement(vector2 size, char defaultChar, Color::Rgb bg){
            this->size = size;
            for (int i = 0; i < size.x*size.y; i++)
            {
                screen.push_back(Pixel(defaultChar, bg));
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

    vector2 ScreenSize(){
        winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        return vector2(w.ws_col, w.ws_row);
    }

    void ConvertToAbsolute(vector2& size, vector2& pos, vector2 rsize){
        if (size.x.u == UNIT::PERCENTAGE)
        {
            size.x = round((size.x/100)*rsize.x.v);
            size.x.u = UNIT::ABSOLUT;
        }else{
            size.x.u = UNIT::ABSOLUT;
        }

        if (size.y.u == UNIT::PERCENTAGE)
        {
            size.y = round((size.y/100)*rsize.y.v);
            size.y.u = UNIT::ABSOLUT;
        } else{
            size.y.u = UNIT::ABSOLUT;
        }

        if (pos.x.u == UNIT::PERCENTAGE)
        {
            pos.x = round((pos.x/100)*rsize.x.v);
            pos.x.u = UNIT::ABSOLUT;
        }else{
            pos.x.u = UNIT::ABSOLUT;
        }

        if (pos.y.u == UNIT::PERCENTAGE)
        {
            pos.y = round((pos.y/100)*rsize.y.v);
            pos.y.u = UNIT::ABSOLUT;
        }else{
            pos.y.u = UNIT::ABSOLUT;
        }
    }

    enum EVENT{
        BEGINNING,
        NONE,
        HOVER,
        ACTIVE
    };

    class Element{

        public:
        std::map<EVENT, std::string> css_events = {
            {NONE, {}},
            {HOVER, {}},
            {ACTIVE, {}},
        };

        double aspectRatio = 0;

        bool OverrideCss = false;

        bool css_is_up_to_date = false;

        std::string id;
        std::vector<std::string> css;

        virtual double GetPreferedAspectRatio() const {
            return aspectRatio;
        }

        virtual void SetCss(std::vector<std::string> cs){
            css = cs;
        }

        virtual std::vector<std::string> GetCss(){
            return css;
        }

        static std::map<std::string, void(*)(std::string, Element *)> map;

        POSITIONS position_type = STATIC;

        virtual RenderElement Render(vector2 size) {}

        virtual void SetPosition(vector2 pos){}

        virtual void SetPosition_X(Unit x){}

        virtual void SetPosition_Y(Unit y){}

        virtual void SetHeight(Unit h){}

        virtual void SetWidth(Unit w){}

        virtual void SetSize(vector2 pos){}

        virtual void SetBackgroundColor(Color::Rgb){}

        virtual vector2 GetPosition() const {}

        virtual vector2 GetSize() const {}

        virtual void ExtraParse(std::string css){}

        virtual void cssSelectorSepreator(){

            for (int i = 0; i < css.size(); i++)
            {
                size_t br = css[i].find_first_of('{');
                std::string defaul = css[i]; 

                if (css[i].substr(0, br).find("hover") != std::string::npos)
                {
                    css_events.find(EVENT::HOVER)->second = defaul;
                }
                else if (css[i].substr(0, br).find("active") != std::string::npos)
                {
                    css_events.find(EVENT::ACTIVE)->second = defaul;
                }
                else{
                    css_events.find(EVENT::NONE)->second = defaul;
                }
            }
        }

        virtual void ParseCss(EVENT e){

            std::vector<std::string> lines;

            cssSelectorSepreator();

            std::string defaul = css_events.find(e)->second;

            if(defaul.length() == 0){
                return;
            }

            {
                auto a = defaul.find_first_of('{');
                defaul = defaul.substr(a, defaul.find_last_of('}')-a);
            }

            size_t pos = 0;
            while (true)
            {
                size_t c = defaul.find(";", pos);
                
                if(c == std::variant_npos)
                    break;

                lines.push_back(defaul.substr(pos + 1, c-pos));
                pos = c + 1;
            }

            std::vector<std::thread> thr;

            for (int i = 0; i < lines.size(); i++)
            {
                size_t kp = lines[i].find_first_of(':');
                auto cb = map[trim(lines[i].substr(0, kp))];
                
                if(cb) 
                    thr.emplace_back(cb, lines[i], this);
            }

            for(auto& t : thr){
                if (t.joinable())
                {
                    t.join();
                }   
            } 
            
            css_is_up_to_date = true;      
        }

        virtual void ParseCss(std::string NewCss){

            std::vector<std::string> lines;

            std::string defaul = NewCss;

            if(defaul.length() == 0){
                return;
            }

            {
                auto a = defaul.find_first_of('{');
                defaul = defaul.substr(a + 1, defaul.find_last_of('}')-a);
            }

            size_t pos = 0;


            while (true)
            {
                size_t c = defaul.find(";", pos);
                
                if(c == std::variant_npos)
                    break;

                lines.push_back(defaul.substr(pos + 1, c-pos));
                pos = c + 1;
            }

            std::vector<std::thread> thr;

            for (int i = 0; i < lines.size(); i++)
            {
                size_t kp = lines[i].find_first_of(':');
                auto cb = map[trim(lines[i].substr(0, kp))];

                
                if(cb) 
                    thr.emplace_back(cb, lines[i], this);
            }

            for(auto& t : thr){
                if (t.joinable())
                {
                    t.join();
                }   
            } 
            css_is_up_to_date = true;      
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

             std::string px = "px";
             std::string per = "%";
            size_t dd = css.find_first_of(':');

            
        
            if(css.find(px) != std::string::npos){
                size_t end = css.find("px");
                u = Unit(std::stod(css.substr(dd+1, end-1)), UNIT::ABSOLUT);
            }
            else if(css.find(per) != std::string::npos){
                size_t end = css.find_first_of('%');
                u = Unit(std::stod(css.substr(dd+1, end-1)), UNIT::PERCENTAGE);
            }
            E->SetPosition_Y(u);
        }),

        std::make_pair("left", [](std::string css, Element * E){
             Unit u = 0;
             std::string px = "px";
             std::string per = "%";

            size_t dd = css.find_first_of(':');

        
            if(css.find(px) != std::string::npos){
                size_t end = css.find("px");
                u = Unit(std::stod(css.substr(dd+1, end-1)), UNIT::ABSOLUT);
            }
            else if(css.find(per) != std::string::npos){
                size_t end = css.find_first_of('%');
                u = Unit(std::stod(css.substr(dd+1, end-1)), UNIT::PERCENTAGE);
            }
            
            E->SetPosition_X(u);
        }),

        std::make_pair("height", [](std::string css, Element * E){
             Unit u = 0;

             std::string px = "px";
             std::string per = "%";

            size_t dd = css.find_first_of(':');
        
            if(css.find(px) != std::string::npos){
                size_t end = css.find("px");
                u = Unit(std::stod(css.substr(dd+1, end-1)), UNIT::ABSOLUT);
            }
            else if(css.find(per) != std::string::npos){
                size_t end = css.find_first_of('%');
                u = Unit(std::stod(css.substr(dd+1, end-1)), UNIT::PERCENTAGE);
            }
            
            E->SetHeight(u);
        }),

        std::make_pair("width", [](std::string css, Element * E){
             Unit u = 0;

            std::string px = "px";
            std::string per = "%";

            size_t dd = css.find_first_of(':');
        
            if(css.find(px) != std::string::npos){
                size_t end = css.find("px");
                u = Unit(std::stod(css.substr(dd+1, end-1)), UNIT::ABSOLUT);
            }
            else if(css.find(per) != std::string::npos){
                size_t end = css.find_first_of('%');
                u = Unit(std::stod(css.substr(dd+1, end-1)), UNIT::PERCENTAGE);
            }
            
            E->SetWidth(u);
        }),
 
        std::make_pair("aspect-ratio", [](std::string css, Element * E){
            std::smatch match;

            size_t i = css.find_first_of('/');
            size_t kp = css.find_first_of(':');
            size_t sc = css.find_first_of(';');
            double u = std::stod(css.substr(kp+1, i));
            double d = std::stod(css.substr(i +1 , sc));

            E->aspectRatio = u/d;
        }),

        std::make_pair("background-color", [](std::string css, Element * E){

            std::smatch match;
            
            size_t kp = css.find_first_of(':');
            size_t fbr = css.find_first_of('(', kp);
            size_t sc = css.find_first_of(';');

            size_t last = fbr + 1;
            
            Color::Rgb color(0,0,0);
            
            size_t v = css.find_first_of(',', last + 1);
            color.r = std::stod(css.substr(last, v - last));
            last = v + 1;

            v = css.find_first_of(',', last + 1);
            color.g = std::stod(css.substr(last, v - last));
            last = v + 1;

            v = css.find_first_of(',', last + 1);
            color.b = std::stod(css.substr(last, v - last));
            last = v + 1;

            E->SetBackgroundColor(color);
            
        }),
    });

    class Text : public Element{
        public:
        std::string text;
        Color::Rgb bg;

        vector2 position, size;

        
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

        void SetPosition_Y(Unit y) override{
            position.y = y;
        }

        void SetHeight(Unit h) override{
            size.y = h;
        }

        void SetWidth(Unit w) override{
            size.x = w;
        }

        void SetBackgroundColor(Color::Rgb c) override {
            bg = c;
        }

        vector2 GetPosition() const override{
            return position;
        }

        vector2 GetSize() const override{
            return size;
        }

        void SetCss(std::vector<std::string> cs) override{
            css = cs;
        }

        RenderElement Render(vector2 size) override{
            RenderElement RE(size);
            vector2 textStartPos((size.x/2.0)-(double)(text.length()/2.0), size.y/2.0);
            
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

    class Button : public Element{
        public:
            std::string text;

            vector2 position, size;

            Color::Rgb bg;
            Color::Rgb highlight;
            Color::Rgb pressed;

            Button(float pa, std::string eid) : bg(0, 0, 0), highlight(0,0,0), pressed(0,0,0){
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

            void SetPosition_Y(Unit y) override{
                position.y = y;
            }

            void SetHeight(Unit h) override{
                size.y = h;
            }

            void SetWidth(Unit w) override{
                size.x = w;
            }

            void SetCss(std::vector<std::string> cs) override{
                css = cs;
            }

            vector2 GetPosition() const override{
                return position;
            }

            vector2 GetSize() const override{
                return size;
            }

            void SetBackgroundColor(Color::Rgb c) override {
                bg = c;
            }

            bool Inside(){
                vector2 siz = size;
                vector2 pos = position;

                ConvertToAbsolute(siz, pos, ScreenSize());

                vector2 cursorPos = InputManager::GetMousePosition();
                if ((cursorPos.x >= pos.x && (double)cursorPos.x <= (double)(pos.x + siz.x)) && (cursorPos.y >= pos.y && (double)cursorPos.y <= (double)(pos.y + siz.y)))
                {
                    return true;
                }else{
                    return false;
                }
            }

            using CallbackFunction = std::function<void()>;

            void setOnClick(CallbackFunction callback) {
                callback_ = callback;
            }


            RenderElement Render(vector2 size) override{
                EVENT e;

                bool onHighlight = Inside();
                if (InputManager::GetMouseKey() == 32 && onHighlight)
                {
                    e = EVENT::ACTIVE;
                }else if (onHighlight){
                    e = EVENT::HOVER;
                }else{
                    e = EVENT::NONE;
                }
                
                if (lastEvent != e)
                {

                    ParseCss(css_events.find(e)->second);

                    lastEvent = e;

                    if (e == EVENT::ACTIVE)
                    {
                        if (callback_ != nullptr)
                        {
                            callback_();
                        }     
                    }     
                }

                RenderElement RE(size);
                vector2 textStartPos((size.x/2.0)-(double)(text.length()/2.0), size.y/2.0);
                
                for (int y = 0; y < size.y; y++)
                {
                    for (int x = 0; x < size.x; x++)
                    {
                        Pixel r;
                        r.bg = bg;

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
         

        private:
            CallbackFunction callback_;
            EVENT lastEvent = EVENT::BEGINNING;
    };

    class Checkbox : public Element{
        public:

            std::map<int, std::string> CustomStates = {
                {2, {}},
            };


            std::string text;

            vector2 position, size;

            Color::Rgb bg;
            Color::Rgb highlight;
            Color::Rgb pressed;

            std::vector<int> states = {0, 1, 2};

            int state = 0;
            int lastState = states[0];

            std::vector<Checkbox *> friends;

            Checkbox(float pa, std::string eid) : bg(0, 0, 0), highlight(0,0,0), pressed(0,0,0){
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

            void SetPosition_Y(Unit y) override{
                position.y = y;
            }

            void SetHeight(Unit h) override{
                size.y = h;
            }

            void SetWidth(Unit w) override{
                size.x = w;
            }   

            void SetCss(std::vector<std::string> cs) override{
                css = cs;
            }

            vector2 GetPosition() const override{
                return position;
            }

            vector2 GetSize() const override{
                return size;
            }

            void SetBackgroundColor(Color::Rgb c) override {
                bg = c;
            }

            void Circle(){
                state = state % states.size();
            }

            void toggle(){
                AddState();
            }

            void AddState(){
                for (Checkbox* togglePtr : friends)
                {
                    togglePtr->DisableFromOther(id);
                }

                state += 1; 
                clog << "before: " << state << std::endl;
                Circle();
                clog << "after: " << state << std::endl;
            }

            void SubstractState(){
                state -= 1;
                clog << "before: " << state << std::endl;
                Circle();
                clog << "after: " << state << std::endl;

            }

            void SetZero(){
                state = false;
            }

            void DisableFromOther(std::string killer){
                state = false;
            }

            bool Inside(){
                vector2 siz = size;
                vector2 pos = position;

                ConvertToAbsolute(siz, pos, ScreenSize());

                vector2 cursorPos = InputManager::GetMousePosition();
                if ((cursorPos.x >= pos.x && (double)cursorPos.x <= (double)(pos.x + siz.x)) && (cursorPos.y >= pos.y && (double)cursorPos.y <= (double)(pos.y + siz.y)))
                {
                    return true;
                }else{
                    return false;
                }
            }

            using CallbackFunction = std::function<void()>;

            void setOnClick(CallbackFunction callback) {
                callback_ = callback;
            }


            RenderElement Render(vector2 size) override{
                EVENT e;
                bool eventDifference =false;
                bool onHighlight = Inside();
                if (InputManager::GetMouseKey() == 32 && onHighlight)
                {
                    e = EVENT::ACTIVE;
                }else if (onHighlight){
                    e = EVENT::HOVER;
                }else{
                    e = EVENT::NONE;
                }
                
                if (lastEvent != e)
                {
                    if (e == EVENT::ACTIVE)
                    {
                        toggle();    
                        callback_();
                        
                    }
                    eventDifference = true;
                    
                }

                

                if (state != lastState || lastEvent != e)
                {
                    std::string current_css;

                    if (state == 0 && onHighlight)
                    {
                        current_css = css_events.find(EVENT::HOVER)->second;
                    }
                    else if(state == 0 && !onHighlight){
                        current_css = css_events.find(EVENT::NONE)->second;
                    }
                    else if(state == 1){
                        current_css = css_events.find(EVENT::ACTIVE)->second;
                    }
                    else{
                        current_css = CustomStates.find(state)->second;
                    }

                    ParseCss(current_css);

                    lastState = state;
                    lastEvent = e;
                }

                

                RenderElement RE(size);
                vector2 textStartPos((size.x/2.0)-(double)(text.length()/2.0), size.y/2.0);
 
                for (int y = 0; y < size.y; y++)
                {
                    for (int x = 0; x < size.x; x++)
                    {
                        Pixel r;
                        r.bg = bg;

                        if(y==(int)textStartPos.y && x >= (int)textStartPos.x && x < (int)textStartPos.x + (double)text.length()){
                            r.ch = text[x-textStartPos.x];
                        }else{
                            r.ch = ' ';
                        }

                        RE.SetPixel(vector2(x,y), r);
                    }   
                }
                
                return RE; 
            }
         

        private:
            CallbackFunction callback_;
            EVENT lastEvent = EVENT::BEGINNING;
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
            }
        }

        std::cout << std::flush;
    }

    class Window : public Element{
        public:
        // this stores elements and handles the placemant and position of them
        std::vector<Element *> elements;
        std::vector<RenderElement> renderElements;

        Color::Rgb bg;

        vector2 position, size;

        void SetId(std::string eid){
            id = eid;
        }

        std::vector<std::string> GetCss() override{
            return css;
        }

        Window() : bg(0,0,0){
        }

        void AddElement(Element *element) {
            elements.push_back(element);
        }

        void SetPosition_X(Unit x) override{
            position.x = x;
        }

        void SetPosition_Y(Unit y) override{
            position.y = y; 
        }

        void SetHeight(Unit h) override{
            size.y = h;
        }

        void SetWidth(Unit w) override {
            size.x = w;
        }

        vector2 GetPosition() const override{
            return position;
        }

        vector2 GetSize() const override{
            return size;
        }

        void SetCss(std::vector<std::string> cs) override{
            css = cs;
        }

        RenderElement Render(vector2 size){
            RenderElement window = RenderElement(size, ' ', bg);
            
            // Remember to make optimations for this, so if nothing moves it should not generate everything from start, only thigns tthat changes
            
            for (int i = 0; i < elements.size(); i++)
            {
                
                if (!elements[i]->OverrideCss && !elements[i]->css_is_up_to_date)
                {
                    elements[i]->SetCss(findCssById(FullCss, elements[i]->id)); 
                    elements[i]->ParseCss(EVENT::NONE);
                }
                
                vector2 siz = elements[i]->GetSize();
                vector2 pos = elements[i]->GetPosition();

                ConvertToAbsolute(siz, pos, size);
                
                RenderElement e1 = elements[i]->Render(siz);
                window = CombineRenderElements(window, e1, pos);
                
            }
            return window;
        }

    };

    class Screen : public std::vector<Window *>
    {
    private:
        RenderElement oldScreen = RenderElement(vector2(0,0), 'a');

    public:
        Screen(/* args */) {}
       
        void Render(vector2 size){

            
            if(oldScreen.size != size){
                oldScreen = RenderElement(vector2(size));
                //CLEAR_TERMINAL();
            }

            

            //std::vector<Window> windows = *this;

            RenderElement screen(size, ' ');

            for (int i = 0; i < this->size(); i++)
            {
                if (!(*this)[i]->OverrideCss && !(*this)[i]->css_is_up_to_date)
                {
                    (*this)[i]->SetCss(findCssById(FullCss, (*this)[i]->id)); 
                    (*this)[i]->ParseCss(EVENT::NONE);
                }

                

                vector2 siz = (*this).at(i)->size;
                vector2 pos = (*this).at(i)->position;
                
                ConvertToAbsolute(siz, pos, size);
                
                RenderElement e = (*this).at(i)->Render(siz);
                
                screen = CombineRenderElements(screen, e, pos);
                
            }          
            RenderToScreen(oldScreen, screen);    
             
        }

        ~Screen() {}
    };

    void InitializeWindowManager(){
        HIDE_CURSOR()
        ENABLE_ALT_BUF();
        DISABLE_WARP();
    }

    void EndWindowManager(){
        SHOW_CURSOR()
        DISABLE_ALT_BUF();
        ENABLE_WARP();
    }
}
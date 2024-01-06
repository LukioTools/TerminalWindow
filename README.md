This is an "Library(s)" for c++ which includes inputmanager and window/screen manager/renderer for terminal. It's not ready nor will be ready :). You have to repleace some paths from Makefile for CMake to work. 
Easiest way is to find all "/home/hiha/c++" and repleace those with path of your program. (yes the cmake configuration is almost generic so you can steal it too :9) 
As I said this is not ready, and right now it's broken. This is just to make backup from it. I dunno why it's public. here's 🍪 if you have ended up here. Maybe you had great war between you and a bug or you just wanted to find something new. Neither way good luck with your journy. Sorry for not being helpful :)


After a long journey it's ready, at least working for me. It also includes tally light program as example. Use esp8266 as the tally light and learn to read code so you can understand how it works. 

Basicly: Connect esp8266 to pc via usb, open serial port and reset the esp using the button on the esp8266. If you want to edit config like WIFI SSID... You have to send Y in 5 of establishing connection. after that, it will ask you few questions.

1. first, it asks for an id, enter 'f', this mean it doesnt have specific id, and will be configured automatically when connected to server.
2.  Next it asks for name, max lenght is 10 chars. next is the WIFI ssid, max lenght is 20 char.
3.  Next is wifi password, max lenght is 20 chars.
4.  Then is servers ip address, max lenght is 15 chars.
5.  lastly server port, normal integer.

Remember it will save those values in esp EEPROM and it has about 100 000 flash before it breaks, but you can read it indefently.

after that you can unplug it and connect it to power way you prefer. compile to code usin command: "make" it doesnt need anything else as i use cmake. This will only work in linux as it uses termios :(. After compiling run  "./main". It sould give you an GUI in terminal. Now the esp will automatically connect to the server, that is running on your computer. Remember to open port 8080 (default port in server, you can edit it, in main.cpp). "ufw allow 8080/tcp". 

You should see a popup, and tehre is your tally name, now i f***d it up and you have to sellect it, by cliking, it turns red and now you can type the place where you want to place this new tally, in this case type "cam 1", but remeber to clear the "placeholder" that doesnt work as intended. When you are ready, press Enter and now you type name that you want it to be called like "tally 1" and press enter. now your tally should appear in one of the 4 squares. After this it should work.

and you can add up to 4 tally lights, if you need more, just add more WindowManager::Checkbox to toggless vector in main.cpp, and configure it in element.css

all working css lines are
top: 0px/%;
left: 0px/%; 
width: 50px/%;
height: 50px/%; 
background-color: (56,32,100);
:active{}

if tally light disconnects for some reason like powerloss, it will say -disconnected- and if you get power back on it will automatically connect and the text -disconnected- and it will send current state to the tally light. If you have multiple tallies it will work even if there is more than one client disconnected. It's working based on the name you gave. Yes there is still one problem, if you are having more than one esp with same name, it will override the first connection. just make sure you dont do that :). 

I may do a web interface for windows. My dream is to use atem sdk so i can get real time information about the cameras. But the sdk doesn't support linux and i have no clue how it works anyway, and because SKaarhoj doesnt give anymore free api for the Switcher we are fu**d...

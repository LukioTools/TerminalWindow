#include "./libs/inputManager.hpp"
#include "./libs/WindowMangerV2.hpp"
#include "./libs/FileManager.hpp"
#include "./tcp/tcp.hpp"
#include "libs/queue.hpp"

#include <fstream>
#include <iostream>
#include <unistd.h>
#include <vector>


void PrintOnClick(){
    clog << "button pressed" << std::endl;
}

std::thread tcp_thread;
boost::asio::io_service io;
TCP::TCPServer server(io, 8080);

WindowManager::Screen screen;
WindowManager::Window window = WindowManager::Window();

std::vector<WindowManager::Checkbox*> toggless;
WindowManager::InputField* newClient;

void StateChanged(WindowManager::Checkbox& temp){
    clog << "state changed" << std::endl;
    auto it = temp.CustomAttributes.find("clientId");
    if (it != temp.CustomAttributes.end()) {
        clog << "found: " << it->second << std::endl;
        server.ChangeState(std::stoi(it->second), std::to_string(temp.state));
    }
}

bool AddClientStage = false;
std::string Newname;
std::string Newplace;

void AddNewClient(WindowManager::InputField& temp){

    if (!AddClientStage)
    {
        Newplace = temp.text;
        AddClientStage = true;
        temp.text = "";

    }

    else{
            Newname = temp.text;
            AddClientStage = false;

        for (int i = 0; i < toggless.size(); i++)
        {
            if(toggless[i]->text == Newplace){     
                int clientId = std::stoi(temp.FindCustomAttribute("clientId"));
                std::string *originalName = &server.TallyGuys.find(clientId)->second.name;

                //Sends new name to client
                if (Newname != *originalName)
                {
                    std::string message = "5: -" + Newname + "-";
                    auto client = server.TallyGuys.find(clientId)->second.client;
                    server.sendSyncMessage(client, message);  

                    //Making sure they got it right.
                    //TODO: take care of this shit
                    message = server.ReadUntilSync(client);
                    *originalName = Newname;
                }        

                toggless[i]->text = Newname;
                toggless[i]->AddCustomAttribute("clientId", std::to_string(clientId));
                toggless[i]->AddCustomAttribute("clientName", Newname);
                newClient->Enabled = false;
                return;
            }
        }
    }  
}

void NewClientConnected(std::shared_ptr<boost::asio::ip::tcp::socket> client){

    server.sendSyncMessage(client, "2");
    std::string data = server.ReadUntilSync(client);

    if(data[0] != '2'){ return; }

    server.currentId += 1;
    std::string name;

    //Sends new id to client
    std::string responce = "3: -" + std::to_string(server.currentId) + "-";
    server.sendSyncMessage(client, responce);

    //Making sure they got it right.
    //TODO: take care of this shit
    data = server.ReadUntilSync(client);

    //Get client name
    responce = "4";
    server.sendSyncMessage(client, responce);        

    //Get answer
    name = server.ReadUntilSync(client);

    size_t first = name.find_first_of('-'); 
    size_t second = name.find_first_of('-', first + 1);

    name = name.substr(first + 1, second-first-1);


    server.TallyGuys.insert({server.currentId, {name, client}});

    server.TallyGuys.find(server.currentId)->second.client = client;

    server.StartHeartbeatForClient(server.currentId);

    for (int i = 0; i < toggless.size(); i++)
        {
            if(toggless[i]->FindCustomAttribute("clientName") == name){

                newClient->CustomAttributes.find("clientId")->second = std::to_string(server.currentId);

                std::string *originalName = &server.TallyGuys.find(server.currentId)->second.name;       

                server.sendSyncMessage(client, "9");   

                toggless[i]->text = name;
                
                toggless[i]->AddCustomAttribute("clientId", std::to_string(server.currentId));
                toggless[i]->AddCustomAttribute("clientName", name);
                newClient->Enabled = false;

                server.ChangeState(server.currentId, std::to_string(toggless[i]->state));

                return;
            }
    }

    server.sendSyncMessage(client, "9");   
    
    //Display it to user, and check if user want to change info
    newClient->Enabled = true;
    newClient->CustomAttributes.find("clientId")->second = std::to_string(server.currentId);
    newClient->text = name;
}

void OnClientDisconnected(int clientId){
    server.StopHeartbeatForClient(clientId);

    //find the GUI element:
    WindowManager::Checkbox* e;
    for (int i = 0; i < toggless.size(); i++)
    {
        auto it = toggless[i]->CustomAttributes.find("clientId");
        if (it == toggless[i]->CustomAttributes.end())
            continue;

        if (it->second == std::to_string(clientId))
        {
            toggless[i]->text += "- disconnected - "; 
        }
    }
    

}

int main(int argc, char const *argv[])
{

    
    WindowManager::FullCss = FileManager::readFile("element.css");

    //{position: relative; top: 0px; left: 0px}   
    InputManager::vector2 lastMousePos(0,0);
    InputManager::InitializeInputManager();
    WindowManager::InitializeWindowManager();

    WindowManager::Text* text = new WindowManager::Text(1, "text_1");
    text->SetText("teksti 1");

    WindowManager::Button* button = new WindowManager::Button(1, "text_2");
    button->SetText("teksti 2");
    button->setOnClick(PrintOnClick);



    WindowManager::Checkbox* toggle = new WindowManager::Checkbox(1, "cam1");
    toggle->SetText("cam 1");
    toggle->setOnClick(PrintOnClick);
    toggle->AddCustomState(2, ".hehe{background-color: (255,0, 0); height: 50%;}");
    toggle->AddCustomAttribute("clientName", "cam1");
    toggle->setOnStateChange(StateChanged);

    WindowManager::Checkbox* toggle2 = new WindowManager::Checkbox(1, "cam2");
    toggle2->SetText("cam 2");
    toggle2->setOnClick(PrintOnClick);
    toggle2->AddCustomState(2, ".hehe{background-color: (255,0, 0); height: 50%;}");
    toggle2->setOnStateChange(StateChanged);

    WindowManager::Checkbox* toggle3 = new WindowManager::Checkbox(1, "cam3");
    toggle3->SetText("cam 3");
    toggle3->setOnClick(PrintOnClick);
    toggle3->AddCustomState(2, ".hehe{background-color: (255, 0, 0); height: 50%;}");
    toggle3->setOnStateChange(StateChanged);

    WindowManager::Checkbox* toggle4 = new WindowManager::Checkbox(1, "cam4");
    toggle4->SetText("cam 4");
    toggle4->setOnClick(PrintOnClick);
    toggle4->AddCustomState(2, ".hehe{background-color: (255, 0, 0); height: 50%;}");
    toggle4->setOnStateChange(StateChanged);

    toggless.push_back(toggle);
    toggless.push_back(toggle2);
    toggless.push_back(toggle3);
    toggless.push_back(toggle4);

    newClient = new WindowManager::InputField(1, "moi");
    newClient->Enabled = false;
    newClient->SetOnDone(AddNewClient);
    newClient->AddCustomAttribute("clientId", "");
    std::cout << "hei: " << newClient->id << std::endl;
    newClient->id = "InputField";
 

    toggle->friends.push_back(toggle2);
    toggle->friends.push_back(toggle3);
    toggle->friends.push_back(toggle4);

    toggle2->friends.push_back(toggle);
    toggle2->friends.push_back(toggle3);
    toggle2->friends.push_back(toggle4);

    toggle3->friends.push_back(toggle);
    toggle3->friends.push_back(toggle2);
    toggle3->friends.push_back(toggle4);

    toggle4->friends.push_back(toggle);
    toggle4->friends.push_back(toggle2);
    toggle4->friends.push_back(toggle3);


    
    window.id = "window";

    //window.AddElement(text);
    //window.AddElement(button);
    window.AddElement(toggle);
    window.AddElement(toggle2);
    window.AddElement(toggle3);
    window.AddElement(toggle4);

    window.AddElement(newClient);

    server.OnNewClient = NewClientConnected;
    server.OnDisconnectClient = OnClientDisconnected;

    screen.push_back(&window);
    boost::asio::io_service* ios = &io;
    tcp_thread = std::thread{[&ios](){ io.run(); }};


    // "main loop" is here
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

        if(InputManager::GetKeyDown('a')){
           
        }

        if(InputManager::GetKeyDown('s')){
            //text->SetPosition(text->GetPosition() + InputManager::vector2(0,1));
        }

        if(InputManager::GetKeyDown('d')){
            //text->SetPosition(text->GetPosition() + InputManager::vector2(1,0));
        }


        if(InputManager::GetKeyDown('w')){
            //text->SetPosition(text->GetPosition() - InputManager::vector2(0,1));
        }

        //if(InputManager::GetKey() != 0)
        //clog << InputManager::GetKey() << std::endl;

        
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

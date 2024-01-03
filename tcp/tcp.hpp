#pragma once

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <thread>
#include <chrono>
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <functional>

namespace TCP{

    class TCPServer {

        public:
            TCPServer(boost::asio::io_service &ioService, short port)
                :acceptor_(ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
                  socket_(ioService) {
    
                StartAccept();
            
            }


            // UNIQUE ID FOR ESP (will be saved to esp memory), {NAME, SOCKET CLIENT}
            std::map<int, std::pair<std::string, std::shared_ptr<boost::asio::ip::tcp::socket>>> TallyGuys = {
            };

            int currentId = 0;

            void ChangeState(std::string name, std::string state){
                std::shared_ptr<boost::asio::ip::tcp::socket> client = TallyGuys.find(findClient(name))->second.second;
                std::string message = "6: -" + state + "-";
                SendMessage(client, message);
            }
            
        /*
        Servers message meanings:

            "2": asks if esp has allready an id
                "3: -[id]-" sets id;
                "4": asking name;
            "5: -[name]-", sets name

            "6: -[state]-": sets current state;
        
        Client message meanings:
            "0": failed
            "1": success
            "2: f/[number]"", f = no id, [number] = has id, and it is...
            "3: -[name]-"
        */

        std::function<void()> OnNewClient;

        void SendMessage(std::shared_ptr<boost::asio::ip::tcp::socket> client, std::string message){
                boost::asio::async_write(*client, boost::asio::buffer(message),
                    boost::bind(&TCPServer::HandleWrite, this, boost::asio::placeholders::error));
            }

            void sendSyncMessage(std::shared_ptr<boost::asio::ip::tcp::socket> client, std::string message){
                boost::asio::write(*client, boost::asio::buffer(message));
            }

            std::string ReadUntilSync(std::shared_ptr<boost::asio::ip::tcp::socket> client){
                boost::system::error_code readError;
                boost::asio::read_until(*client, readBuffer_, '\n', readError);

                std::istream is(&readBuffer_);
                std::string message;
                std::getline(is, message);

                return message;
            }

        private:
            void StartAccept() {
                acceptor_.async_accept(socket_,
                    boost::bind(&TCPServer::HandleAccept, this, boost::asio::placeholders::error));
            }

            void HandleAccept(const boost::system::error_code& error) {
                if (!error) {
                    //std::cout << "Client connected: " << socket_.remote_endpoint() << std::endl;

                    auto client = std::make_shared<boost::asio::ip::tcp::socket>(std::move(socket_));

                    //SendMessage(client, "2");

                    //StartRead(client);
                    
                    clients.push_back(client);

                    if(OnNewClient){
                        OnNewClient();
                    }

                    StartAccept();

                } else {
                    std::cerr << "Error accepting connection: " << error.message() << std::endl;
                }
            }

            void BroadcastEvent(const std::string& message) {
                for (const auto& client : clients) {
                    boost::asio::async_write(*client, boost::asio::buffer(message),
                        boost::bind(&TCPServer::HandleWrite, this, boost::asio::placeholders::error));
                }
            }

            void HandleWrite(const boost::system::error_code& error) {
                if (error) {
                    std::cerr << "Error writing to client: " << error.message() << std::endl;
                }
            }

            void StartRead(const std::shared_ptr<boost::asio::ip::tcp::socket>& client) {
                boost::asio::async_read_until(*client, readBuffer_, '\n',
                boost::bind(&TCPServer::HandleRead, this, client, boost::asio::placeholders::error));
            }

            void HandleRead(const std::shared_ptr<boost::asio::ip::tcp::socket>& client, const boost::system::error_code& error) {
                if (!error) {
                    std::istream is(&readBuffer_);
                    std::string message;
                    std::getline(is, message);

                    //std::cout << "Received from client: " << message << std::endl;

                    // initialization
                    if(message[0] == '2'){
                        if(message.find_first_of('f') != std::string::npos){
                            int id;
                            std::string name;

                            currentId ++;

                            // sends new id 
                            std::string responce = "3: -" + std::to_string(currentId) + "-";
                            boost::asio::write(*client, boost::asio::buffer(responce));

                            //Making sure its good
                            boost::system::error_code readError;
                            boost::asio::read_until(*client, readBuffer_, '\n', readError);

                            std::istream is(&readBuffer_);
                            std::string message;
                            std::getline(is, message);

                            if(message[0] != '1'){
                                //std::cout << "failed: " << message;
                                return;
                            }

                            // Asking for name;
                            responce = "4";
                            boost::asio::write(*client, boost::asio::buffer(responce));

                            boost::asio::read_until(*client, readBuffer_, '\n', readError);

                            if(!readError){
                                std::istream is(&readBuffer_);
                                std::string message;
                                std::getline(is, message);

                                size_t first = message.find_first_of('-'); 
                                size_t second = message.find_first_of('-', first + 1);

                                name = message.substr(first + 1, second-first);
                                id = currentId;

                                TallyGuys.insert({id, {name, client}});

                            }else{
                                std::cerr << "Error reading from client: " << error.message() << std::endl;
                            }
                        }else{
                            size_t first = message.find_first_of('-');
                            size_t second = message.find_first_of('-', first + 1);

                            int id = std::stoi(message.substr(first + 1, second));

                            std::string responce = "4";
                            boost::asio::write(*client, boost::asio::buffer(responce));

                            boost::system::error_code readError;
                            boost::asio::read_until(*client, readBuffer_, '\n', readError);

                            if(!readError){
                                std::istream is(&readBuffer_);
                                std::string message;
                                std::getline(is, message);

                                size_t first = message.find_first_of('-'); 
                                size_t second = message.find_first_of('-', first + 1);

                                std::string name = message.substr(first + 1, second-first);

                                auto it = TallyGuys.find(id);
                                if (it != TallyGuys.end()) {
                                    it->second = {name, client};
                                } else {
                                    TallyGuys.insert({id, {name, client}});

                                }

                            }else{
                                std::cerr << "Error reading from client: " << error.message() << std::endl;
                            }
                        }
                    }

                    StartRead(client);

                } else {
                    std::cerr << "Error reading from client: " << error.message() << std::endl;

                    // If there's an error, you may want to handle disconnection or remove the client
                    // clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
                }
            }

            int findClient(std::string name){
                for (auto it = TallyGuys.begin(); it != TallyGuys.end(); ++it) {
                    if (it->second.first == name) {
                        return it->first;
                    }
                }
                return -1;
            }

            boost::asio::ip::tcp::acceptor acceptor_;
            boost::asio::ip::tcp::socket socket_;
            std::vector<std::shared_ptr<boost::asio::ip::tcp::socket>> clients;
            boost::asio::streambuf readBuffer_;
            std::thread serverThread_;
            

    };
}
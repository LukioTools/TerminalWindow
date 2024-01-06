#pragma once

#include <boost/asio/ip/udp.hpp>
#include <boost/asio/read_until.hpp>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <thread>
#include <chrono>

#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio.hpp>

#include <functional>

#include "../clog.hpp"

namespace TCP{

    class TCPServer {

        public:
            TCPServer(boost::asio::io_service &ioService, short port)
                :acceptor_(ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
                  socket_(ioService) {
    
                StartAccept();
            
            }

            struct clientData{
                std::string name;
                std::shared_ptr<boost::asio::ip::tcp::socket> client;
                std::shared_ptr<boost::asio::steady_timer> heartbeatTimer;
            };


            // UNIQUE ID FOR ESP (will be saved to esp memory), {NAME (will be saved in esp memory), SOCKET CLIENT}
            std::map<int, clientData> TallyGuys = {
            };

            int currentId = 0;

            void ChangeState(int id, std::string state){
                
                std::shared_ptr<boost::asio::ip::tcp::socket> client = TallyGuys.find(id)->second.client;
                std::string message = "6: -" + state + "-";
                sendSyncMessage(client, message);
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

            std::function<void(std::shared_ptr<boost::asio::ip::tcp::socket>)> OnNewClient;
            std::function<void(int)> OnDisconnectClient;

            void SendMessage(std::shared_ptr<boost::asio::ip::tcp::socket> client, std::string message){
                    boost::asio::async_write(*client, boost::asio::buffer(message),
                        boost::bind(&TCPServer::HandleWrite, this, boost::asio::placeholders::error));
            }

            void sendSyncMessage(std::shared_ptr<boost::asio::ip::tcp::socket> client, std::string message){
                    boost::system::error_code write_error;
                    boost::asio::write(*client, boost::asio::buffer(message), write_error);
                    //clog << "sendSyncMessage error code: " << write_error.message() << std::endl;
            }

            void sendSyncMessage(std::shared_ptr<boost::asio::ip::tcp::socket> client, std::string message, boost::system::error_code& write_error){
                    
                    boost::asio::write(*client, boost::asio::buffer(message), write_error);
                    //clog << "sendSyncMessage error code 2 : " << write_error.message() << std::endl;
            }

            std::string ReadUntilSync(std::shared_ptr<boost::asio::ip::tcp::socket> client){
                boost::system::error_code readError;
                boost::asio::read_until(*client, readBuffer_, '\n', readError);
                //clog << "ReadUntilSync error code: " << readError.message() << std::endl;

                std::istream is(&readBuffer_);
                std::string message;
                std::getline(is, message);

                return message;
            }

            std::string ReadUntilSync(std::shared_ptr<boost::asio::ip::tcp::socket> client, boost::system::error_code readError){
                boost::asio::read_until(*client, readBuffer_, '\n', readError);
                //clog << "ReadUntilSync error code 2 : " << readError.message() << std::endl;

                std::istream is(&readBuffer_);
                std::string message;
                std::getline(is, message);

                return message;
            }

            void StartHeartbeatForClient(int clientId) {
                auto clientIt = TallyGuys.find(clientId);
                if (clientIt != TallyGuys.end()) {
                    auto& heartbeatTimer = clientIt->second.heartbeatTimer;
                    heartbeatTimer = std::make_shared<boost::asio::steady_timer>(acceptor_.get_executor());
                    HeartbeatForClient(clientId);
                }
            }

            void StopHeartbeatForClient(int clientId) {
                auto clientIt = TallyGuys.find(clientId);
                if (clientIt != TallyGuys.end() && clientIt->second.heartbeatTimer) {
                    clientIt->second.heartbeatTimer->cancel();
                }
            }

        private:

            void handle_read(const boost::system::error_code& error, std::size_t bytes_transferred) {
            }


            void StartAccept() {
                acceptor_.async_accept(socket_,
                    boost::bind(&TCPServer::HandleAccept, this, boost::asio::placeholders::error));
            }

            void HandleAccept(const boost::system::error_code& error) {
                if (!error) {
                    auto client = std::make_shared<boost::asio::ip::tcp::socket>(std::move(socket_));            
                    clients.push_back(client);
                    if(OnNewClient){
                        OnNewClient(clients[clients.size()-1]);
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
                    clog << "async receive: " << error.message() << std::endl;
            }

            int findClient(std::string name){
                for (auto it = TallyGuys.begin(); it != TallyGuys.end(); ++it) {
                    if (it->second.name == name) {
                        return it->first;
                    }
                }
                return -1;
            }

            void HandleTimeout(const boost::system::error_code& error) {
                if (error != boost::asio::error::operation_aborted) {
                    // Timeout occurred
                    // You can handle the timeout here and cancel the read operation if needed
                    //clientSocket->cancel(); // Cancel the read operation
                    clog << "timeout" << std::endl;
                }
            }

            void HeartbeatForClient(int clientId){
                auto clientIt = TallyGuys.find(clientId);

                if (clientIt == TallyGuys.end()) {return;}

                auto& clientSocket = clientIt->second.client;
                auto& heartbeatTimer = clientIt->second.heartbeatTimer;

                sendSyncMessage(clientSocket, "h");

                boost::system::error_code _error_code;

                std::atomic<bool> terminateThread(false);
                std::atomic<bool> died(false);

                std::thread th = std::thread([&](){
                    int count = 0;
                    while (!terminateThread) {
                        count ++;
                        if(count == 2000) break;
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    }
                    if (!terminateThread)
                    {
                        died = true;
                        clientSocket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, _error_code);
                    }
                });

                ReadUntilSync(clientSocket);
                terminateThread = true;

                th.join();

                if(died){
                    OnDisconnectClient(clientId);
                    return;
                }

                if(_error_code)
                {
                    OnDisconnectClient(clientId);
                }else{
                    boost::system::error_code error;
                    heartbeatTimer->expires_after(std::chrono::seconds(2));
                    heartbeatTimer->async_wait([this, clientId](const boost::system::error_code& error) {
                        if (!error) {
                            HeartbeatForClient(clientId);
                        }
                    });
                } 
            }

            std::shared_ptr<boost::asio::steady_timer> heartbeatTimer_;
            boost::asio::ip::tcp::acceptor acceptor_;
            boost::asio::ip::tcp::socket socket_;
            std::vector<std::shared_ptr<boost::asio::ip::tcp::socket>> clients;
            boost::asio::streambuf readBuffer_;
            std::thread serverThread_;
            

    };
}
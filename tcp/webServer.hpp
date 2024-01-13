#pragma once

#include <crow.h>
#include <crow/routing.h>


namespace Server{

    typedef const crow::request& req;
    typedef crow::response& res;

    class Server : crow::SimpleApp{

        public:
            template <typename Func>
            void post(std::string pattern, Func callback){
                this->route_dynamic("").methods("POST"_method)        
                ([&](req req, res res){
                    callback(req, res);
                });
            }

            template <typename Func>
            void get(std::string pattern, Func callback) {
                this->route_dynamic(std::move(pattern)).methods("GET"_method)        
                ([&](const crow::request& req, crow::response& res){
                    callback(req, res);
                });
            }


            std::unique_ptr<std::thread> run(unsigned short port = 8080) {

                crow::logger::setLogLevel(crow::LogLevel::Critical);

                return std::make_unique<std::thread>([=](){
                    this->port(port).multithreaded().run();
                });
            }

    };
}


/*
int main() {

    Server::Server server;
    std::string f = "/index/";
    server.get(f, [](Server::req req, Server::res res){
        res.set_header("Content-Type", "text/plain");
        res.write("This is a GET request!\n");
        res.end();
    });

    std::unique_ptr<std::thread> t1 = server.run();

    t1->join();

    return 0;
}
*/

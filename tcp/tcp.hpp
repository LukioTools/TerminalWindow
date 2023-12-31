#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <csignal>
#include <iostream>
#include <vector>

//#include "./Camera.hpp"
#define log(smth) std::cout << smth << std::endl;

namespace tcp{
    class tcp_server
    {
        private:
        std::thread thread_obj; // Store the thread as a member variable.
        int sockfd;


        std::string convertToString(char* a, int size)
        {
            int i;
            std::string s = "";
            for (i = 0; i < size; i++) {
                s = s + a[i];
            }
            return s;
        }

        void error(const char *msg)
        {
            perror(msg);
            exit(1);
        }

        private: static void signalHandler(int signum, int sockfd) {
            if (signum == SIGINT) {
                // Handle Ctrl+C: Close the server and clean up
                close(sockfd);
                exit(signum);
            }
        }

        public: void Server(int portno)
        {
            log("starting server");
            int newsockfd;
            socklen_t clilen;
            char buffer[256];
            struct sockaddr_in serv_addr, cli_addr;
            int n;
            sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd < 0) 
                error("ERROR opening socket");

            bzero((char *) &serv_addr, sizeof(serv_addr));
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_addr.s_addr = INADDR_ANY;
            serv_addr.sin_port = htons(portno);
            if (bind(sockfd, (struct sockaddr *) &serv_addr,
                    sizeof(serv_addr)) < 0) 
                    error("ERROR on binding");
            
            int yes = 1;
            setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
            listen(sockfd,5);


            while (true)
            {
                clilen = sizeof(cli_addr);
                newsockfd = accept(sockfd, 
                            (struct sockaddr *) &cli_addr, 
                            &clilen);
                if (newsockfd < 0) 
                    error("ERROR on accept");
                bzero(buffer,256);
                n = read(newsockfd,buffer,255);
                if (n < 0) error("ERROR reading from socket");
        
                std::string unclean = std::string(buffer);
                
                log(unclean.size() << " " << std::string("pos").size())

                if (unclean == "pos")
                {
                    log("sending scene");
                    std::string m = "message";
                    log(m);
                    n = write(newsockfd,m.c_str(),m.size());
                }    

                if (n < 0) error("ERROR writing to socket");  
            }
            
            close(sockfd);

        }

        void Thread(int port){
            thread_obj = std::thread(&tcp::tcp_server::Server, this, port);
        }
            
    };
}

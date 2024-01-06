#pragma once

#include <asm-generic/errno-base.h>
#include <cstddef>
#include <cstring>
#include <iterator>
#include <memory>
#include <optional>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <type_traits>
#include <unistd.h>
#include <cerrno>
#include <stdexcept>
#include <vector>

namespace TCP {

struct Socket
{
    int fd = -1;
    
    void connect(const void* a, size_t l){
        auto r = ::connect(fd, reinterpret_cast<const sockaddr*>(a), l);
        if(r != 0)
            throw std::runtime_error(std::string("Socket::connect::Connection failed::") + strerror(errno));
    }

    void connect(const char* addr, unsigned short port, int domain = AF_INET){
        sockaddr_in serv_addr;
        serv_addr.sin_family = domain;
        serv_addr.sin_port = htons(port);
        if(inet_pton(domain, addr, &serv_addr.sin_addr) <= 0){
            throw std::runtime_error(std::string("Socket::connect:: Could not parse: ") + addr);
        }
        connect(&serv_addr, sizeof(serv_addr));
    }

    template<typename T>
    void send(const T& e){
        auto n = ::send(fd, &e, sizeof(T), 0);
        if(n != sizeof(T))
            throw std::runtime_error(std::string("Socket::send::failed to send:: ") + strerror(errno));
    }

    void send(const void* buffer, size_t size){
        auto n = ::send(fd, buffer, size, 0);
        if(n != size)
            throw std::runtime_error(std::string("Socket::send::failed to send:: ") + strerror(errno));
    }

    template<typename T>
    void recv(T* e){
        auto n = ::recv(fd, e, sizeof(T),0);
        if(n != sizeof(T))
            throw std::runtime_error(std::string("Socket::recv::failed to recv:: ") + strerror(errno));
    }

    void recv(void* buffer, size_t size){
        auto n = ::recv(fd, buffer, size, 0);
        if(n != size)
            throw std::runtime_error(std::string("Socket::recv::failed to recv:: ") + strerror(errno) );
    }
    void bind(unsigned short port, int domain = AF_INET, int in_addr = INADDR_ANY){
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);
        bind(&addr);
    }

    template<typename T>
    void bind(const T* a){
        auto r = ::bind(fd, reinterpret_cast<const struct sockaddr *>(a), sizeof(T));
        if(r)
            throw std::runtime_error(std::string("Socket::bind::failed to bind::") + strerror(errno));
    }

    void listen(int n){
        if(auto err = ::listen(fd, n))
            throw  std::runtime_error(std::string("Socket::listen::failed to listen::") + strerror(errno));
    }

    int accept(void* addr, socklen_t* addr_len){
        auto r = ::accept(fd, reinterpret_cast<struct sockaddr *__restrict>( addr), addr_len);
        if(r <= 0)
            throw std::runtime_error(std::string("Socket::accept::failed to accept::") + strerror(errno));
        return r;
    }
            //scuffed alert maby implement your own protocoll
        //put to -1 to no trunctuate or what ever
    Socket& sendmsg(const std::string& str, size_t max_size = 0x1fff){
        auto sz =  str.size() > max_size ? max_size : str.size();
        send(sz);
        send(str.c_str(), sz);
        return *this;
    }
            //scuffed alert maby implement your own protocoll
        //put to -1 to no trunctuate or what ever
    Socket& recvmsg(std::string& str, size_t max_size = 0x1fff){
        size_t imsg_sz = 0;
        recv(&imsg_sz);
        auto recv_size = imsg_sz > max_size ? max_size : imsg_sz;

        std::unique_ptr<char[]> buffer = std::make_unique<char[]>(recv_size);
        recv(buffer.get(), recv_size);
        str.append(buffer.get(), recv_size);
        return *this;
    }

    template<typename T, std::enable_if_t<!std::is_pointer_v<T>>>
    Socket& operator<<(const T& val){
        send(val);
        return *this;
    }

    Socket& operator<<(const char* ptr){
        send(ptr, strlen(ptr));
        return *this;
    }

    template<typename T, std::enable_if_t<!std::is_pointer_v<T>>>
    Socket& operator>>(T& val){
        recv(&val);
        return *this;
    }

    template<typename T = int>
    void setopt(int opt, std::optional<T*> val = std::nullopt){
        int yes = 1;        
        auto e = setsockopt(fd, SOL_SOCKET, opt, val.value_or(&yes), val ? sizeof(T) : sizeof(int));

        if(e != 0)
            throw std::runtime_error(std::string("Socket::setopt::setting socket option failed::") + strerror(errno));
    }


    
    
    operator bool(){
        return fd > 0;
    }

    operator int(){
        return fd;
    }


    void close(){
        if(this->operator bool())
            ::close(fd);
    }

    Socket() : fd(-1){};
    Socket(int fd) : fd(fd){};
};

#define MAX_BLOCK_SIZE (0x10000)
    //support for read untill
class BufferedSocket : public Socket
{
public:
    std::vector<char> buffer;

        //use strerror to get error message
    int read_block_to_buffer(){
        char buf[MAX_BLOCK_SIZE];
        auto read = ::recv(fd, buf, MAX_BLOCK_SIZE, MSG_DONTWAIT);
        if(read == -1){
            return errno;
        }
        buffer.insert(buffer.end(), buf, buf+read);
        return 0;
    }
        //use strerror to get error message
    BufferedSocket& read_until(std::string& str, char delim = '\0', size_t* error = nullptr){
        while (true)
        {
            for (size_t i = 0; i < buffer.size(); i++)
            {
                if(buffer[i] == delim){
                    //copy this part of the buffer 
                    auto it = buffer.begin();
                    std::advance(it, i+1);
                    str.append(buffer.begin(), it);

                    //remove the part of the buffer
                    buffer.erase(buffer.begin(), it);
                    return *this;
                }
            }
            //buffer ended, append it to the string and get moooor
            str.append(buffer.begin(), buffer.end());
            buffer.clear();

            auto err = read_block_to_buffer();
            if(err != 0 && err != EAGAIN && error){
                *error = err;
                return *this;
            }
        }
    }
    template<typename T>
    BufferedSocket& recv(T& e){
        while (buffer.size() < sizeof(T)) {
            read_block_to_buffer();
        }
        std::memcpy(&e, buffer.begin().base(), sizeof(T));
        return *this;
    }

    BufferedSocket& recv(void* e, size_t n){
        while (buffer.size() < n) {
            read_block_to_buffer();
        }
        std::memcpy(e, buffer.begin().base(), n);
        return *this;
    }

    template<typename T, std::enable_if_t<!std::is_pointer_v<T>>>
    inline BufferedSocket& operator>>(T& val){
        return recv(&val);
    }





    BufferedSocket() {}
    BufferedSocket(int s) : Socket(s) {}
    BufferedSocket(Socket s) : Socket(s) {}
    ~BufferedSocket() {}
};
}
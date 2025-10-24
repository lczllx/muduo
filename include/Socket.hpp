#ifndef MUDUO_SOCKET_H
#define MUDUO_SOCKET_H

#include <string>
#include <cstdint>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include "Logger.hpp"

#define MAX_LISTEN 1024

class Socket {
private:
    int _sockfd;
public:
    Socket();
    Socket(int fd);
    ~Socket();
    
    // 简单方法在头文件实现
    int Fd() { return _sockfd; }
    void Close() { if(_sockfd != -1) { close(_sockfd); _sockfd = -1; } }
    
    bool Create();
    bool Bind(const std::string &ip, uint16_t port);
    bool Listen(int backlog = MAX_LISTEN);
    bool Connect(const std::string &ip, uint16_t port);
    int Accept();
    ssize_t Recv(void *buf, size_t len, int flag = 0);
    ssize_t NonBlockRecv(void *buf, size_t len);
    ssize_t Send(const void *buf, size_t len, int flag = 0);
    ssize_t NonBlockSend(const void *buf, size_t len);
    bool CreateServer(uint16_t port, const std::string &ip = "0.0.0.0", bool block_flag = false);
    bool CreateClient(uint16_t port, const std::string &ip);
    void ReuseAdderess();
    void NoBlock();
};

#endif
#ifndef MUDUO_POLLER_H
#define MUDUO_POLLER_H

#include <unordered_map>
#include <vector>
#include <sys/epoll.h>
#include "Logger.hpp"

#define MAX_EPOLLEVENTS 1024

class Channel;

class Poller {
private:
    int _epfd;
    struct epoll_event _evs[MAX_EPOLLEVENTS];
    std::unordered_map<int, Channel*> _channels;
    
    void Update(Channel *channel, int op);
    bool HasChannel(Channel *channel);
    
public:
    Poller();
    void UpdateEvent(Channel *channel);
    void RemoveEvent(Channel *channel);
    void Poll(std::vector<Channel*>* active);
};

#endif
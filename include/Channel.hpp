#ifndef MUDUO_CHANNEL_H
#define MUDUO_CHANNEL_H

#include <functional>
#include <sys/epoll.h>
#include "CallbackTypes.hpp"

class EventLoop;
class Poller;

class Channel {
private:
    EventLoop* _loop;
    int _fd;
    uint32_t _events;
    uint32_t _revents;
    using EventCallback = std::function<void()>;
    EventCallback _read_cb;
    EventCallback _write_cb;
    EventCallback _error_cb;
    EventCallback _close_cb;
    EventCallback _event_cb;
    
public:
    Channel(EventLoop *loop, int fd);
    
    // 简单getter在头文件实现
    int Fd() const { return _fd; }
    uint32_t Events() { return _events; }
    void SetREvent(uint32_t events) { _revents = events; }
    
    void SetReadCallback(const EventCallback& cb) { _read_cb = cb; }
    void SetWriteCallback(const EventCallback& cb) { _write_cb = cb; }
    void SetErrorCallback(const EventCallback& cb) { _error_cb = cb; }
    void SetCloseCallback(const EventCallback& cb) { _close_cb = cb; }
    void SetEventCallback(const EventCallback& cb) { _event_cb = cb; }
    
    bool ReadAble() const { return _events & EPOLLIN; }
    bool WriteAble() const { return _events & EPOLLOUT; }
    
    void EnableRead();
    void EnableWrite();
    void DisableRead();
    void DisableWrite();
    void DisableAll();
    void Remove();
    void Update();
    void HandleEvent();
};

#endif
#ifndef MUDUO_CHANNEL_H
#define MUDUO_CHANNEL_H

#include <functional>
#include <sys/epoll.h>
#include "CallbackTypes.hpp"

class EventLoop;
class Poller;
/*对一个描述符要进行的io事件管理的模块
让对于描述符的监控事件在用户态更容易维护，以及触发事件后的操作流程更加清晰*/
class Channel {
private:
    EventLoop* _loop;
    int _fd;
    uint32_t _events;//当前需要监控的事件
    uint32_t _revents;//当前连接触发的事件
    using EventCallback = std::function<void()>;
    EventCallback _read_cb;//可读回调
    EventCallback _write_cb;//可写回调
    EventCallback _error_cb;//错误回调
    EventCallback _close_cb;//连接断开回调
    EventCallback _event_cb;//任意事件回调
    
public:
    Channel(EventLoop *loop, int fd);
    
    // 简单getter在头文件实现
    int Fd() const { return _fd; }//获取描述符fd
    uint32_t Events() { return _events; }//获取当前事件
    void SetREvent(uint32_t events) { _revents = events; }//设置当前触发事件
    
    void SetReadCallback(const EventCallback& cb) { _read_cb = cb; }
    void SetWriteCallback(const EventCallback& cb) { _write_cb = cb; }
    void SetErrorCallback(const EventCallback& cb) { _error_cb = cb; }
    void SetCloseCallback(const EventCallback& cb) { _close_cb = cb; }
    void SetEventCallback(const EventCallback& cb) { _event_cb = cb; }
    
   
    bool ReadAble() const { return _events & EPOLLIN; } //描述符是否可读
    bool WriteAble() const { return _events & EPOLLOUT; } //描述符是否可写
    
    void EnableRead();//对描述符监控可读 -移到eventloop上面进行监控
    void EnableWrite();//对描述符监控可写
    void DisableRead();//解除可读事件监控
    void DisableWrite();//解除可写事件监控
    void DisableAll();//解除所有事件监控
    void Remove();//移除监控
    void Update();//更新监控
    void HandleEvent();//事件处理->revents确定触发的事件->调用对应回调
};

#endif
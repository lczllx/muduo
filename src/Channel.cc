#include "../include/Channel.hpp"
#include "../include/EventLoop.hpp"
#include "../include/Logger.hpp"

Channel::Channel(EventLoop *loop, int fd) 
    : _loop(loop), _fd(fd), _events(0), _revents(0) {}

void Channel::EnableRead() { 
    _events |= EPOLLIN; 
    Update(); 
}

void Channel::EnableWrite() { 
    _events |= EPOLLOUT; 
    Update(); 
}

void Channel::DisableRead() { 
    _events &= ~EPOLLIN; 
    Update(); 
}

void Channel::DisableWrite() { 
    _events &= ~EPOLLOUT; 
    Update(); 
}

void Channel::DisableAll() { 
    _events = 0; 
    Update(); 
}

void Channel::Remove() { 
    _loop->RemoveEvent(this); 
}

void Channel::Update() { 
    _loop->UpdateEvent(this); 
}

void Channel::HandleEvent() {
    // 错误事件优先处理
    if (_revents & EPOLLERR)
    {
        if (_error_cb) _error_cb();
        // 通常错误后需要关闭连接，但这里是否立即返回取决于业务
        // 如果错误后连接已经不可用，那么应该返回，不再处理其他事件
        return;
    }
    // 对端关闭连接（或者半关闭）且没有数据可读，通常表示连接已经关闭
    // 注意：EPOLLRDHUP 需要显式在 epoll 事件中注册，且不是所有系统都支持
    if (_revents & EPOLLRDHUP)
    {
        if (_close_cb) _close_cb();
        return;
    }
    // 挂起事件，表示连接已经挂起，无法再进行读写
    if (_revents & EPOLLHUP)
    {
        if (_close_cb) _close_cb();
        return;
    }
    // 读事件（包括普通数据和带外数据）
    if ((_revents & EPOLLIN) || (_revents & EPOLLPRI))
    {
        if (_read_cb) _read_cb();
    }
    // 写事件
    if (_revents & EPOLLOUT)
    {
        if (_write_cb) _write_cb();
    }
    // 任意事件回调
    if (_event_cb) _event_cb();
}
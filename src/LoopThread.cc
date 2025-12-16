#include "../include/LoopThread.hpp"

LoopThread::LoopThread() : _loop(nullptr), _thread(std::thread(&LoopThread::ThreadEntry, this)) {}

void LoopThread::ThreadEntry() {
    EventLoop loop;
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _loop = &loop;
        _cond.notify_all();//loop实例化完唤醒可能的阻塞
    }
    loop.Start();//启动eventloop
}

EventLoop* LoopThread::Getloop() {
    EventLoop* loop = nullptr;
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _cond.wait(lock, [&](){ return _loop != nullptr; });//阻塞至loop实例化完成
        loop = _loop;
    }
    return loop;
}
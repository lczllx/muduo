#include "../include/LoopThread.hpp"

LoopThread::LoopThread() : _loop(nullptr), _thread(std::thread(&LoopThread::ThreadEntry, this)) {}

void LoopThread::ThreadEntry() {
    EventLoop loop;
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _loop = &loop;
        _cond.notify_all();
    }
    loop.Start();
}

EventLoop* LoopThread::Getloop() {
    EventLoop* loop = nullptr;
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _cond.wait(lock, [&](){ return _loop != nullptr; });
        loop = _loop;
    }
    return loop;
}
#ifndef MUDUO_LOOPTHREAD_H
#define MUDUO_LOOPTHREAD_H

#include "EventLoop.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>

class LoopThread {
private:
    std::mutex _mutex;
    std::condition_variable _cond;
    EventLoop* _loop;
    std::thread _thread;

    void ThreadEntry();

public:
    LoopThread();
    EventLoop* Getloop();
};

#endif
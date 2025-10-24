#ifndef MUDUO_LOOPTHREADPOOL_H
#define MUDUO_LOOPTHREADPOOL_H

#include "EventLoop.hpp"
#include "LoopThread.hpp"
#include <vector>
#include <memory>

class LoopThreadPool {
private:
    int _thread_cnt;
    int _next_loop_idx;
    EventLoop* _base_loop;
    std::vector<EventLoop*> _loop;
    std::vector<std::unique_ptr<LoopThread>> _threads;

public:
    LoopThreadPool(EventLoop* base_loop);
    void SetThreadCnt(int cnt);
    void Create();
    EventLoop* NextLoop();
};

#endif
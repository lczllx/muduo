#ifndef MUDUO_LOOPTHREAD_H
#define MUDUO_LOOPTHREAD_H

#include "EventLoop.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>

//eventloop和线程整合的模块
class LoopThread {
private:
    std::mutex _mutex;
    std::condition_variable _cond;//条件变量 结合互斥锁实现loop获取同步关系

    EventLoop* _loop;//在线程内实例化eventloop指针，ThreadEntry 返回前置 nullptr
    std::thread _thread;//eventloop对应线程

    void ThreadEntry();//实例化eventloop对象并启动eventloop

public:
    LoopThread();
    ~LoopThread();
    EventLoop* Getloop();
    // muduo::net::EventLoopThread::startLoop() 兼容接口：
    // 返回该线程绑定的 EventLoop（阻塞直到就绪）。
    // 注：LoopThread 构造即启动线程（非 muduo 的惰性 start），语义等价，仅启动时机更早。
    EventLoop* startLoop() { return Getloop(); }
};

// 兼容 muduo::net::EventLoopThread 命名，迁移 lyqtRpc 等外部工程时可直接替换类型名
using EventLoopThread = LoopThread;

#endif
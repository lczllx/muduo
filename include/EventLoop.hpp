#ifndef MUDUO_EVENTLOOP_H
#define MUDUO_EVENTLOOP_H

#include <thread>
#include <mutex>
#include <vector>
#include <functional>
#include <memory>
#include <condition_variable>
#include <sys/eventfd.h>
#include <unistd.h>
#include <cassert>

#include "Logger.hpp"
#include "Poller.hpp"
#include "Timer.hpp"

/*一个模块对应一个线程，对于服务器的任意事件都是EventLoop模块完成*/
class Channel;
class LoopThread;
class EventLoop {
private:
    std::thread::id _thread_id;//线程id
    int _eventfd;//事件通知描述符-唤醒io事件监控中有可能的阻塞
    Poller _poller;
    std::unique_ptr<Channel> _event_channel;//eventfd的channel
    using Tasks = std::function<void()>;
    std::vector<Tasks> _task;//任务队列
    std::mutex _mutex;
    TimingWheel _timerwheel;

    void RunAllTask();//运行任务池所有任务
    static int CreateEventfd();//创建eventfd
    void ReadEventfd();//读取eventfd
    void WeakupEventfd();//唤醒epoll_wait可能因为没有事件就绪而造成的阻塞

public:
    EventLoop();
    void Start();//启动eventloop
    void RunInLoop(const Tasks& t);//判断要执行的任务是不是在当前线程，不是就压入任务池，是就执行
    void TasksInLoop(const Tasks& t);//将操作加入任务池
    bool IsInLoop();//判断当前线程是不是在Eventloop所在线程里面
    void UpdateEvent(Channel* channel);//更新描述符的事件监控
    void RemoveEvent(Channel* channel);//移除描述符的事件监控
    void TimerAdd(uint64_t id, uint32_t delay, const TaskFunc& cb);//添加定时器
    void TimerReflesh(uint64_t id);//刷新定时器
    void TimerCancel(uint64_t id);//取消定时
    bool HasTimer(uint64_t id);//是否有这个定时器
    void AssertInLoop();//断言在eventloop线程里面
};

#endif
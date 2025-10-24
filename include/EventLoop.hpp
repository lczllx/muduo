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

class Channel;
class LoopThread;

class EventLoop {
private:
    std::thread::id _thread_id;
    int _eventfd;
    Poller _poller;
    std::unique_ptr<Channel> _event_channel;
    using Tasks = std::function<void()>;
    std::vector<Tasks> _task;
    std::mutex _mutex;
    TimingWheel _timerwheel;

    void RunAllTask();
    static int CreateEventfd();
    void ReadEventfd();
    void WeakupEventfd();

public:
    EventLoop();
    void Start();
    void RunInLoop(const Tasks& t);
    void TasksInLoop(const Tasks& t);
    bool IsInLoop();
    void UpdateEvent(Channel* channel);
    void RemoveEvent(Channel* channel);
    void TimerAdd(uint64_t id, uint32_t delay, const TaskFunc& cb);
    void TimerReflesh(uint64_t id);
    void TimerCancel(uint64_t id);
    bool HasTimer(uint64_t id);
    void AssertInLoop();
};

#endif
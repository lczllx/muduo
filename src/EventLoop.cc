#include "../include/EventLoop.hpp"
#include "../include/Channel.hpp"
#include <cstring>
#include <cerrno>

EventLoop::EventLoop() : 
    _thread_id(std::this_thread::get_id()),
    _eventfd(CreateEventfd()),
    _event_channel(new Channel(this, _eventfd)),
    _timerwheel(this) {

    _event_channel->SetReadCallback(std::bind(&EventLoop::ReadEventfd, this));
    _event_channel->EnableRead();
}

void EventLoop::Start() {
    while(true) {
        std::vector<Channel*> actives;
        _poller.Poll(&actives);
        for(auto &e : actives) {
            e->HandleEvent();
        }
        RunAllTask();
    }
}

void EventLoop::RunAllTask() {
    std::vector<Tasks> tmp;
    {
        std::unique_lock<std::mutex> _lock(_mutex);
        _task.swap(tmp);
    }
    for(auto &task : tmp) {
        task();
    }
}

int EventLoop::CreateEventfd() {
    int efd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    if(efd < 0) {
        L_ERROR << "create eventfd failed";
        abort();
    }
    return efd;
}

void EventLoop::ReadEventfd() {
    uint64_t res = 0;
    int ret = read(_eventfd, &res, sizeof(res));
    if(ret < 0) {
        if(errno == EINTR || errno == EAGAIN) return;
        L_ERROR << "read eventfd failed";
        abort();
    }
}

void EventLoop::WeakupEventfd() {
    uint64_t val = 1;
    int ret = write(_eventfd, &val, sizeof(val));
    if(ret < 0) {
        if(errno == EINTR) return;
        L_ERROR << "write eventfd failed";
        abort();
    }
}

void EventLoop::RunInLoop(const Tasks& t) {
    if(IsInLoop()) return t();
    return TasksInLoop(t);
}

void EventLoop::TasksInLoop(const Tasks& t) {
    {
        std::unique_lock<std::mutex> _lock(_mutex);
        _task.push_back(t);
    }
    WeakupEventfd();
}

bool EventLoop::IsInLoop() {
    return (_thread_id == std::this_thread::get_id());
}

void EventLoop::UpdateEvent(Channel* channel) {
    _poller.UpdateEvent(channel);
}

void EventLoop::RemoveEvent(Channel* channel) {
    _poller.RemoveEvent(channel);
}

void EventLoop::TimerAdd(uint64_t id, uint32_t delay, const TaskFunc& cb) {
    _timerwheel.TimerAdd(id, delay, cb);
}

void EventLoop::TimerReflesh(uint64_t id) {
    _timerwheel.TimerReflesh(id);
}

void EventLoop::TimerCancel(uint64_t id) {
    _timerwheel.TimerCancel(id);
}

bool EventLoop::HasTimer(uint64_t id) {
    return _timerwheel.HasTimer(id);
}

void EventLoop::AssertInLoop() {
    assert(_thread_id == std::this_thread::get_id());
}
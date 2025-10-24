#include "../include/Timer.hpp"
#include "../include/EventLoop.hpp"

// TimingWheel 的公共接口实现
void TimingWheel::TimerAdd(uint64_t id, uint32_t delay, const TaskFunc& cb) {
    _loop->RunInLoop(std::bind(&TimingWheel::TimerAddInLoop, this, id, delay, cb));
}

void TimingWheel::TimerReflesh(uint64_t id) {
    _loop->RunInLoop(std::bind(&TimingWheel::TimerRefleshInLoop, this, id));
}

void TimingWheel::TimerCancel(uint64_t id) {
    _loop->RunInLoop(std::bind(&TimingWheel::TimerCancelInLoop, this, id));
}

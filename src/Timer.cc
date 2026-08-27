#include "../include/Timer.hpp"
#include "../include/EventLoop.hpp"
#include <cstring>
#include <cerrno>
#include <limits>

TimerQueue::TimerQueue(EventLoop *loop)
    : _loop(loop),
      _timerfd(createTimerfd()),
      _timer_channel(new Channel(loop, _timerfd)),
      _next_id(1ULL << 32),  // 自动 id 从大值开始，避开连接 id 等显式 id
      _calling_expired(false)
{
    _timer_channel->SetReadCallback(std::bind(&TimerQueue::handleRead, this));
    _timer_channel->EnableRead();
}

TimerQueue::~TimerQueue()
{
    _timer_channel->Remove();
    ::close(_timerfd);
}

int TimerQueue::createTimerfd()
{
    int fd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (fd < 0)
    {
        DLMUDUO_ERROR("timerfd_create failed");
        abort();
    }
    return fd;
}

void TimerQueue::readTimerfd()
{
    uint64_t howmany;
    ssize_t n = ::read(_timerfd, &howmany, sizeof(howmany));
    if (n != sizeof(howmany))
    {
        if (errno == EAGAIN || errno == EINTR)
            return;
        DLMUDUO_ERROR("read timerfd failed");
    }
}

// 重设 timerfd 的到期时间（相对时间，纳秒精度）
void TimerQueue::resetTimerfd(std::chrono::steady_clock::time_point expiration)
{
    struct itimerspec new_value;
    std::memset(&new_value, 0, sizeof(new_value));

    auto diff = expiration - std::chrono::steady_clock::now();
    if (diff < std::chrono::steady_clock::duration::zero())
        diff = std::chrono::steady_clock::duration::zero();

    auto sec = std::chrono::duration_cast<std::chrono::seconds>(diff);
    auto nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(diff - sec);
    new_value.it_value.tv_sec = static_cast<time_t>(sec.count());
    new_value.it_value.tv_nsec = static_cast<long>(nsec.count());
    ::timerfd_settime(_timerfd, 0, &new_value, nullptr);
}

// ---- 公开接口：跨线程安全，路由到 EventLoop 线程 ----
TimerId TimerQueue::addTimer(const TimerCallback &cb, double seconds)
{
    TimerId id = _next_id++;
    _loop->RunInLoop(std::bind(&TimerQueue::addTimerInLoop, this, cb, seconds, false, id));
    return id;
}

TimerId TimerQueue::addRepeatTimer(const TimerCallback &cb, double seconds)
{
    TimerId id = _next_id++;
    _loop->RunInLoop(std::bind(&TimerQueue::addTimerInLoop, this, cb, seconds, true, id));
    return id;
}

void TimerQueue::addTimerWithId(TimerId id, const TimerCallback &cb, double seconds)
{
    _loop->RunInLoop(std::bind(&TimerQueue::addTimerInLoop, this, cb, seconds, false, id));
}

void TimerQueue::cancel(TimerId id)
{
    _loop->RunInLoop(std::bind(&TimerQueue::cancelInLoop, this, id));
}

void TimerQueue::refresh(TimerId id)
{
    _loop->RunInLoop(std::bind(&TimerQueue::refreshInLoop, this, id));
}

bool TimerQueue::hasTimer(TimerId id)
{
    return _timers.find(id) != _timers.end();
}

// ---- 内部：在 EventLoop 线程执行 ----
void TimerQueue::addTimerInLoop(const TimerCallback &cb, double seconds, bool repeat, TimerId id)
{
    TimerPtr timer = std::make_shared<Timer>();
    timer->id = id;
    timer->delay = seconds;
    timer->repeat = repeat;
    timer->cb = cb;
    timer->expiration = std::chrono::steady_clock::now()
                        + std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<double>(seconds));

    // 若已是重复 id，先移除旧条目避免重复
    auto old = _timers.find(id);
    if (old != _timers.end())
    {
        _entries.erase({old->second->expiration, id});
        _timers.erase(old);
    }

    bool earliest = _entries.empty() || timer->expiration < _entries.begin()->expiration;
    _timers[id] = timer;
    _entries.insert({timer->expiration, id});
    if (earliest)
        resetTimerfd(timer->expiration);
}

void TimerQueue::cancelInLoop(TimerId id)
{
    auto it = _timers.find(id);
    if (it == _timers.end())
    {
        // 到期回调执行期间：定时器已从 _timers 摘除但尚未 reset 重挂，
        // 记录到 _canceling_timers，reset 时跳过重挂
        if (_calling_expired)
            _canceling_timers.insert(id);
        return;
    }
    _entries.erase({it->second->expiration, id});
    _timers.erase(it);
}

void TimerQueue::refreshInLoop(TimerId id)
{
    auto it = _timers.find(id);
    if (it == _timers.end())
        return;
    TimerPtr timer = it->second;
    _entries.erase({timer->expiration, id});
    timer->restart(std::chrono::steady_clock::now());
    _entries.insert({timer->expiration, id});
    // 刷新只会把到期时间后移，但若被刷新的是最早定时器，需重设 timerfd 到新的最早值
    resetTimerfd(_entries.begin()->expiration);
}

std::vector<TimerQueue::TimerPtr> TimerQueue::getExpired(std::chrono::steady_clock::time_point now)
{
    std::vector<TimerPtr> expired;
    Entry sentry{now, std::numeric_limits<TimerId>::max()};
    auto end = _entries.lower_bound(sentry);
    for (auto it = _entries.begin(); it != end; ++it)
    {
        auto tit = _timers.find(it->id);
        if (tit != _timers.end())
        {
            expired.push_back(tit->second);
            _timers.erase(tit);
        }
    }
    _entries.erase(_entries.begin(), end);
    return expired;
}

void TimerQueue::handleRead()
{
    auto now = std::chrono::steady_clock::now();
    readTimerfd();
    std::vector<TimerPtr> expired = getExpired(now);

    _calling_expired = true;
    _canceling_timers.clear();
    for (const auto &timer : expired)
    {
        timer->run();
    }
    _calling_expired = false;

    reset(expired, now);
}

void TimerQueue::reset(const std::vector<TimerPtr> &expired, std::chrono::steady_clock::time_point now)
{
    for (const auto &timer : expired)
    {
        // 回调执行期间被 cancel 的周期任务不重挂
        if (_canceling_timers.count(timer->id) > 0)
            continue;
        if (timer->repeat)
        {
            timer->restart(now);
            _timers[timer->id] = timer;
            _entries.insert({timer->expiration, timer->id});
        }
    }
    if (!_entries.empty())
        resetTimerfd(_entries.begin()->expiration);
}

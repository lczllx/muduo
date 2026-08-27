#ifndef MUDUO_TIMER_H
#define MUDUO_TIMER_H

#include <memory>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <functional>
#include <chrono>
#include <sys/timerfd.h>
#include <unistd.h>
#include "log_system/lcz_log.h"
#include "CallbackTypes.hpp"
#include "Channel.hpp"

/*定时器队列：std::set 按到期时间排序 + 单个 timerfd 驱动
  相比旧时间轮的优势：
  1. 毫秒级精度（timerfd 纳秒精度，支持亚秒超时）
  2. 任意延迟（不再受 360 槽 × 1 秒上限约束）
  3. 自动分配定时器 id（runAfter/runEvery 返回 TimerId）
  4. 支持周期任务（repeat）
  线程安全：所有公开接口通过 RunInLoop 路由到 EventLoop 线程*/
class EventLoop;
class TimerQueue
{
public:
    using TimerCallback = std::function<void()>;

    TimerQueue(EventLoop *loop);
    ~TimerQueue();

    // 新增一次性定时器，自动分配 id，seconds 秒后触发
    TimerId addTimer(const TimerCallback &cb, double seconds);
    // 新增周期定时器，每 seconds 秒触发一次，直到 cancel
    TimerId addRepeatTimer(const TimerCallback &cb, double seconds);
    // 显式指定 id 的定时器（兼容旧 TimerAdd 语义）
    void addTimerWithId(TimerId id, const TimerCallback &cb, double seconds);
    // 取消定时器（no-op 若不存在）
    void cancel(TimerId id);
    // 刷新定时器：把到期时间重置为 now + delay（活跃度刷新用）
    void refresh(TimerId id);
    // 判断定时器是否存在
    bool hasTimer(TimerId id);

private:
    struct Timer
    {
        TimerId id;
        double delay;   // 到期延迟（秒），refresh 时复用
        bool repeat;    // 是否周期任务
        TimerCallback cb;
        std::chrono::steady_clock::time_point expiration;

        void run() const
        {
            if (cb)
                cb();
        }
        void restart(std::chrono::steady_clock::time_point now)
        {
            expiration = now + std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<double>(delay));
        }
    };
    using TimerPtr = std::shared_ptr<Timer>;

    // set 排序键：(到期时间, id)，保证同一到期时间下按 id 稳定排序
    struct Entry
    {
        std::chrono::steady_clock::time_point expiration;
        TimerId id;
        bool operator<(const Entry &rhs) const
        {
            return expiration < rhs.expiration || (expiration == rhs.expiration && id < rhs.id);
        }
    };

    void handleRead();  // timerfd 可读 → 触发到期任务
    void readTimerfd();
    static int createTimerfd();
    void resetTimerfd(std::chrono::steady_clock::time_point expiration);

    void addTimerInLoop(const TimerCallback &cb, double seconds, bool repeat, TimerId id);
    void cancelInLoop(TimerId id);
    void refreshInLoop(TimerId id);

    std::vector<TimerPtr> getExpired(std::chrono::steady_clock::time_point now);
    void reset(const std::vector<TimerPtr> &expired, std::chrono::steady_clock::time_point now);

private:
    EventLoop *_loop;
    int _timerfd;
    std::unique_ptr<Channel> _timer_channel;
    std::set<Entry> _entries;                     // 按到期时间排序的定时器索引
    std::unordered_map<TimerId, TimerPtr> _timers; // id → 定时器对象

    TimerId _next_id;               // 自动分配 id 计数器（从大值开始，避开连接 id 等显式 id）
    bool _calling_expired;          // 是否正在执行到期回调（处理回调内 cancel 的重入）
    std::unordered_set<TimerId> _canceling_timers; // 回调执行期间被 cancel 的 id 集合
};

#endif

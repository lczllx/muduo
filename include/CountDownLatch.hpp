#ifndef MUDUO_COUNTDOWNLATCH_H
#define MUDUO_COUNTDOWNLATCH_H

#include <mutex>
#include <condition_variable>

/*倒计时门闩：主线程等待 IO 线程完成初始化（如同步连接建立）时使用
  wait() 阻塞直到计数归零，countDown() 递减并唤醒等待线程*/
class CountDownLatch
{
public:
    explicit CountDownLatch(int count) : _count(count) {}
    CountDownLatch(const CountDownLatch &) = delete;
    CountDownLatch &operator=(const CountDownLatch &) = delete;

    void wait()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _cond.wait(lock, [this] { return _count <= 0; });
    }

    void countDown()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        --_count;
        if (_count <= 0)
            _cond.notify_all();
    }

    int getCount() const
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _count;
    }

private:
    mutable std::mutex _mutex;
    std::condition_variable _cond;
    int _count;
};

#endif

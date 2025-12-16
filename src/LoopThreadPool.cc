#include "../include/LoopThreadPool.hpp"

LoopThreadPool::LoopThreadPool(EventLoop* base_loop)
    : _thread_cnt(0), _next_loop_idx(0), _base_loop(base_loop) {}

//设置从属线程数量
void LoopThreadPool::SetThreadCnt(int cnt) {
    _thread_cnt = cnt;
}

void LoopThreadPool::Create() {
    //创建从属线程
    _threads.resize(_thread_cnt);
    //创建从属线程的eventloop
    _loop.resize(_thread_cnt);
    for(int i = 0; i < _thread_cnt; i++) {
        _threads[i] = std::unique_ptr<LoopThread>(new LoopThread());//创建从属线程
        _loop[i] = _threads[i]->Getloop();//获取从属线程的eventloop
    }
}
//获取下一个从属线程的eventloop
EventLoop* LoopThreadPool::NextLoop() {
    if(_thread_cnt == 0) return _base_loop;
    _next_loop_idx = (_next_loop_idx + 1) % _thread_cnt;
    return _loop[_next_loop_idx];
}
#ifndef MUDUO_ACCEPTOR_H
#define MUDUO_ACCEPTOR_H

#include "Socket.hpp"
#include "Channel.hpp"
#include <functional>

class EventLoop;

class Acceptor {
private:
    Socket _socket;
    EventLoop* _loop;
    Channel _acpt_channel;

    using AcceptorFunc = std::function<void(int)>;
    AcceptorFunc _acpt_cb;

    void HandleRead();
    int CreateServer(int port);

public:
    Acceptor(EventLoop* loop, int port);
    void SetAcceptorCallBack(const AcceptorFunc& acpt_cb);
    void Listen();
};

#endif
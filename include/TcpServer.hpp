#ifndef MUDUO_TCPSERVER_H
#define MUDUO_TCPSERVER_H

#include "Acceptor.hpp"
#include "LoopThreadPool.hpp"
#include "Connection.hpp"
#include "EventLoop.hpp"
#include <unordered_map>

class TcpServer {
private:
    uint64_t _next_id;
    int _port;
    int _timeout;
    bool _enable_inactive_release;
    EventLoop _base_loop;
    Acceptor _acceptor;
    LoopThreadPool _pool;
    std::unordered_map<uint64_t, PtrConnection> _connections;

    ConnectedCallBack _connected_cb;
    ClosedCallBack _closed_cb;
    MessageCallBack _message_cb;
    AnyEventCallBack _event_cb;
    ClosedCallBack _server_closed_cb;

    void RemoveConnection(const PtrConnection& conne);
    void RemoveConnectionInLoop(const PtrConnection& conne);
    void NewConnection(int fd);
    void RunAfterInLoop(const std::function<void()>& task, int delay);

public:
    TcpServer(int port);
    void SetThreadCnt(int cnt);
    void SetConnectedCallBack(const ConnectedCallBack& connected_cb);
    void SetClosedCallBack(const ClosedCallBack& closed_cb);
    void SetMessageCallBack(const MessageCallBack& message_cb);
    void SetAnyEventCallBack(const AnyEventCallBack& event_cb);
    void SetServerClosedCallBack(const ClosedCallBack& cb);
    void EnableInactiveRelease(int timeout);
    void RunAfter(const std::function<void()>& task, int delay);
    void Start();
};

#endif
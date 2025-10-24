#ifndef MUDUO_CONNECTION_H
#define MUDUO_CONNECTION_H

#include "Any.hpp"
#include "Buffer.hpp"
#include "Socket.hpp"
#include "Channel.hpp"
#include "CallbackTypes.hpp"
#include <memory>

class EventLoop;

class Connection : public std::enable_shared_from_this<Connection> {
private:
    uint64_t _conne_id;
    uint64_t _timer_id;
    int _sockfd;
    bool _enable_inactive_release;
    EventLoop* _loop;
    ConneStatus _status;
    Socket _socket;
    Channel _conne_channel;
    Buffer _in_buffer;
    Buffer _out_buffer;
    Any _context;

    ConnectedCallBack _connected_cb;
    ClosedCallBack _closed_cb;
    MessageCallBack _message_cb;
    AnyEventCallBack _event_cb;
    ClosedCallBack _server_closed_cb;

    void HandleRead();
    void HandleWrite();
    void HandleClose();
    void HandleError();
    void HandleEvent();
    void EstablishedInLoop();
    void ReleaseInLoop();
    void ShutdownInLoop();
    void SendInLoop(Buffer buf);
    void EnableInactiveReleaseInLoop(int sec);
    void CancelInactiveReleaseInLoop();
    void UpgradeInLoop(const Any& context, const ConnectedCallBack& connected_cb, const ClosedCallBack& closed_cb, const MessageCallBack& message_cb, const AnyEventCallBack& event_cb);

public:
    Connection(EventLoop* loop, uint64_t cone_id, int sockfd);
    ~Connection();

    int Fd() const { return _sockfd; }
    int Id() const { return _conne_id; }
    bool Connected() const { return _status == CONNECTED; }

    void SetContext(const Any& context) { _context = context; }
    Any* GetContext() { return &_context; }

    void SetConnectedCallBack(const ConnectedCallBack& connected_cb) { _connected_cb = connected_cb; }
    void SetClosedCallBack(const ClosedCallBack& closed_cb) { _closed_cb = closed_cb; }
    void SetMessageCallBack(const MessageCallBack& message_cb) { _message_cb = message_cb; }
    void SetAnyEventCallBack(const AnyEventCallBack& event_cb) { _event_cb = event_cb; }
    void SetServerClosedCallBack(const ClosedCallBack& cb) { _server_closed_cb = cb; }

    void Established();
    void Send(const char* data, size_t len);
    void EnableInactiveRelease(int sec);
    void CancelInactiveRelease();
    void Shutdown();
    void Release();
    void Upgrade(const Any& context, const ConnectedCallBack& connected_cb, const ClosedCallBack& closed_cb, const MessageCallBack& message_cb, const AnyEventCallBack& event_cb);
};

#endif
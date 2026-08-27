#include "../include/TcpClient.hpp"
#include "../include/EventLoop.hpp"
#include "log_system/lcz_log.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <cerrno>
#include <cstring>

TcpClient::TcpClient(EventLoop *loop, const InetAddress &serverAddr, const std::string &name)
    : _loop(loop),
      _server_addr(serverAddr),
      _name(name),
      _state(kDisconnected),
      _next_conn_id(0)
{
}

TcpClient::~TcpClient()
{
    // 连接/channel 的清理依赖 loop 线程；调用方需保证 loop 生命周期长于 TcpClient，
    // 并在析构前调用 stop()/disconnect() 完成优雅断开。
}

void TcpClient::SetConnectionCallback(const ConnectedCallBack &cb)
{
    _connection_cb = cb;
}

void TcpClient::SetMessageCallback(const MessageCallBack &cb)
{
    _message_cb = cb;
}

void TcpClient::connect()
{
    _loop->RunInLoop(std::bind(&TcpClient::connectInLoop, this));
}

void TcpClient::disconnect()
{
    _loop->RunInLoop([this]() {
        if (_connection)
            _connection->Shutdown();
    });
}

void TcpClient::stop()
{
    _loop->RunInLoop([this]() {
        if (_connection)
            _connection->Shutdown();
        if (_channel)
        {
            _channel->Remove();
            _channel.reset();
        }
        _state = kDisconnected;
    });
}

void TcpClient::connectInLoop()
{
    // 已连接或正在连接，直接返回
    if (_state == kConnecting || _state == kConnected)
        return;
    _state = kConnecting;

    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (sockfd < 0)
    {
        DLMUDUO_ERROR("%s socket create failed: %s", _name.c_str(), strerror(errno));
        _state = kDisconnected;
        return;
    }

    int ret = ::connect(sockfd, reinterpret_cast<const struct sockaddr *>(&_server_addr.getSockAddr()),
                        sizeof(struct sockaddr_in));
    int savedErrno = (ret == 0) ? 0 : errno;
    switch (savedErrno)
    {
    case 0:           // 本地连接可能立即成功
    case EINPROGRESS: // 非阻塞 connect 正常进行中
    case EINTR:
    case EISCONN:
        connecting(sockfd);
        break;
    default:
        DLMUDUO_ERROR("%s connect failed: %s", _name.c_str(), strerror(savedErrno));
        ::close(sockfd);
        _state = kDisconnected;
        break;
    }
}

void TcpClient::connecting(int sockfd)
{
    _state = kConnecting;
    _channel.reset(new Channel(_loop, sockfd));
    _channel->SetWriteCallback(std::bind(&TcpClient::handleWrite, this));
    _channel->SetErrorCallback(std::bind(&TcpClient::handleError, this));
    _channel->SetCloseCallback(std::bind(&TcpClient::handleError, this));
    _channel->EnableWrite(); // 非阻塞 connect 完成后 socket 变为可写
}

void TcpClient::handleWrite()
{
    int sockfd = _channel->Fd();
    int err = 0;
    socklen_t len = sizeof(err);
    if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &err, &len) < 0)
    {
        err = errno;
    }

    if (err != 0)
    {
        DLMUDUO_ERROR("%s connect failed (SO_ERROR): %s", _name.c_str(), strerror(err));
        _channel->Remove();
        _channel.reset();
        ::close(sockfd);
        _state = kDisconnected;
    }
    else
    {
        newConnection(sockfd);
    }
}

void TcpClient::handleError()
{
    if (_state != kConnecting)
        return;
    int sockfd = _channel->Fd();
    _channel->Remove();
    _channel.reset();
    ::close(sockfd);
    _state = kDisconnected;
    DLMUDUO_ERROR("%s connect error", _name.c_str());
}

void TcpClient::newConnection(int sockfd)
{
    // 先移除 connect 阶段的临时 channel，释放 fd 在 Poller 中的占用，
    // 交由 Connection 内部重新注册自己的 channel（Poller 以 fd 为 key，不能同时存在两个 channel）
    _channel->Remove();
    _channel.reset();
    _state = kConnected;

    PtrConnection conn(new Connection(_loop, _next_conn_id++, sockfd));
    conn->SetMessageCallBack(_message_cb);
    conn->SetConnectedCallBack(_connection_cb);  // 连接建立 → 用户回调
    conn->SetClosedCallBack(_connection_cb);     // 连接断开 → 用户回调
    conn->SetServerClosedCallBack(std::bind(&TcpClient::removeConnection, this, std::placeholders::_1));
    _connection = conn;

    DLMUDUO_DEBUG("%s connected, fd=%d", _name.c_str(), sockfd);
    conn->Established(); // 切换 CONNECTED、启动读监控、触发 _connected_cb
}

void TcpClient::removeConnection(const PtrConnection &conn)
{
    if (_connection == conn)
    {
        _connection.reset();
        _state = kDisconnected;
    }
}

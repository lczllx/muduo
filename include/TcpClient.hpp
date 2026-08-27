#ifndef MUDUO_TCPCLIENT_H
#define MUDUO_TCPCLIENT_H

#include "CallbackTypes.hpp"
#include "InetAddress.hpp"
#include "Connection.hpp"
#include <memory>
#include <string>

/*TCP 客户端：非阻塞 connect + 建立 Connection
  连接建立后复用 Connection 的读/写/回调机制，对使用者表现为回调驱动
  生命周期约定：loop 必须长于 TcpClient；析构前应调用 stop()/disconnect()*/
class EventLoop;
class TcpClient
{
public:
    TcpClient(EventLoop *loop, const InetAddress &serverAddr, const std::string &name = "TcpClient");
    ~TcpClient();

    void connect();     // 发起连接（断开后可再次调用）
    void disconnect();  // 断开当前连接
    void stop();        // 停止客户端：断开连接并清理 connect 阶段资源

    // 连接建立/断开回调（同一回调，连接建立时传入新连接，断开时传入已断连接）
    void SetConnectionCallback(const ConnectedCallBack &cb);
    void SetMessageCallback(const MessageCallBack &cb);

    bool Connected() const { return _connection && _connection->Connected(); }
    PtrConnection connection() const { return _connection; }

private:
    enum State
    {
        kDisconnected,
        kConnecting,
        kConnected
    };

    void connectInLoop();
    void connecting(int sockfd);             // 注册写事件，等待非阻塞 connect 完成
    void handleWrite();                      // EPOLLOUT：getsockopt 检查连接结果
    void handleError();                      // 连接出错
    void newConnection(int sockfd);          // 连接成功，构造 Connection
    void removeConnection(const PtrConnection &conn); // 连接断开后的内部清理

private:
    EventLoop *_loop;
    InetAddress _server_addr;
    std::string _name;
    State _state;
    uint64_t _next_conn_id;
    std::unique_ptr<Channel> _channel; // connect 阶段的临时 channel
    PtrConnection _connection;         // 建立后的连接
    ConnectedCallBack _connection_cb;
    MessageCallBack _message_cb;
};

#endif

#ifndef MUDUO_CALLBACK_TYPES_H
#define MUDUO_CALLBACK_TYPES_H

#include <functional>
#include <memory>

class Connection;
class Buffer;

using TaskFunc = std::function<void()>;
using ReleaseFunc = std::function<void()>; 
using PtrConnection = std::shared_ptr<Connection>;

//connection.hpp
enum ConneStatus { DISCONNECTED/*连接关闭状态*/, CONNECTING/*连接建立状态*/, CONNECTED/*通信状态*/, DISCONNECTING/*待关闭状态*/ };
using ConnectedCallBack = std::function<void(const PtrConnection&)>;
using ClosedCallBack = std::function<void(const PtrConnection&)>;
using MessageCallBack = std::function<void(const PtrConnection&, Buffer*)>;
using AnyEventCallBack = std::function<void(const PtrConnection&)>;

#endif
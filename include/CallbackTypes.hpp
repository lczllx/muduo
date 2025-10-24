#ifndef MUDUO_CALLBACK_TYPES_H
#define MUDUO_CALLBACK_TYPES_H

#include <functional>
#include <memory>

class Connection;
class Buffer;

using TaskFunc = std::function<void()>;
using ReleaseFunc = std::function<void()>; 
using PtrConnection = std::shared_ptr<Connection>;

enum ConneStatus { DISCONNECTED, CONNECTING, CONNECTED, DISCONNECTING };

using ConnectedCallBack = std::function<void(const PtrConnection&)>;
using ClosedCallBack = std::function<void(const PtrConnection&)>;
using MessageCallBack = std::function<void(const PtrConnection&, Buffer*)>;
using AnyEventCallBack = std::function<void(const PtrConnection&)>;

#endif
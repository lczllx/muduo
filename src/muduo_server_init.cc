#include "../include/TcpServer.hpp"
#include "../include/Logger.hpp"
#include <csignal>

class NetWork {
public:
    NetWork() {
        L_DEBUG("SIGPIPE INIT");
        signal(SIGPIPE, SIG_IGN);//忽略连接断开时向已经关闭的socket写数据造成的信号
    }
};

static NetWork nw;
#include "../include/TcpServer.hpp"
#include "../include/Logger.hpp"
#include <csignal>

class NetWork {
public:
    NetWork() {
        L_DEBUG << "SIGPIPE INIT";
        signal(SIGPIPE, SIG_IGN);
    }
};

static NetWork nw;
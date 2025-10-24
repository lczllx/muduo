#include "../include/Socket.hpp"
#include <cstring>
#include <cerrno>

Socket::Socket() : _sockfd(-1) {}
Socket::Socket(int fd) : _sockfd(fd) {}
Socket::~Socket() { Close(); }

bool Socket::Create() {
    _sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(_sockfd < 0) {
        L_ERROR << "create socket failed";
        return false;
    }
    return true;
}

bool Socket::Bind(const std::string &ip, uint16_t port) {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    socklen_t len = sizeof(struct sockaddr_in);
    int ret = bind(_sockfd, (struct sockaddr*)&addr, len);
    if(ret < 0) {
        L_ERROR << "socket bind failed";
        return false;
    }
    return true;
}


// 监听套接字，等待客户端连接
bool Socket::Listen(int backlog)
{
    int ret=listen(_sockfd,backlog);
    if(ret<0)
    {
        L_ERROR << "socket listen failed";
        return false;
    }
    return true;
}
// 连接远程服务器
bool Socket::Connect(const std::string &ip,uint16_t port)
{
        struct sockaddr_in addr{};
    addr.sin_family=AF_INET;
    addr.sin_port=htons(port);
    addr.sin_addr.s_addr=inet_addr(ip.c_str());
    socklen_t len=sizeof(struct sockaddr_in);
    // int connect(int sockfd, struct sockaddr*addr, socklen_t len);
    int ret=connect(_sockfd,(struct sockaddr*)&addr,len);
    if(ret<0)
    {
        L_ERROR << "socket connect failed";
        return false;
    }
    return true;

}
// 接受客户端连接，返回新的套接字描述符
int Socket::Accept()//可以使用 sockaddr_storage，避免 IPv4/IPv6 不兼容
{
    int newfd=accept(_sockfd,NULL,NULL);
    if(newfd<0)
    {
        L_ERROR << "socket Accept failed";
        return -1;
    }
    return newfd;

}
ssize_t Socket::Recv(void *buf,size_t len,int flag)
{
    ssize_t ret=recv(_sockfd,buf,len,flag);
    if(ret<=0)
    {
        //EAGAIN 表示当前socket的接收缓冲区没有数据，这个错误在非阻塞情况下才会发生
        //EINTR 表示当前socket的阻塞等待被信号打断了
        if(errno==EAGAIN||errno==EINTR)
        {
            return 0;
        }
        L_ERROR << "socket Recv failed";
        return -1;
    }
    return ret;//实际接收的数据长度
}
ssize_t Socket::NonBlockRecv(void *buf,size_t len)
{
    if(len==0)return 0;
    return Recv(buf,len,MSG_DONTWAIT);//MSG_DONTWAIT为接收非阻塞
}
ssize_t Socket::Send(const void *buf,size_t len,int flag)
{
    ssize_t ret=send(_sockfd,buf,len,flag);
    if(ret<0)
    {
        if (errno == EAGAIN || errno == EINTR) //
        {
            return 0;
        }
        L_ERROR << "socket Send failed";
        return -1;
    }
    return ret;//实际发送的数据长度
}
ssize_t Socket::NonBlockSend(const void *buf,size_t len)
{
    if(len==0)return 0;
    return Send(buf,len,MSG_DONTWAIT);//MSG_DONTWAIT为发送非阻塞
}

//创建服务器端，绑定端口，开始监听，并设置非阻塞和地址重用
bool Socket::CreateServer(uint16_t port,const std::string &ip,bool block_flag)
{
    //1 创建套接字 2 绑定地址 3 开始监听 4 设置非阻塞 5 启动地址重用
    if(!Create())return false;
    ReuseAdderess();
    if(block_flag)NoBlock();
    if(!Bind(ip,port))return false;
    if(!Listen())return false;
    return true;
}
// 创建客户端并连接服务器
bool Socket::CreateClient(uint16_t port,const std::string &ip)
{
    //1.创建套接字 2.指定连接服务器
        if(!Create())return false;
        if(!Connect(ip,port))return false;
        return true;
}
// 设置套接字选项，地址重用（端口复用）
void Socket::ReuseAdderess()
{
    //设置套接字选项 setsockopt(int fd,int leve,int optname,void *val,int vallen)
    int val=1;
    setsockopt(_sockfd,SOL_SOCKET,SO_REUSEADDR,(void*)&val,sizeof(int));
    val=1;
    setsockopt(_sockfd,SOL_SOCKET,SO_REUSEPORT,(void*)&val,sizeof(int));
}
void Socket::NoBlock()// 设置套接字为非阻塞模式
{
    int flag = fcntl(_sockfd, F_GETFL, 0);
    if (flag == -1) {
        L_ERROR << "fcntl F_GETFL failed: " << strerror(errno);
        return;
    }
    if (fcntl(_sockfd, F_SETFL, flag | O_NONBLOCK) == -1) {
        L_ERROR << "fcntl F_SETFL failed: " << strerror(errno);
    }
}
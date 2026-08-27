#ifndef MUDUO_INETADDRESS_H
#define MUDUO_INETADDRESS_H

#include <string>
#include <cstdint>
#include <netinet/in.h>

/*网络地址封装：将 ip + port 封装为 sockaddr_in，提供 toIp/toPort/toIpPort 和 DNS 解析
  供连接层获取对端地址、客户端构造远端地址使用*/
class InetAddress
{
private:
    struct sockaddr_in _addr;

public:
    InetAddress();                                          // 默认：0.0.0.0:0
    explicit InetAddress(uint16_t port, bool loopbackOnly = false);
    InetAddress(const std::string &ip, uint16_t port);
    explicit InetAddress(const struct sockaddr_in &addr);

    std::string toIp() const;       // 返回点分十进制 IP
    std::string toIpPort() const;   // 返回 "ip:port"
    uint16_t toPort() const;        // 返回主机序端口

    const struct sockaddr_in &getSockAddr() const { return _addr; }

    // DNS 解析 hostname → InetAddress，成功返回 true，失败返回 false
    static bool resolve(const std::string &hostname, InetAddress *result);
};

#endif

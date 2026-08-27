#include "../include/InetAddress.hpp"
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>

InetAddress::InetAddress()
{
    std::memset(&_addr, 0, sizeof(_addr));
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = htonl(INADDR_ANY);
    _addr.sin_port = 0;
}

InetAddress::InetAddress(uint16_t port, bool loopbackOnly)
{
    std::memset(&_addr, 0, sizeof(_addr));
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = htonl(loopbackOnly ? INADDR_LOOPBACK : INADDR_ANY);
    _addr.sin_port = htons(port);
}

InetAddress::InetAddress(const std::string &ip, uint16_t port)
{
    std::memset(&_addr, 0, sizeof(_addr));
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(port);
    if (::inet_pton(AF_INET, ip.c_str(), &_addr.sin_addr) <= 0)
    {
        // 非法 IP 回退到通配地址，避免上层因解析失败而崩溃
        _addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
}

InetAddress::InetAddress(const struct sockaddr_in &addr)
    : _addr(addr) {}

std::string InetAddress::toIp() const
{
    char buf[INET_ADDRSTRLEN] = {0};
    ::inet_ntop(AF_INET, &_addr.sin_addr, buf, sizeof(buf));
    return std::string(buf);
}

uint16_t InetAddress::toPort() const
{
    return ntohs(_addr.sin_port);
}

std::string InetAddress::toIpPort() const
{
    return toIp() + ":" + std::to_string(toPort());
}

bool InetAddress::resolve(const std::string &hostname, InetAddress *result)
{
    if (result == nullptr)
        return false;

    struct addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *res = nullptr;
    if (::getaddrinfo(hostname.c_str(), nullptr, &hints, &res) != 0)
        return false;

    bool ok = false;
    if (res != nullptr && res->ai_addr != nullptr)
    {
        *result = InetAddress(*reinterpret_cast<struct sockaddr_in *>(res->ai_addr));
        ok = true;
    }
    ::freeaddrinfo(res);
    return ok;
}

#include "../include/TcpServer.hpp"
#include "../include/Logger.hpp"
#include <iostream>
class EchoServer
{
    private:
    TcpServer _server;
    void OnConnected(const PtrConnection&conne)
    {
        L_DEBUG << "new connected " << conne.get();
    }
    void OnClosed(const PtrConnection&conne)
    {
        L_DEBUG << "close connected " << conne.get();
    }
    void OnMessage(const PtrConnection&conne,Buffer *buf)
    {
    if (!buf || buf->ReadableBytes() == 0) return;  // 增加空指针和空数据检查
        std::string response = "hello ";
        conne->Send(response.c_str(), response.size());
        conne->Shutdown();
    }
    public:

    EchoServer(int port):_server(port){
    _server.SetThreadCnt(2);
    _server.EnableInactiveRelease(10);
    _server.SetMessageCallBack(std::bind(&EchoServer::OnMessage,this,std::placeholders::_1,std::placeholders::_2));
    _server.SetConnectedCallBack(std::bind(&EchoServer::OnConnected,this,std::placeholders::_1));
    _server.SetClosedCallBack(std::bind(&EchoServer::OnClosed,this,std::placeholders::_1));
    
    }
    void Start(){_server.Start();}

};


int main()
{
    EchoServer echo(8889);
    echo.Start();


    return 0;
}
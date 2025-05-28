#include "SQLServer.hpp"
#include <functional>
#include "json.hpp"
using json=nlohmann::json;
#include <string>
using namespace std::placeholders;
#include "SQLService.hpp"


SQLServer::SQLServer(EventLoop *loop,
                     const InetAddress &listenAddr,
                     const string &nameArg)
    : _server(loop, listenAddr, nameArg),
      _loop(loop)
{
    // 连接回调
    _server.setConnectionCallback(std::bind(&SQLServer::onConnection, this, _1));
    // 读写回调
    _server.setMessageCallback(std::bind(&SQLServer::onMessage, this, _1, _2, _3));
    // 设置服务器端的线程数量
    _server.setThreadNum(3); // 1 I/O 3 worker
}

void SQLServer::start()
{
    _server.start();
}

void SQLServer::onConnection(const TcpConnectionPtr &conn)
{
    if (!conn->connected())
    {
        conn->shutdown();
    }
    
}
void SQLServer::onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp time)
{
    string buf = buffer->retrieveAllAsString();
    json js=json::parse(buf);

    // 解耦合业务和网络模块
    auto msgHandler=SQLService::getInstance()->getHandler(js["msgid"].get<int>());
    msgHandler(conn,js,time);

}

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <iostream>
#include <functional>
#include <string>
using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace std::placeholders;

class SQLServer
{
public:
    SQLServer(EventLoop *loop,
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

    void start()
    {
        _server.start();
    }

private:
    void onConnection(const TcpConnectionPtr &conn)
    {
        if (conn->connected())
        {
            cout << conn->peerAddress().toIpPort() << "->" << conn->localAddress().toIpPort() << " state:online" << endl;
        }
        else
        {
            cout << conn->peerAddress().toIpPort() << "->" << conn->localAddress().toIpPort() << " state:offline" << endl;
            conn->shutdown();
        }
    }
    void onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp time)
    {
        string buf = buffer->retrieveAllAsString();
        cout << "recv data :" << buf << " time" << time.toString() << endl;
        conn->send(buf);
    }
    // 创建TcpServer对象
    TcpServer _server;
    // 事件循环指针
    EventLoop *_loop;
};

int main()
{
    EventLoop loop;
    InetAddress addr("127.0.0.1", 6000);
    SQLServer server(&loop, addr, "SQLServer");
    server.start();
    loop.loop();
    return 0;
}
#include <iostream>
#include "SQLServer.hpp"
#include <signal.h>
#include "SQLService.hpp"

using namespace std;

void resetHandler(int){
    SQLService::getInstance()->reset();
    exit(0);
}
int main()
{
    signal(SIGINT,resetHandler);
    EventLoop loop;
    InetAddress addr("127.0.0.1", 10000);
    SQLServer server(&loop, addr, "SQLServer");
    server.start();
    loop.loop();
    return 0;
}
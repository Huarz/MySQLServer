#include <string>
#include <ctime>
#include <iostream>
#include <mysql/mysql.h>

using namespace std;

// 实现mysql C API的包装

class Connection
{
public:
    Connection();
    ~Connection();
    bool connect(string ip,
                 unsigned short port,
                 string user,
                 string password,
                 string dbname);
    bool update(string sql);
    MYSQL_RES *query(string sql);
    void refreshAliveTime() { _aliveTime = clock(); }
    clock_t getAliveTime() const { return clock() - _aliveTime; }
    MYSQL *getConnection();

private:
    MYSQL *_conn;
    clock_t _aliveTime;
};
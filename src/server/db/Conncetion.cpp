#include "Connection.hpp"
#include <muduo/base/Logging.h>

using namespace std;

Connection::Connection()
{
    _conn = mysql_init(nullptr);
}

Connection::~Connection()
{
    if (_conn != nullptr)
        mysql_close(_conn);
}

bool Connection::connect(string ip, unsigned short port, string username,
                         string password, string dbname)
{
    MYSQL *p = mysql_real_connect(_conn, ip.c_str(), username.c_str(),
                                  password.c_str(),
                                  dbname.c_str(), port, nullptr, 0);
    if (p != nullptr)
    {
        // C和C++代码默认的编码字符是ASCII，如果不设置，从MySQL上拉下来的中文显示？
        mysql_query(_conn, "set names gbk");
        LOG_INFO << "connect mysql success!";
        return true;
    }
    else
    {
        LOG_INFO << "connect mysql fail!";
        return false;
    }
}

bool Connection::update(string sql)
{
    if (mysql_query(_conn, sql.c_str()))
    {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
                 << sql << "更新失败!";
        return false;
    }

    return true;
    ;
}

MYSQL_RES *Connection::query(string sql)
{
    if (mysql_query(_conn, sql.c_str()))
    {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
                 << sql << "查询失败!";
        return nullptr;
    }
    return mysql_use_result(_conn);
}

MYSQL *Connection::getConnection()
{
    return _conn;
}
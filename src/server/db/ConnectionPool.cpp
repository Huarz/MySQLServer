#include "ConnectionPool.hpp"
#include <muduo/base/Logging.h>

ConnectionPool *ConnectionPool::getConnectionPool()
{
    static ConnectionPool pool;
    return &pool;
}

bool ConnectionPool::loadConfigFile()
{
    FILE *pf = fopen("mysql.conf", "r");
    if (pf == nullptr)
    {
        LOG_INFO << "mysql.conf file is not exsit!";
        return false;
    }
    char line[1024] = {0};
    while (fgets(line, 1024, pf) != nullptr)
    {
        string str = line;
        int idx = str.find("=");
        if (idx == -1)
        {
            continue;
        }
        // password=123456\n
        int endidx = str.find("\n", idx);
        string key = str.substr(0, idx);
        string value = str.substr(idx + 1, endidx - idx - 1);
        if (key == "ip")
            _ip = value;
        else if (key == "port")
            _port = atoi(value.c_str());
        else if (key == "username")
            _username = value;
        else if (key == "password")
            _password = value;
        else if (key == "dbname")
            _dbname = value;
        else if (key == "initSize")
            _initSize = atoi(value.c_str());
        else if (key == "maxSize")
            _maxSize = atoi(value.c_str());
        else if (key == "maxIdleTime")
            _maxIdleTime = atoi(value.c_str());
        else if (key == "connectionTimeOut")
            _connectionTimeout = atoi(value.c_str());
    }
    fclose(pf);
    return true;
}

ConnectionPool::ConnectionPool()
{
    if (!loadConfigFile())
    {
        LOG_INFO << "mysql.conf file error";
    }
    for (int i = 0; i < _initSize; i++)
    {
        Connection *p = new Connection();
        p->Connection::connect(_ip, _port, _username, _password, _dbname);
        p->refreshAliveTime();
        _connectionQue.push(p);
        _connectionCnt++;
    }
    thread produce(std::bind(&ConnectionPool::produceConnectionTask, this));
    produce.detach();
    thread scanner(std::bind(&ConnectionPool::scannerConnectionTask, this));
    scanner.detach();
}

void ConnectionPool::produceConnectionTask()
{
    while (true)
    {
        unique_lock<mutex> lock(_queueMutex);
        while (!_connectionQue.empty())
        {
            cv.wait(lock);
        }
        if (_connectionCnt < _maxSize)
        {
            Connection *p = new Connection();
            p->connect(_ip, _port, _username, _password, _dbname);
            p->refreshAliveTime(); // 刷新一下开始空闲的起始时间
            _connectionQue.push(p);
            _connectionCnt++;
        }
        cv.notify_all();
    }
}

shared_ptr<Connection> ConnectionPool::getConnection()
{
    unique_lock<mutex> lock(_queueMutex);
    while (_connectionQue.empty())
    {
        if (cv_status::timeout == cv.wait_for(lock, chrono::milliseconds(_connectionTimeout)))
        {
            if (_connectionQue.empty())
            {
                LOG_INFO << "获取空闲连接超时了...获取连接失败!";
            }
            return nullptr;
        }
    }
    shared_ptr<Connection> sp(_connectionQue.front(), [&](Connection *pcon)
                              {
        unique_lock<mutex>lock(_queueMutex);
        pcon->refreshAliveTime();
        _connectionQue.push(pcon); });
    _connectionQue.pop();
    cv.notify_all();
    return sp;
}

void ConnectionPool::scannerConnectionTask()
{
    while (1)
    {
        this_thread::sleep_for(chrono::seconds(_maxIdleTime));
        unique_lock<mutex> lock(_queueMutex);
        while (_connectionCnt > _initSize)
        {
            Connection *p = _connectionQue.front();
            if (p->getAliveTime() >= _maxIdleTime * 1000)
            {
                _connectionQue.pop();
                _connectionCnt--;
                delete p;
            }
            else
                break;
        }
    }
}

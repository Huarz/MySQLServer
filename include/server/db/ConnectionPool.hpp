#include "Connection.hpp"
#include "queue"
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <functional>

class ConnectionPool
{
public:
    static ConnectionPool *getConnectionPool();
    shared_ptr<Connection> getConnection();

private:
    bool loadConfigFile();
    void produceConnectionTask();
    void scannerConnectionTask();
    ConnectionPool();
    ~ConnectionPool() = default;
    string _ip;
    unsigned short _port;
    string _username;
    string _password;
    string _dbname;
    int _initSize;
    int _maxSize;
    int _maxIdleTime;
    int _connectionTimeout;
    queue<Connection *> _connectionQue;
    mutex _queueMutex;
    atomic_int _connectionCnt;
    condition_variable cv;
};
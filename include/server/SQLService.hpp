#ifndef SQLSERVICE_H
#define SQLSERVICE_H

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include "studentModel.hpp"
#include "teacherModel.hpp"
#include "adminModel.hpp"
#include <mutex>
using namespace std;
using namespace muduo;
using namespace muduo::net;


#include "json.hpp"
using json = nlohmann::json;

// 表示处理消息的事件回调方法类型
using MsgHandler = std::function<void(const TcpConnectionPtr &conn, json &js, Timestamp)>;

// 聊天服务器业务类
class SQLService
{
public:
    // 获取单例对象的接口函数
    static SQLService *getInstance();
    // 处理学生登录业务
    void stuLogin(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 处理学生注册业务
    void stuReg(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 处理教师登录业务
    void teaLogin(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 处理教师注册业务
    void teaReg(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 处理管理员登录业务
    void adminLogin(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 处理管理员注册业务
    void adminReg(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 处理客户端异常退出
    void stuClientCloseException(const TcpConnectionPtr &conn);
    void teaClientCloseException(const TcpConnectionPtr &conn);
    void adminClientCloseException(const TcpConnectionPtr &conn);
    // 服务器异常，业务重置方法
    void reset();
    // 获取消息对应的处理器
    MsgHandler getHandler(int msgid);
    

private:
   SQLService();

    // 存储消息id和其对应的业务处理方法
    unordered_map<int, MsgHandler> _msgHandlerMap;
    unordered_map<string,TcpConnectionPtr> _stuConnMap;
    unordered_map<string,TcpConnectionPtr> _teaConnMap;
    unordered_map<string,TcpConnectionPtr> _adminConnMap;

    
    StudentModel _stuModel;
    TeacherModel _teaModel;
    AdminModel   _adminModel;
    // 定义互斥锁，保证_userConnMap的线程安全
    mutex _connMutex;

};

#endif
#include "SQLService.hpp"
#include "public.hpp"
#include <muduo/base/Logging.h>

using namespace muduo;
SQLService *SQLService::getInstance()
{
    static SQLService service;
    return &service;
}

SQLService::SQLService()
{

    // 学生相关消息处理
    _msgHandlerMap.insert({STU_LOGIN_MSG, std::bind(&SQLService::stuLogin, this, _1, _2, _3)});
    _msgHandlerMap.insert({STU_REG_MSG, std::bind(&SQLService::stuReg, this, _1, _2, _3)});

    // 教师相关消息处理
    _msgHandlerMap.insert({TEA_LOGIN_MSG, std::bind(&SQLService::teaLogin, this, _1, _2, _3)});
    _msgHandlerMap.insert({TEA_REG_MSG, std::bind(&SQLService::teaReg, this, _1, _2, _3)});

    // 管理员相关消息处理
    _msgHandlerMap.insert({AD_LOGIN_MSG, std::bind(&SQLService::adminLogin, this, _1, _2, _3)});
    _msgHandlerMap.insert({AD_REG_MSG, std::bind(&SQLService::adminReg, this, _1, _2, _3)});
}

MsgHandler SQLService::getHandler(int msgid)
{
    auto it = _msgHandlerMap.find(msgid);
    if (it == _msgHandlerMap.end())
    {
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp time)
        {
            LOG_ERROR << "msgid " << msgid << " can not find handler";
        };
    }
    else
    {
        return _msgHandlerMap[msgid];
    }
}

void SQLService::stuLogin(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    string id = js["id"];
    string pwd = js["password"];
    Student stu = _stuModel.query(id);
    if (stu.getStudentID() == id && stu.getPassword() == pwd)
    {
        if (stu.getState() == "online")
        {
            json response;
            response["msgid"] = STU_LOGIN_MSG_ACK;
            response["errno"] = 1;
            response["errmsg"] = "该学生已登录";
            conn->send(response.dump());
        }
        else
        {
            {
                lock_guard<mutex> lock(_connMutex);
                _stuConnMap.insert({id, conn});
            }
            json response;
            response["msgid"] = STU_LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["studentId"] = stu.getStudentID();
            response["gender"] = stu.getGender();
            response["enteranceAge"] = stu.getEnteranceAge();
            response["enteranceYear"] = stu.getEnteranceYear();
            response["stuClass"] = stu.getClass();
            stu.setState("online");
            _stuModel.updateState(stu);
            conn->send(response.dump());
        }
    }
    else
    {
        json response;
        response["msgid"] = STU_LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "学生ID错误或者密码错误";
        conn->send(response.dump());
    }
}

void SQLService::stuReg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    Student stu;
    stu.setStudentName(js["StudentName"]);
    stu.setPassword(js["password"]);
    stu.setGender(js["gender"] == "male" ? Gender::male : Gender::female);
    stu.setEnteranceAge(js["entranceAge"].get<int>());
    stu.setEnteranceYear(js["entranceYear"].get<int>());
    stu.setClass(js["class"]);
    stu.setState("offline");

    bool state = _stuModel.insert(stu);

    if (state)
    {
        json response;
        response["msgid"] = STU_REG_MAG_ACK;
        response["errno"] = 0;
        response["id"] = stu.getStudentID();
        conn->send(response.dump());
    }
    else
    {
        json response;
        response["msgid"] = STU_REG_MAG_ACK;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}

void SQLService::teaLogin(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    string id = js["id"];
    string pwd = js["password"];
    Teacher teacher = _teaModel.query(id);
    if (teacher.getTeacherID() == id && teacher.getPassword() == pwd)
    {
        if (teacher.getState() == "online")
        {
            json response;
            response["msgid"] = TEA_LOGIN_MSG_ACK;
            response["errno"] = 1;
            response["errmsg"] = "该教师已登录";
            conn->send(response.dump());
        }
        else
        {
            {
                lock_guard<mutex> lock(_connMutex);
                _teaConnMap.insert({id, conn});
            }
            json response;
            response["msgid"] = TEA_LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["teacherId"] = teacher.getTeacherID();
            response["teacherName"] = teacher.getTeacherName();
            response["courseIds"] = teacher.getCourseIDs();
            teacher.setState("online");
            _teaModel.updateState(teacher);
            conn->send(response.dump());
        }
    }
    else
    {
        json response;
        response["msgid"] = TEA_LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "教师ID错误或者密码错误";
        conn->send(response.dump());
    }
}

void SQLService::teaReg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    Teacher teacher;
    teacher.setTeacherName(js["TeacherName"]);
    teacher.setPassword(js["password"]);
    teacher.setCourseIDs(js["courseIds"]);
    teacher.setState("offline");

    bool state = _teaModel.insert(teacher);

    if (state)
    {
        json response;
        response["msgid"] = TEA_REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = teacher.getTeacherID();
        conn->send(response.dump());
    }
    else
    {
        json response;
        response["msgid"] = TEA_REG_MSG_ACK;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}

void SQLService::adminLogin(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    string id = js["id"];
    string pwd = js["password"];
    Admin admin = _adminModel.query(id);
    if (admin.getAdminID() == id && admin.getPassword() == pwd)
    {
        if (admin.getState() == "online")
        {
            json response;
            response["msgid"] = AD_LOGIN_MSG_ACK;
            response["errno"] = 1;
            response["errmsg"] = "该管理员已登录";
            conn->send(response.dump());
        }
        else
        {
            {
                lock_guard<mutex> lock(_connMutex);
                _adminConnMap.insert({id, conn});
            }
            json response;
            response["msgid"] = AD_LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["adminId"] = admin.getAdminID();
            response["adminName"] = admin.getAdminName();
            admin.setState("online");
            _adminModel.updateState(admin);
            conn->send(response.dump());
        }
    }
    else
    {

        json response;
        response["msgid"] = AD_LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "管理员ID错误或者密码错误";
        conn->send(response.dump());
    }
}

void SQLService::adminReg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    Admin admin;
    admin.setAdminName(js["AdminName"]);
    admin.setPassword(js["password"]);
    admin.setState("offline");

    bool state = _adminModel.insert(admin);

    if (state)
    {
        json response;
        response["msgid"] = AD_REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = admin.getAdminID();
        conn->send(response.dump());
    }
    else
    {
        json response;
        response["msgid"] = AD_REG_MSG_ACK;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}

void SQLService::stuClientCloseException(const TcpConnectionPtr &conn)
{
    Student stu;
    {
        lock_guard<mutex> lock(_connMutex);
        for (auto it = _stuConnMap.begin(); it != _stuConnMap.end(); ++it)
        {
            if (it->second == conn)
            {
                stu.setStudentID(it->first);
                _stuConnMap.erase(it);
                break;
            }
        }
    }
    stu.setState("offline");
    _stuModel.updateState(stu);
}

void SQLService::teaClientCloseException(const TcpConnectionPtr &conn)
{
    Teacher tea;
    {
        lock_guard<mutex> lock(_connMutex);
        for (auto it = _teaConnMap.begin(); it != _teaConnMap.end(); ++it)
        {
            if (it->second == conn)
            {
                tea.setTeacherID(it->first);
                _teaConnMap.erase(it);
                break;
            }
        }
    }
    tea.setState("offline");
    _teaModel.updateState(tea);
}

void SQLService::adminClientCloseException(const TcpConnectionPtr &conn)
{
    Admin admin;
    {
        lock_guard<mutex> lock(_connMutex);
        for (auto it = _adminConnMap.begin(); it != _adminConnMap.end(); ++it)
        {
            if (it->second == conn)
            {
                admin.setAdminID(it->first);
                _adminConnMap.erase(it);
                break;
            }
        }
    }
    admin.setState("offline");
    _adminModel.updateState(admin);
}

void SQLService::reset(){
    _stuModel.resetState();
    _teaModel.resetState();
    _adminModel.resetState();
}
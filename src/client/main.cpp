#include "json.hpp"

#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
using namespace std;
using json = nlohmann::json;

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <semaphore.h>
#include <atomic>

#include "public.hpp" // 包含消息类型定义

// 记录当前登录用户信息
struct CurrentUser
{
    string id;
    string name;
    string type; // "student", "teacher", "admin"
};

CurrentUser g_currentUser;
atomic_bool g_isLoginSuccess{false};
sem_t rwsem;
bool isMainMenuRunning = false;

// 函数声明
void readTaskHandler(int clientfd);
void showMainMenu(int clientfd);
void handleStudentMenu(int clientfd);
void handleTeacherMenu(int clientfd);
void handleAdminMenu(int clientfd);

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        cerr << "Usage: " << argv[0] << " <IP> <PORT>" << endl;
        exit(-1);
    }

    // 创建socket并连接服务器
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == clientfd)
    {
        cerr << "Socket creation error" << endl;
        exit(-1);
    }

    sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));
    server.sin_addr.s_addr = inet_addr(argv[1]);

    if (-1 == connect(clientfd, (sockaddr *)&server, sizeof(server)))
    {
        cerr << "Connection failed" << endl;
        close(clientfd);
        exit(-1);
    }

    sem_init(&rwsem, 0, 0);
    thread readTask(readTaskHandler, clientfd);
    readTask.detach();

    while (true)
    {
        cout << "===== 教务管理系统 =====" << endl;
        cout << "1. 学生登录" << endl;
        cout << "2. 教师登录" << endl;
        cout << "3. 管理员登录" << endl;
        cout << "4. 学生注册" << endl;
        cout << "5. 教师注册" << endl;
        cout << "6. 管理员注册" << endl;
        cout << "0. 退出系统" << endl;
        cout << "========================" << endl;
        cout << "请选择操作: ";

        int choice;
        cin >> choice;
        cin.ignore(); // 清除输入缓冲区

        json js;
        string userType;

        switch (choice)
        {
        case 1:
            userType = "student";
            goto login;
        case 2:
            userType = "teacher";
            goto login;
        case 3:
            userType = "admin";
            goto login;
        case 4:
            userType = "student";
            goto reg;
        case 5:
            userType = "teacher";
            goto reg;
        case 6:
            userType = "admin";
            goto reg;
        case 0:
            close(clientfd);
            sem_destroy(&rwsem);
            exit(0);
        default:
            cerr << "无效选项!" << endl;
            continue;
        }

    login:
    {
        string id, password;
        cout << "请输入ID: ";
        getline(cin, id);
        cout << "请输入密码: ";
        getline(cin, password);

        js["msgid"] =
            (userType == "student") ? STU_LOGIN_MSG : (userType == "teacher") ? TEA_LOGIN_MSG
                                                                              : AD_LOGIN_MSG;
        js["id"] = id;
        js["password"] = password;
        break;
    }

    reg:
    {
        string name, password;
        cout << "请输入姓名: ";
        getline(cin, name);
        cout << "请输入密码: ";
        getline(cin, password);

        // 根据用户类型设置消息ID
        js["msgid"] =
            (userType == "student") ? STU_REG_MSG : (userType == "teacher") ? TEA_REG_MSG
                                                                            : AD_REG_MSG;

        // 公共字段
        js["name"] = name;
        js["password"] = password;

        // 用户类型特定字段
        if (userType == "student")
        {
            // 学生注册额外字段
            cout << "请输入性别 (male/female): ";
            string gender;
            getline(cin, gender);
            js["gender"] = gender;

            cout << "请输入入学年龄: ";
            string age;
            getline(cin, age);
            js["entranceAge"] = stoi(age);

            cout << "请输入入学年份: ";
            string year;
            getline(cin, year);
            js["entranceYear"] = stoi(year);

            cout << "请输入班级: ";
            string className;
            getline(cin, className);
            js["class"] = className;
        }
        else if (userType == "teacher")
        {
            // 教师注册额外字段
            cout << "请输入教授的课程ID (多个用逗号分隔): ";
            string courseIds;
            getline(cin, courseIds);
            js["courseIds"] = courseIds;
        }
        break;
    }

        string request = js.dump();
        g_isLoginSuccess = false;
        send(clientfd, request.c_str(), request.size() + 1, 0);
        sem_wait(&rwsem);

        if (g_isLoginSuccess)
        {
            isMainMenuRunning = true;
            showMainMenu(clientfd);
        }
    }

    return 0;
}

void readTaskHandler(int clientfd)
{
    char buffer[4096];
    while (true)
    {
        int len = recv(clientfd, buffer, sizeof(buffer), 0);
        if (len <= 0)
        {
            cerr << "服务器连接断开" << endl;
            exit(-1);
        }

        json js = json::parse(buffer);
        int msgid = js["msgid"];

        // 处理登录响应
        if (msgid == STU_LOGIN_MSG_ACK || msgid == TEA_LOGIN_MSG_ACK || msgid == AD_LOGIN_MSG_ACK)
        {
            if (js["errno"] == 0)
            {
                g_currentUser.id = js["id"].is_string() ? js["id"].get<string>() : to_string(js["id"].get<int>());
                g_currentUser.type =
                    (msgid == STU_LOGIN_MSG_ACK) ? "student" : (msgid == TEA_LOGIN_MSG_ACK) ? "teacher"
                                                                                            : "admin";

                if (msgid == STU_LOGIN_MSG_ACK)
                {
                    g_currentUser.name = js["studentName"].get<string>();
                }
                else if (msgid == TEA_LOGIN_MSG_ACK)
                {
                    g_currentUser.name = js["teacherName"].get<string>();
                }
                else
                {
                    g_currentUser.name = js["adminName"].get<string>();
                }

                g_isLoginSuccess = true;
                cout << "登录成功! 欢迎," << g_currentUser.name << endl;
            }
            else
            {
                cerr << "登录失败: " << js["errmsg"].get<string>() << endl;
            }
            sem_post(&rwsem);
        }
        // 处理注册响应
        else if (msgid == STU_REG_MAG_ACK || msgid == TEA_REG_MSG_ACK || msgid == AD_REG_MSG_ACK)
        {
            if (js["errno"] == 0)
            {
                string id = js["id"].is_string() ? js["id"].get<string>() : to_string(js["id"].get<int>());
                cout << "注册成功! 您的ID: " << id << endl;
            }
            else
            {
                cerr << "注册失败" << endl;
            }
            sem_post(&rwsem);
        }
        // 处理其他操作响应
        else
        {
            cout << "\n===== 操作结果 =====" << endl;
            if (js.contains("errno") && js["errno"] != 0)
            {
                cerr << "错误: " << js["errmsg"].get<string>() << endl;
            }
            else
            {
                // 显示查询结果
                if (js.contains("courses"))
                {
                    for (auto &course : js["courses"])
                    {
                        cout << "课程ID: " << course["courseID"]
                             << " 名称: " << course["courseName"]
                             << " 学分: " << course["credit"] << endl;
                    }
                }
                else if (js.contains("courseChoosings"))
                {
                    for (auto &cc : js["courseChoosings"])
                    {
                        cout << "课程ID: " << cc["courseID"]
                             << " 成绩: " << cc["score"] << endl;
                    }
                }
                else if (js.contains("studentGrades"))
                {
                    for (auto &sg : js["studentGrades"])
                    {
                        cout << "学号: " << sg["studentID"]
                             << " 平均分: " << sg["averageScore"] << endl;
                    }
                }
                else if (js.contains("data"))
                {
                    for (auto &item : js["data"])
                    {
                        if (item.contains("studentID"))
                        {
                            cout << "学号: " << item["studentID"]
                                 << " 成绩: " << item["score"] << endl;
                        }
                        else
                        {
                            cout << "课程ID: " << item["courseId"]
                                 << " 名称: " << item["courseName"] << endl;
                        }
                    }
                }
                // 显示操作成功信息
                else if (js.contains("errmsg"))
                {
                    cout << js["errmsg"].get<string>() << endl;
                }
            }
            cout << "=====================" << endl;
            cout << "按回车键继续...";
            cin.ignore();
        }
    }
}

void showMainMenu(int clientfd)
{
    while (isMainMenuRunning)
    {
        cout << "\n===== 主菜单 =====" << endl;
        cout << "用户类型: " << g_currentUser.type
             << " | ID: " << g_currentUser.id
             << " | 姓名: " << g_currentUser.name << endl;

        if (g_currentUser.type == "student")
        {
            handleStudentMenu(clientfd);
        }
        else if (g_currentUser.type == "teacher")
        {
            handleTeacherMenu(clientfd);
        }
        else if (g_currentUser.type == "admin")
        {
            handleAdminMenu(clientfd);
        }
    }
}

void handleStudentMenu(int clientfd)
{
    cout << "\n===== 学生功能 =====" << endl;
    cout << "1. 查询课程信息" << endl;
    cout << "2. 查询所有课程" << endl;
    cout << "3. 查询我的选课" << endl;
    cout << "4. 查询课程成绩" << endl;
    cout << "0. 退出登录" << endl;
    cout << "====================" << endl;
    cout << "请选择操作: ";

    int choice;
    cin >> choice;
    cin.ignore();

    json js;
    js["studentID"] = g_currentUser.id;

    switch (choice)
    {
    case 1:
    {
        cout << "输入课程ID: ";
        string courseId;
        getline(cin, courseId);
        js["msgid"] = STU_QUERY_COURSE;
        js["courseId"] = courseId;
        break;
    }
    case 2:
    {
        js["msgid"] = STU_QUERY_ALL_COURSE;
        break;
    }
    case 3:
    {
        js["msgid"] = STU_QUERY_COURSE_CHOOSING;
        break;
    }
    case 4:
    {
        cout << "输入课程ID: ";
        string courseId;
        getline(cin, courseId);
        js["msgid"] = STU_QUERY_GRADE;
        js["courseID"] = courseId;
        break;
    }
    case 0:
    {
        break;
    }
    default:
    {
        cerr << "无效选项!" << endl;
        return;
    }
    }

    string request = js.dump();
    send(clientfd, request.c_str(), request.size() + 1, 0);
}

void handleTeacherMenu(int clientfd)
{
    cout << "\n===== 教师功能 =====" << endl;
    cout << "1. 更新学生成绩" << endl;
    cout << "2. 查询课程选课情况" << endl;
    cout << "0. 退出登录" << endl;
    cout << "====================" << endl;
    cout << "请选择操作: ";

    int choice;
    cin >> choice;
    cin.ignore();

    json js;
    js["teacherId"] = g_currentUser.id;

    switch (choice)
    {
    case 1:
    {
        cout << "输入学生ID: ";
        string studentId;
        getline(cin, studentId);
        cout << "输入课程ID: ";
        string courseId;
        getline(cin, courseId);
        cout << "输入新成绩: ";
        double score;
        cin >> score;
        cin.ignore();

        js["msgid"] = TEA_UPDATE_SCORE;
        js["studentID"] = studentId;
        js["courseID"] = courseId;
        js["score"] = score;
        break;
    }
    case 2:
    {
        cout << "输入课程ID: ";
        string courseId;
        getline(cin, courseId);
        js["msgid"] = TEA_QUERY_COURSE_CHOOSING;
        js["courseID"] = courseId;
        break;
    }
    case 0:
    {
        break;
    }
    default:
    {
        cerr << "无效选项!" << endl;
        return;
    }
    }

    string request = js.dump();
    send(clientfd, request.c_str(), request.size() + 1, 0);
}

void handleAdminMenu(int clientfd)
{
    cout << "\n===== 管理员功能 =====" << endl;
    cout << "1. 添加课程" << endl;
    cout << "2. 查询课程" << endl;
    cout << "3. 查询所有课程" << endl;
    cout << "4. 删除课程" << endl;
    cout << "5. 添加选课记录" << endl;
    cout << "6. 查询所有选课" << endl;
    cout << "7. 删除选课记录" << endl;
    cout << "8. 查询学生成绩" << endl;
    cout << "9. 查询所有学生成绩" << endl;
    cout << "10. 查询班级成绩" << endl;
    cout << "11. 查询课程平均分" << endl;
    cout << "12. 查询班级课程平均分" << endl;
    cout << "0. 退出登录" << endl;
    cout << "=====================" << endl;
    cout << "请选择操作: ";

    int choice;
    cin >> choice;
    cin.ignore(); // 清除输入缓冲区中的换行符

    json js;
    string inputStr; // 用于字符串输入的临时变量
    double inputDbl; // 用于浮点数输入的临时变量
    int inputInt;    // 用于整数输入的临时变量

    switch (choice)
    {
    case 1:
    { // 添加课程
        js["msgid"] = AD_INSERT_COURSE;

        cout << "课程名称: ";
        getline(cin, inputStr);
        js["courseName"] = inputStr;

        cout << "教师ID: ";
        getline(cin, inputStr);
        js["teacherId"] = inputStr;

        cout << "学分: ";
        cin >> inputDbl;
        js["credit"] = inputDbl;

        cout << "年级: ";
        cin >> inputInt;
        js["grade"] = inputInt;

        cout << "取消年份: ";
        cin >> inputInt;
        js["canceledYear"] = inputInt;

        cin.ignore(); // 清除数值输入后的换行符
        break;
    }
    case 2:
    { // 查询课程
        js["msgid"] = AD_QUERY_COURSE;

        cout << "课程ID: ";
        getline(cin, inputStr);
        js["courseId"] = inputStr;
        break;
    }
    case 3:
    { // 查询所有课程
        js["msgid"] = AD_QUERY_ALL_COURSE;
        break;
    }
    case 4:
    { // 删除课程
        js["msgid"] = AD_REMOVE_COURSE;

        cout << "课程ID: ";
        getline(cin, inputStr);
        js["courseId"] = inputStr;
        break;
    }
    case 5:
    { // 添加选课记录
        js["msgid"] = AD_INSERT_CHOOSING;

        cout << "学生ID: ";
        getline(cin, inputStr);
        js["studentID"] = inputStr;

        cout << "课程ID: ";
        getline(cin, inputStr);
        js["courseID"] = inputStr;

        cout << "教师ID: ";
        getline(cin, inputStr);
        js["teacherID"] = inputStr;

        cout << "选课年份: ";
        getline(cin, inputStr);
        js["chosenYear"] = inputStr;

        cout << "成绩: ";
        cin >> inputDbl;
        js["score"] = inputDbl;

        cin.ignore(); // 清除数值输入后的换行符
        break;
    }
    case 6:
    { // 查询所有选课
        js["msgid"] = AD_QUERY_ALL_CHOOSING;
        break;
    }
    case 7:
    { // 删除选课记录
        js["msgid"] = AD_REMOVE_CHOOSING;

        cout << "学生ID: ";
        getline(cin, inputStr);
        js["studentID"] = inputStr;

        cout << "课程ID: ";
        getline(cin, inputStr);
        js["courseID"] = inputStr;
        break;
    }
    case 8:
    { // 查询学生成绩
        js["msgid"] = AD_QUERY_STUDENT_GRADES;

        cout << "学生ID: ";
        getline(cin, inputStr);
        js["studentID"] = inputStr;
        break;
    }
    case 9:
    { // 查询所有学生成绩
        js["msgid"] = AD_QUERY_ALL_STUDENT_GRADES;
        break;
    }
    case 10:
    { // 查询班级成绩
        js["msgid"] = AD_QUERY_CLASS_GRADES;

        cout << "班级名称: ";
        getline(cin, inputStr);
        js["className"] = inputStr;
        break;
    }
    case 11:
    { // 查询课程平均分
        js["msgid"] = AD_QUERY_COURSES_AVERAGE;
        break;
    }
    case 12:
    { // 查询班级课程平均分
        js["msgid"] = AD_QUERY_CLASS_COURSES_AVERAGE;

        cout << "班级名称: ";
        getline(cin, inputStr);
        js["className"] = inputStr;
        break;
    }
    case 0:
    { // 退出登录

        break;
    }
    default:
    {
        cerr << "无效选项!" << endl;
        return;
    }
    }

    // 发送请求到服务器
    string request = js.dump();
    if (send(clientfd, request.c_str(), request.size() + 1, 0) <= 0)
    {
        cerr << "发送请求失败" << endl;
    }
}
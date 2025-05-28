#ifndef PUBLIC_H
#define PUBLIC_H
#include <string>
// 这里修改一下 初始四个选项是teacher，student，admin,quit
// 每一种身份进去之后都有REG，LOGIN，RETURN 三种选项
// 接下来是每个人对应的操作后面细说
enum EnMsgType{
    STU_LOGIN_MSG=1,
    STU_LOGIN_MSG_ACK,
    STU_REG_MSG,
    STU_REG_MAG_ACK,
    TEA_LOGIN_MSG,
    TEA_LOGIN_MSG_ACK,
    TEA_REG_MSG,
    TEA_REG_MSG_ACK,
    AD_LOGIN_MSG,
    AD_LOGIN_MSG_ACK,
    AD_REG_MSG,
    AD_REG_MSG_ACK
};

struct CourseAverage {
    std::string courseID;      // 课程编号
    std::string courseName;    // 课程名称
    double averageScore;       // 平均成绩
};
struct StudentGrade {
    std::string studentID;     // 学生学号
    std::string studentName;   // 学生姓名
    std::string className;     // 所属班级
    double averageScore;       // 平均成绩
};


#endif
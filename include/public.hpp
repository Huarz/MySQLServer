#ifndef PUBLIC_H
#define PUBLIC_H
#include <string>
// 这里修改一下 初始四个选项是teacher，student，admin,quit
// 每一种身份进去之后都有REG，LOGIN，RETURN 三种选项
// 接下来是每个人对应的操作后面细说
enum EnMsgType {
    STU_LOGIN_MSG = 1,
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
    AD_REG_MSG_ACK,

    STU_QUERY_COURSE,              // 13
    STU_QUERY_COURSE_ACK,
    STU_QUERY_ALL_COURSE,
    STU_QUERY_ALL_COURSE_ACK,

    STU_QUERY_COURSE_CHOOSING,     // 17
    STU_QUERY_COURSE_CHOOSING_ACK,
    STU_QUERY_GRADE,
    STU_QUERY_GRADE_ACK,

    TEA_UPDATE_SCORE,              // 21
    TEA_UPDATE_SCORE_ACK,
    TEA_QUERY_COURSE_CHOOSING,
    TEA_QUERY_COURSE_CHOOSING_ACK,

    AD_INSERT_COURSE,     // 25
    AD_INSERT_COURSE_ACK,
    AD_QUERY_COURSE,
    AD_QUERY_COURSE_ACK,
    AD_QUERY_ALL_COURSE,
    AD_QUERY_ALL_COURSE_ACK,
    AD_REMOVE_COURSE,
    AD_REMOVE_COURSE_ACK,

    AD_INSERT_CHOOSING,              
    AD_INSERT_CHOOSING_ACK,
    AD_QUERY_CHOOSING,
    AD_QUERY_CHOOSING_ACK,
    AD_QUERY_ALL_CHOOSING,
    AD_QUERY_ALL_CHOOSING_ACK,
    AD_REMOVE_CHOOSING,
    AD_REMOVE_CHOOSING_ACK,

    AD_QUERY_STUDENT_GRADES,       
    AD_QUERY_STUDENT_GRADES_ACK,
    AD_QUERY_ALL_STUDENT_GRADES,
    AD_QUERY_ALL_STUDENT_GRADES_ACK,
    AD_QUERY_CLASS_GRADES,
    AD_QUERY_CLASS_GRADES_ACK,
    AD_QUERY_COURSES_AVERAGE,
    AD_QUERY_COURSES_AVERAGE_ACK,
    AD_QUERY_CLASS_COURSES_AVERAGE,
    AD_QUERY_CLASS_COURSES_AVERAGE_ACK
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
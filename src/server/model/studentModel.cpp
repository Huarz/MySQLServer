#include "studentModel.hpp"
#include "ConnectionPool.hpp"
#include <iostream>
#include <memory>
using namespace std;

bool StudentModel::insert(Student &stu)
{
    char sql[1024] = {0};
    sprintf(sql, "INSERT INTO Students ( studentName, password,gender, entranceAge, entranceYear, stuClass,State) "
                 "VALUES ( '%s', '%s','%s', %d, %d, '%s','%s')",
            stu.getStudentName().c_str(),
            stu.getPassword().c_str(),
            stu.getGender() == Gender::male ? "male" : "female",
            stu.getEnteranceAge(),
            stu.getEnteranceYear(),
            stu.getClass().c_str(),
            stu.getState().c_str());

    shared_ptr<Connection> conn = ConnectionPool::getConnectionPool()->getConnection();

    if (conn != nullptr)
    {
        if (conn->update(sql))
        {
            stu.setStudentID(to_string(mysql_insert_id(conn->getConnection())));
            return true;
        }
    }
    return false;
}

Student StudentModel::query(string id)
{
    char sql[1024] = {0};
    sprintf(sql, "SELECT * FROM Students WHERE StudentID = %s", id.c_str());

    shared_ptr<Connection> conn = ConnectionPool::getConnectionPool()->getConnection();
    if (conn != nullptr)
    {
        MYSQL_RES *result = conn->query(sql);
        if (result != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(result);
            if (row != nullptr)
            {
                Student stu;
                stu.setStudentID(row[0]);
                stu.setStudentName(row[1]);
                stu.setPassword(row[2]);
                stu.setGender(row[3][0] == 'm' ? Gender::male : Gender::female);
                stu.setEnteranceAge(atoi(row[4]));
                stu.setEnteranceYear(atoi(row[5]));
                stu.setClass(row[6]);
                stu.setState(row[7]);
                mysql_free_result(result);
                return stu;
            }
        }
    }

    return Student();
}

bool StudentModel::updateState(Student &stu)
{
    char sql[1024] = {0};
    sprintf(sql,"update Students set state='%s' where id ='%s'",stu.getState().c_str(),stu.getStudentID().c_str());

    shared_ptr<Connection> conn = ConnectionPool::getConnectionPool()->getConnection();

    if (conn != nullptr)
    {
        if (conn->update(sql))
        {
            return true;
        }
    }
    return false;
}

void StudentModel::resetState()
{
    char sql[1024] = "UPDATE Students SET state = 'offline' where state ='online'";

    shared_ptr<Connection> conn = ConnectionPool::getConnectionPool()->getConnection();

    if (conn != nullptr)
    {
        conn->update(sql);
    }
}
vector<CourseChoosing> StudentModel::queryByStudent(const Student & stu) {
    vector<CourseChoosing> vec;
    char sql[1024] = {0};
    sprintf(sql, "SELECT * FROM CourseChoosing WHERE studentID = '%s'", stu.getStudentID().c_str());

    shared_ptr<Connection> conn = ConnectionPool::getConnectionPool()->getConnection();
    if (conn) {
        MYSQL_RES *result = conn->query(sql);
        if (result) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(result))) {
                CourseChoosing cc;
                cc.setStudentID(row[0]);
                cc.setCourseID(row[1]);
                cc.setTeacherID(row[2]);
                cc.setChosenYear(atoi(row[3]));
                cc.setScore(atof(row[4]));
                vec.push_back(cc);
            }
            mysql_free_result(result);
        }
    }
    return vec;
}

CourseChoosing StudentModel::queryGrade(const Student&stu, const string& courseID) {
    char sql[1024] = {0};
    sprintf(sql, "SELECT * FROM CourseChoosing WHERE studentID = '%s' AND courseID = '%s'",
            stu.getStudentID().c_str(), courseID.c_str());

    shared_ptr<Connection> conn = ConnectionPool::getConnectionPool()->getConnection();
    if (conn) {
        MYSQL_RES *result = conn->query(sql);
        if (result) {
            MYSQL_ROW row = mysql_fetch_row(result);
            if (row) {
                CourseChoosing courseChoosing;
                courseChoosing.setStudentID(row[0]);
                courseChoosing.setCourseID(row[1]);
                courseChoosing.setTeacherID(row[2]);
                courseChoosing.setChosenYear(atoi(row[3]));
                courseChoosing.setScore(atof(row[4]));
                mysql_free_result(result);
                return courseChoosing;
            }
            mysql_free_result(result);
        }
    }
    return CourseChoosing();
}

Course StudentModel::queryCourse(string &id) {
    char sql[1024] = {0};
    sprintf(sql, "SELECT * FROM Courses WHERE courseId = '%s'", id.c_str());

    shared_ptr<Connection> conn = ConnectionPool::getConnectionPool()->getConnection();

    if (conn != nullptr) {
        MYSQL_RES *result = conn->query(sql);
        if (result != nullptr) {
            MYSQL_ROW row = mysql_fetch_row(result);
            if (row != nullptr) {
                Course course;
                course.setCourseId(row[0]);
                course.setCourseName(row[1]);
                course.setTeacherId(row[2]);
                course.setCredit(strtod(row[3], nullptr));
                course.setGrade(atoi(row[4]));
                course.setCanceledYear(atoi(row[5]));
                mysql_free_result(result);
                return course;
            }
        }
    }

    return Course();
}
vector<Course> StudentModel::queryAllCourse() {
    vector<Course> courses;
    const char* sql = "SELECT * FROM Courses";
    
    shared_ptr<Connection> conn = ConnectionPool::getConnectionPool()->getConnection();

    if (conn != nullptr) {
        MYSQL_RES* result = conn->query(sql);
        if (result != nullptr) {
            MYSQL_ROW row;
            // 遍历结果集所有行
            while ((row = mysql_fetch_row(result)) != nullptr) {
                Course course;
                // 处理可能为NULL的字段，设置默认值
                course.setCourseId(row[0] ? row[0] : "");
                course.setCourseName(row[1] ? row[1] : "");
                course.setTeacherId(row[2] ? row[2] : "");
                course.setCredit(row[3] ? strtod(row[3], nullptr) : 0.0);
                course.setGrade(row[4] ? atoi(row[4]) : 0);
                course.setCanceledYear(row[5] ? atoi(row[5]) : 0);
                
                courses.push_back(course);
            }
            mysql_free_result(result);
        }
    }
    
    return courses;
}
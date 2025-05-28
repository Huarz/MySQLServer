#include <iostream>
#include <memory>
#include "teacherModel.hpp"
#include <string>
#include "ConnectionPool.hpp"
#include <sstream>
#include <vector>
using namespace std;

bool TeacherModel::insert(Teacher &teacher) {
    char sql[1024] = {0};
    vector<string> courseIDs = teacher.getCourseIDs();
    string courses_str;
    for (size_t i = 0; i < courseIDs.size(); ++i) {
        if (i > 0) {
            courses_str += ",";
        }
        courses_str += courseIDs[i];
    }

    sprintf(sql, "INSERT INTO Teachers (teacherName, password, state, courseIDs) "
                 "VALUES ('%s', '%s', '%s', '%s')",
            teacher.getTeacherName().c_str(),
            teacher.getPassword().c_str(),
            teacher.getState().c_str(),
            courses_str.c_str());

    shared_ptr<Connection> conn = ConnectionPool::getConnectionPool()->getConnection();

    if (conn != nullptr) {
        if (conn->update(sql)) {
            teacher.setTeacherId(to_string(mysql_insert_id(conn->getConnection())));
            return true;
        }
    }
    return false;
}

Teacher TeacherModel::query(string teacherID) {
    char sql[1024] = {0};
    sprintf(sql, "SELECT * FROM Teachers WHERE teacherId = '%s'", teacherID.c_str());

    shared_ptr<Connection> conn = ConnectionPool::getConnectionPool()->getConnection();

    if (conn != nullptr) {
        MYSQL_RES *result = conn->query(sql);
        if (result != nullptr) {
            MYSQL_ROW row = mysql_fetch_row(result);
            if (row != nullptr) {
                Teacher teacher;
                teacher.setTeacherId(row[0]);
                teacher.setTeacherName(row[1]);
                teacher.setPassword(row[2]);
                teacher.setState(row[3]);

                // 解析课程 ID 列表
                vector<string> courseIDs;
                stringstream ss(row[4]);
                string courseID;
                while (getline(ss, courseID, ',')) {
                    courseIDs.push_back(courseID);
                }
                teacher.setCourseIDs(courseIDs);

                mysql_free_result(result);
                return teacher;
            }
        }
    }

    return Teacher();
}

bool TeacherModel::updateState(Teacher &teacher) {
    char sql[1024] = {0};
    sprintf(sql, "UPDATE Teachers SET state = '%s' WHERE teacherID = '%s'",
            teacher.getState().c_str(),
            teacher.getTeacherId().c_str());

    shared_ptr<Connection> conn = ConnectionPool::getConnectionPool()->getConnection();

    if (conn != nullptr) {
        if (conn->update(sql)) {
            return true;
        }
    }
    return false;
}
void TeacherModel::resetState()
{
    char sql[1024] = "UPDATE Teachers SET state = 'offline' where state ='online'";

    shared_ptr<Connection> conn = ConnectionPool::getConnectionPool()->getConnection();

    if (conn != nullptr)
    { 
        conn->update(sql);
    }
}

bool TeacherModel::updateScore(const Teacher &tea, const string &studentID,
                                     const string &courseID, double score) {
    char sql[1024] = {0};
    sprintf(sql, "UPDATE CourseChoosing SET score = %.2f WHERE studentID = '%s' AND courseID = '%s' AND teacherID = '%s'",
            score, studentID.c_str(), courseID.c_str(), tea.getTeacherId().c_str());

    shared_ptr<Connection> conn = ConnectionPool::getConnectionPool()->getConnection();
    return conn && conn->update(sql);
}
std::vector<CourseChoosing> TeacherModel::queryByTeacherAndCourse(
    const Teacher&tea, 
    const std::string& courseID) 
{
    std::vector<CourseChoosing> vec;
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), 
            "SELECT * FROM CourseChoosing WHERE teacherID = '%s' AND courseID = '%s'",
            tea.getTeacherId().c_str(),
            courseID.c_str());

    std::shared_ptr<Connection> conn = ConnectionPool::getConnectionPool()->getConnection();
    if (conn) {
        MYSQL_RES* result = conn->query(sql);
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
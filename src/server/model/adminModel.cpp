#include <iostream>
#include <memory>
#include "ConnectionPool.hpp"
#include "adminModel.hpp"
using namespace std;



bool AdminModel::insert(Admin &admin)
{
    char sql[1024] = {0};
    sprintf(sql, "INSERT INTO Admins (adminName, password, state) "
                 "VALUES ('%s', '%s', '%s')",
            admin.getAdminName().c_str(),
            admin.getPassword().c_str(),
            admin.getState().c_str());

    shared_ptr<Connection> conn = ConnectionPool::getConnectionPool()->getConnection();

    if (conn != nullptr)
    {
        if (conn->update(sql))
        {
            admin.setAdminID(to_string(mysql_insert_id(conn->getConnection())));
            return true;
        }
    }
    return false;
}

Admin AdminModel::query(string adminID)
{
    char sql[1024] = {0};
    sprintf(sql, "SELECT * FROM Admins WHERE adminID = '%s'", adminID.c_str());

    shared_ptr<Connection> conn = ConnectionPool::getConnectionPool()->getConnection();

    if (conn != nullptr)
    {
        MYSQL_RES *result = conn->query(sql);
        if (result != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(result);
            if (row != nullptr)
            {
                Admin admin;
                admin.setAdminID(row[0]);
                admin.setAdminName(row[1]);
                admin.setPassword(row[2]);
                admin.setState(row[3]);
                mysql_free_result(result);
                return admin;
            }
        }
    }

    return Admin();
}

bool AdminModel::updateState(Admin &admin)
{
    char sql[1024] = {0};
    sprintf(sql, "UPDATE Admins SET state = '%s' WHERE adminID = '%s'",
            admin.getState().c_str(),
            admin.getAdminID().c_str());

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

void AdminModel::resetState()
{
    char sql[1024] = "UPDATE Admins SET state = 'offline' where state ='online'";

    shared_ptr<Connection> conn = ConnectionPool::getConnectionPool()->getConnection();

    if (conn != nullptr)
    {
        conn->update(sql);
    }
}

bool AdminModel::insertCourse(CourseChoosing &courseChoosing) {
    char sql[1024] = {0};
    sprintf(sql, "INSERT INTO CourseChoosing (studentID, courseID, teacherID, chosenYear, score) "
                 "VALUES ('%s', '%s', '%s', %d, %.2f)",
            courseChoosing.getStudentID().c_str(),
            courseChoosing.getCourseID().c_str(),
            courseChoosing.getTeacherID().c_str(),
            courseChoosing.getChosenYear(),
            courseChoosing.getScore());

    shared_ptr<Connection> conn = ConnectionPool::getConnectionPool()->getConnection();
    return conn && conn->update(sql);
}
vector<CourseChoosing> AdminModel::queryAllCoures() {
    vector<CourseChoosing> vec;
    const char* sql = "SELECT * FROM CourseChoosing";

    shared_ptr<Connection> conn = ConnectionPool::getConnectionPool()->getConnection();
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
bool AdminModel::removeCourse(const string &studentID, const string &courseID) {
    char sql[1024] = {0};
    sprintf(sql, "DELETE FROM CourseChoosing WHERE studentID = '%s' AND courseID = '%s'",
            studentID.c_str(), courseID.c_str());

    shared_ptr<Connection> conn = ConnectionPool::getConnectionPool()->getConnection();
    return conn && conn->update(sql);
}
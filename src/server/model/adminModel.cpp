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
    sprintf(sql, "SELECT * FROM Admins WHERE adminId = '%s'", adminID.c_str());

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
    sprintf(sql, "UPDATE Admins SET state = '%s' WHERE adminId = '%s'",
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

bool AdminModel::insertCourseChoosing(CourseChoosing &courseChoosing) {
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
vector<CourseChoosing> AdminModel::queryAllCouresChoosing() {
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
bool AdminModel::removeCourseChoosing(const string &studentID, const string &courseID) {
    char sql[1024] = {0};
    sprintf(sql, "DELETE FROM CourseChoosing WHERE studentId = '%s' AND courseId = '%s'",
            studentID.c_str(), courseID.c_str());

    shared_ptr<Connection> conn = ConnectionPool::getConnectionPool()->getConnection();
    return conn && conn->update(sql);
}
vector<StudentGrade> AdminModel::queryStudentGrades(const string& studentID) {
    vector<StudentGrade> grades;
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql),
            "SELECT s.studentId, s.studentName, s.stuClass, AVG(cc.score) AS avgScore "
            "FROM Students s "
            "JOIN CourseChoosing cc ON s.studentId = cc.studentID "
            "WHERE s.studentId = '%s' "
            "GROUP BY s.studentId, s.studentName, s.stuClass",
            studentID.c_str());

    shared_ptr<Connection> conn = ConnectionPool::getConnectionPool()->getConnection();
    if (conn) {
        MYSQL_RES* result = conn->query(sql);
        if (result) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(result))) {
                StudentGrade sg;
                sg.studentID = row[0] ? row[0] : "";
                sg.studentName = row[1] ? row[1] : "";
                sg.className = row[2] ? row[2] : "";
                sg.averageScore = row[3] ? atof(row[3]) : 0.0;
                grades.push_back(sg);
            }
            mysql_free_result(result);
        }
    }
    return grades;
}

vector<StudentGrade> AdminModel::queryAllStudentsGrades() {
    vector<StudentGrade> grades;
    const char* sql = 
        "SELECT s.studentId, s.studentName, s.stuClass, AVG(cc.score) AS avgScore "
        "FROM Students s "
        "JOIN CourseChoosing cc ON s.studentId = cc.studentID "
        "GROUP BY s.studentId, s.studentName, s.stuClass";

    shared_ptr<Connection> conn = ConnectionPool::getConnectionPool()->getConnection();
    if (conn) {
        MYSQL_RES* result = conn->query(sql);
        if (result) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(result))) {
                StudentGrade sg;
                sg.studentID = row[0] ? row[0] : "";
                sg.studentName = row[1] ? row[1] : "";
                sg.className = row[2] ? row[2] : "";
                sg.averageScore = row[3] ? atof(row[3]) : 0.0;
                grades.push_back(sg);
            }
            mysql_free_result(result);
        }
    }
    return grades;
}

vector<StudentGrade> AdminModel::queryClassStudentsGrades(const string& className) {
    vector<StudentGrade> grades;
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql),
            "SELECT s.studentId, s.studentName, s.stuClass, AVG(cc.score) AS avgScore "
            "FROM Students s "
            "JOIN CourseChoosing cc ON s.studentId = cc.studentID "
            "WHERE s.stuClass = '%s' "
            "GROUP BY s.studentId, s.studentName, s.stuClass",
            className.c_str());

    shared_ptr<Connection> conn = ConnectionPool::getConnectionPool()->getConnection();
    if (conn) {
        MYSQL_RES* result = conn->query(sql);
        if (result) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(result))) {
                StudentGrade sg;
                sg.studentID = row[0] ? row[0] : "";
                sg.studentName = row[1] ? row[1] : "";
                sg.className = row[2] ? row[2] : "";
                sg.averageScore = row[3] ? atof(row[3]) : 0.0;
                grades.push_back(sg);
            }
            mysql_free_result(result);
        }
    }
    return grades;
}

vector<CourseAverage> AdminModel::queryCoursesAverageScores() {
    vector<CourseAverage> averages;
    const char* sql = 
        "SELECT cc.courseID, c.courseName, AVG(cc.score) AS avgScore "
        "FROM CourseChoosing cc "
        "JOIN Courses c ON cc.courseID = c.courseId "
        "GROUP BY cc.courseID, c.courseName";

    shared_ptr<Connection> conn = ConnectionPool::getConnectionPool()->getConnection();
    if (conn) {
        MYSQL_RES* result = conn->query(sql);
        if (result) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(result))) {
                CourseAverage ca;
                ca.courseID = row[0] ? row[0] : "";
                ca.courseName = row[1] ? row[1] : "";
                ca.averageScore = row[2] ? atof(row[2]) : 0.0;
                averages.push_back(ca);
            }
            mysql_free_result(result);
        }
    }
    return averages;
}

map<string, double> AdminModel::queryClassCoursesAverage(const string& className) {
    map<string, double> courseAverages;
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql),
            "SELECT c.courseId, AVG(cc.score) AS avgScore "
            "FROM CourseChoosing cc "
            "JOIN Students s ON cc.studentID = s.studentId "
            "JOIN Courses c ON cc.courseID = c.courseId "
            "WHERE s.stuClass = '%s' "
            "GROUP BY c.courseId",
            className.c_str());

    shared_ptr<Connection> conn = ConnectionPool::getConnectionPool()->getConnection();
    if (conn) {
        MYSQL_RES* result = conn->query(sql);
        if (result) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(result))) {
                if (row[0] && row[1]) {
                    courseAverages[row[0]] = atof(row[1]);
                }
            }
            mysql_free_result(result);
        }
    }
    return courseAverages;
}

bool AdminModel::insertCourse(Course &course) {
    char sql[1024] = {0};
    
    sprintf(sql, "INSERT INTO Courses (courseName, teacherId, credit, grade, canceledYear) "
                 "VALUES ('%s', '%s', %.2f, %d, %d)",
            course.getCourseName().c_str(),
            course.getTeacherId().c_str(),
            course.getCredit(),
            course.getGrade(),
            course.getCanceledYear());

    shared_ptr<Connection> conn = ConnectionPool::getConnectionPool()->getConnection();

    if (conn != nullptr)
    {
        if (conn->update(sql))
        {
            course.setCourseId(to_string(mysql_insert_id(conn->getConnection())));
            Teacher tea=_teacherModel.query(course.getTeacherId());
            tea.addCourseID(course.getCourseId());
            return true;
        }
    }
    return false;
}

Course AdminModel::queryCourse(const string &id) {
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
vector<Course> AdminModel::queryAllCourse() {
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

bool AdminModel::removeCourse(const string& courseId) {
    char sql[1024] = {0};
    sprintf(sql, "DELETE FROM Courses WHERE courseId = '%s'", courseId.c_str());

    shared_ptr<Connection> conn = ConnectionPool::getConnectionPool()->getConnection();
    
    if (conn != nullptr) {
        Course course=queryCourse(courseId);
        if (conn->update(sql)) {
            Teacher tea=_teacherModel.query(course.getTeacherId());
            tea.removeCourseID(courseId);
            return true;
        }
    }
    return false;
}
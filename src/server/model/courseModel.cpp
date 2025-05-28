#include "courseModel.hpp"
#include <cstring>
#include "ConnectionPool.hpp"
#include "teacher.hpp"

using namespace std;

bool CourseModel::insert(Course &course) {
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

Course CourseModel::query(string &id) {
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

bool CourseModel::remove(string& courseId) {
    char sql[1024] = {0};
    sprintf(sql, "DELETE FROM Courses WHERE courseId = '%s'", courseId.c_str());

    shared_ptr<Connection> conn = ConnectionPool::getConnectionPool()->getConnection();
    
    if (conn != nullptr) {
        Course course=query(courseId);
        if (conn->update(sql)) {
            Teacher tea=_teacherModel.query(course.getTeacherId());
            tea.removeCourseID(courseId);
            return true;
        }
    }
    return false;
}
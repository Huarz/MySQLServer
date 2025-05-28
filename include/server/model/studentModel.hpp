#ifndef STUDENTMODEL_H
#define STUDENTMODEL_H
#include "student.hpp"
#include "courseChoosing.hpp"
#include "course.hpp"
#include <vector>


class StudentModel{
public:
    // student表的增加方法
    bool insert(Student &stu);
    // 根据Id查询学生信息
    Student query(string  id);
    // 更新学生信息
    bool updateState( Student &stu);
    void resetState();

    Course queryCourse(string &id);
    vector<Course> queryAllCourse();
    vector<CourseChoosing> queryByStudent(const Student &stu);
    CourseChoosing queryGrade(const Student&stu, const string& courseID);
};
#endif
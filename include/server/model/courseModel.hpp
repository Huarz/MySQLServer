#ifndef COURSEMODEL_H
#define COURSEMODEL_H
#include "course.hpp"
#include "teacherModel.hpp"


class CourseModel{
public:
    bool insert(Course& _courese);
    Course query(string &id);
    bool remove(string&id);
private:
    TeacherModel _teacherModel;
   
};
#endif
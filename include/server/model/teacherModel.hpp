#ifndef TEACHERMODEL_H
#define TEACHERMODEL_H
#include <iostream>
#include <memory>
#include "courseChoosing.hpp"
#include "teacher.hpp"
using namespace std;

class TeacherModel
{
public:
    bool insert(Teacher &teacher);
    Teacher query(string teacherID);
    bool updateState(Teacher &teacher);
    void resetState();
    bool updateScore(const Teacher &tea, const string &studentID,
                     const string &courseID, double score);
    std::vector<CourseChoosing> queryByTeacherAndCourse(
        const Teacher &tea,
        const std::string &courseID);
};
#endif

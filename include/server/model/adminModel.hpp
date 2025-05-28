#ifndef ADMINMODEL_H
#define ADMINMODEL_H
#include <string>
#include "admin.hpp"
#include "courseChoosing.hpp"
#include "course.hpp"
#include <vector>
#include "../../public.hpp"
#include "teacherModel.hpp"
#include <map>

class AdminModel
{
public:
    bool insert(Admin &ad);
    Admin query(string id);
    bool updateState(Admin &ad);
    void resetState();
    bool insertCourseChoosing(CourseChoosing &courseChoosing);
    vector<CourseChoosing> queryAllCouresChoosing();
    bool removeCourseChoosing(const string &studentID, const string &courseID);
    bool insertCourse(Course &_courese);
    Course queryCourse(const string &id);
    vector<Course> queryAllCourse();
    bool removeCourse(const string &id);
    vector<StudentGrade> queryStudentGrades(const string &studentID);
    vector<StudentGrade> queryAllStudentsGrades();
    vector<StudentGrade> queryClassStudentsGrades(const string& className);
    vector<CourseAverage> queryCoursesAverageScores();
    map<string, double> queryClassCoursesAverage(const string &className);

private:
    TeacherModel _teacherModel;
};
#endif
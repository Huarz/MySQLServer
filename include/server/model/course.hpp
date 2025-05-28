#ifndef COURSE_H
#define COURSE_H
#include <string>

using namespace std;

class Course
{
private:
    string courseId;
    string courseName;
    string teacherId;
    double credit;
    int grade;
    int canceledYear;

public:
    // 构造函数
    Course(string Id = "", string name = "", string tId = "", double cr = 0.0, int g = 0, int cy = 0)
        : courseId(Id), courseName(name), teacherId(tId), credit(cr), grade(g), canceledYear(cy) {}

    // 析构函数
    ~Course() {}

    // Getter 方法
    string getCourseId() const
    {
        return courseId;
    }
    string getCourseName() const
    {
        return courseName;
    }
    string getTeacherId() const
    {
        return teacherId;
    }
    double getCredit() const
    {
        return credit;
    }
    int getGrade() const
    {
        return grade;
    }
    int getCanceledYear() const
    {
        return canceledYear;
    }

    // Setter 方法
    void setCourseId(string Id)
    {
        courseId = Id;
    }
    void setCourseName(string name)
    {
        courseName = name;
    }
    void setTeacherId(string tId)
    {
        teacherId = tId;
    }
    void setCredit(double cr)
    {
        credit = cr;
    }
    void setGrade(int g)
    {
        grade = g;
    }
    void setCanceledYear(int cy)
    {
        canceledYear = cy;
    }
};

#endif
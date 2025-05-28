#ifndef COURSECHOOSING_H
#define COURSECHOOSING_H
#include <string>

using namespace std;

class CourseChoosing
{
private:
    string studentID;
    string courseID;
    string teacherID;
    int chosenYear;
    double score;

public:
    // 构造函数
    CourseChoosing(string sid = "", string cid = "", string tid = "", int year = 0, double sc = 0.0)
        : studentID(sid), courseID(cid), teacherID(tid), chosenYear(year), score(sc) {}

    // 析构函数
    ~CourseChoosing() {}

    // Getter 方法
    string getStudentID() const
    {
        return studentID;
    }
    string getCourseID() const
    {
        return courseID;
    }
    string getTeacherID() const
    {
        return teacherID;
    }
    int getChosenYear() const
    {
        return chosenYear;
    }
    double getScore() const
    {
        return score;
    }

    // Setter 方法
    void setStudentID(string sid)
    {
        studentID = sid;
    }
    void setCourseID(string cid)
    {
        courseID = cid;
    }
    void setTeacherID(string tid)
    {
        teacherID = tid;
    }
    void setChosenYear(int year)
    {
        chosenYear = year;
    }
    void setScore(double sc)
    {
        score = sc;
    }
};

#endif
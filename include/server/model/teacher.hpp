#ifndef TEACHER_H
#define TEACHER_H
#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Teacher
{
private:
    string teacherId;
    string teacherName;
    string password;
    string state;
    vector<string> courseIDs;

public:
    Teacher(string id = "", string name = "", string pwd = "", string _state = "offline")
        : teacherId(id), teacherName(name), password(pwd), state(_state) {}

    ~Teacher() {}

    // Getter 和 Setter 方法
    string getTeacherId() const
    {
        return teacherId;
    }
    void setTeacherId(string id)
    {
        teacherId = id;
    }

    string getTeacherName() const
    {
        return teacherName;
    }
    void setTeacherName(string name)
    {
        teacherName = name;
    }

    string getPassword() const
    {
        return password;
    }
    void setPassword(string pwd)
    {
        password = pwd;
    }
    string getState() const
    {
        return state;
    }
    void setState(string _state)
    {
        state = _state;
    }

    vector<string> getCourseIDs() const
    {
        return courseIDs;
    }
    void setCourseIDs(const vector<string> &crsIDs)
    {
        courseIDs = crsIDs;
    }
    // 增加教授课程
    void addCourseID(const string &courseID)
    {
        courseIDs.push_back(courseID);
    }

    // 减少教授课程
    void removeCourseID(const string &courseID)
    {
        for (auto it = courseIDs.begin(); it != courseIDs.end(); ++it)
        {
            if (*it == courseID)
            {
                courseIDs.erase(it);
                break;
            }
        }
    }
};

#endif
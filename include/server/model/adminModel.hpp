#ifndef ADMINMODEL_H
#define ADMINMODEL_H
#include "admin.hpp"
#include "courseChoosing.hpp"
#include "vector"

class AdminModel{
public:
    bool insert(Admin& ad);
    Admin query(string id);
    bool updateState(Admin&ad);
    void resetState();
    bool insertCourse(CourseChoosing &courseChoosing);
    vector<CourseChoosing> queryAllCoures(); 
    bool removeCourse(const string &studentID, const string &courseID);
};
#endif
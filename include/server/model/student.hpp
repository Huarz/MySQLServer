#ifndef STUDENT_H
#define STUDENT_H

#include <iostream>
#include <string>
using namespace std;

enum Gender {
    male,
    female
};

class Student {
public:
    // 构造函数
    Student(string id = "", string name = "",string pwd="", Gender gender = male, int age = -1, int year = -1, string className = "", string state = "offline")
        : studentId(id),password(pwd) ,studentName(name), gender(gender), enteranceAge(age), enteranceYear(year), stuClass(className), state(state) {}

    // 析构函数
    ~Student() {}

    // Getter方法
    string getStudentID() const {
        return studentId;
    }

    string getStudentName() const {
        return studentName;
    }

    Gender getGender() const {
        return gender;
    }

    int getEnteranceAge() const {
        return enteranceAge;
    }

    int getEnteranceYear() const {
        return enteranceYear;
    }

    string getClass() const {
        return stuClass;
    }

    string getState() const {
        return state;
    }
    string getPassword()const{
        return password;
    }

    // Setter方法
    void setStudentID(const string &_studentId) {
        studentId = _studentId;
    }

    void setStudentName(const string &_studentName) {
        studentName = _studentName;
    }

    void setGender(Gender _gender) {
        gender = _gender;
    }

    void setEnteranceAge(int _enteranceAge) {
        enteranceAge = _enteranceAge;
    }

    void setEnteranceYear(int _enteranceYear) {
        enteranceYear = _enteranceYear;
    }

    void setClass(const string &_stuClass) {
        stuClass = _stuClass;
    }

    void setState(const string &_state) {
        state = _state;
    }
    void setPassword(const string &pwd){
        password=pwd;
    }

private:
    string studentId;
    string studentName;
    Gender gender;
    int enteranceAge;
    int enteranceYear;
    string stuClass;
    string state; 
    string password;
};

#endif
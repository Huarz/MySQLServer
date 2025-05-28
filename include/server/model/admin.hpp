#ifndef ADMIN_H
#define ADMIN_H
#include <string>

using namespace std;

class Admin
{
private:
    string adminId;
    string adminName;
    string password;
    string state;

public:
    Admin(string id = "", string name = "", string pwd = "", string _state = "offline")
        : adminId(id), adminName(name), password(pwd), state(_state) {}

    ~Admin() {}

    string getAdminID() const
    {
        return adminId;
    }
    void setAdminID(string id)
    {
        adminId = id;
    }

    string getAdminName() const
    {
        return adminName;
    }
    void setAdminName(string name)
    {
        adminName = name;
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
};

#endif
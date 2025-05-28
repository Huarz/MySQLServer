#include "SQLService.hpp"
#include "public.hpp"
#include <muduo/base/Logging.h>
using namespace std::placeholders;

using namespace muduo;
SQLService *SQLService::getInstance()
{
    static SQLService service;
    return &service;
}

SQLService::SQLService()
{
    // 学生相关消息处理
    _msgHandlerMap.insert({STU_LOGIN_MSG, std::bind(&SQLService::stuLogin, this, _1, _2, _3)});
    _msgHandlerMap.insert({STU_REG_MSG, std::bind(&SQLService::stuReg, this, _1, _2, _3)});
    _msgHandlerMap.insert({STU_QUERY_COURSE, std::bind(&SQLService::stuQueryCourse, this, _1, _2, _3)});
    _msgHandlerMap.insert({STU_QUERY_ALL_COURSE, std::bind(&SQLService::stuQueryAllCourse, this, _1, _2, _3)});
    _msgHandlerMap.insert({STU_QUERY_COURSE_CHOOSING, std::bind(&SQLService::stuQueryCourseChoosing, this, _1, _2, _3)});
    _msgHandlerMap.insert({STU_QUERY_GRADE, std::bind(&SQLService::stuQueryGrade, this, _1, _2, _3)});

    // 教师相关消息处理
    _msgHandlerMap.insert({TEA_LOGIN_MSG, std::bind(&SQLService::teaLogin, this, _1, _2, _3)});
    _msgHandlerMap.insert({TEA_REG_MSG, std::bind(&SQLService::teaReg, this, _1, _2, _3)});
    _msgHandlerMap.insert({TEA_UPDATE_SCORE, std::bind(&SQLService::teaUpdateScore, this, _1, _2, _3)});
    _msgHandlerMap.insert({TEA_QUERY_COURSE_CHOOSING, std::bind(&SQLService::teaQueryCourseChoosing, this, _1, _2, _3)});

    // 管理员相关消息处理
    _msgHandlerMap.insert({AD_LOGIN_MSG, std::bind(&SQLService::adminLogin, this, _1, _2, _3)});
    _msgHandlerMap.insert({AD_REG_MSG, std::bind(&SQLService::adminReg, this, _1, _2, _3)});
    _msgHandlerMap.insert({AD_INSERT_COURSE, std::bind(&SQLService::adminInsertCourse, this, _1, _2, _3)});
    _msgHandlerMap.insert({AD_QUERY_COURSE, std::bind(&SQLService::adminQueryCourse, this, _1, _2, _3)});
    _msgHandlerMap.insert({AD_QUERY_ALL_COURSE, std::bind(&SQLService::adminQueryAllCourse, this, _1, _2, _3)});
    _msgHandlerMap.insert({AD_REMOVE_COURSE, std::bind(&SQLService::adminRemoveCourse, this, _1, _2, _3)});

    _msgHandlerMap.insert({AD_INSERT_CHOOSING, std::bind(&SQLService::adminInsertCourseChoosing, this, _1, _2, _3)});
    _msgHandlerMap.insert({AD_QUERY_ALL_CHOOSING, std::bind(&SQLService::adminQueryAllCourseChoosing, this, _1, _2, _3)});
    _msgHandlerMap.insert({AD_REMOVE_CHOOSING, std::bind(&SQLService::adminRemoveCourseChoosing, this, _1, _2, _3)});

    _msgHandlerMap.insert({AD_QUERY_STUDENT_GRADES, std::bind(&SQLService::adminQueryStudentGrades, this, _1, _2, _3)});
    _msgHandlerMap.insert({AD_QUERY_ALL_STUDENT_GRADES, std::bind(&SQLService::adminQueryAllStudentGrades, this, _1, _2, _3)});
    _msgHandlerMap.insert({AD_QUERY_CLASS_GRADES, std::bind(&SQLService::adminQueryClassStudentsGrades, this, _1, _2, _3)});
    _msgHandlerMap.insert({AD_QUERY_COURSES_AVERAGE, std::bind(&SQLService::adminQueryCoursesAverageScores, this, _1, _2, _3)});
    _msgHandlerMap.insert({AD_QUERY_CLASS_COURSES_AVERAGE, std::bind(&SQLService::adminQueryClassCoursesAverage, this, _1, _2, _3)});
}


MsgHandler SQLService::getHandler(int msgid)
{
    auto it = _msgHandlerMap.find(msgid);
    if (it == _msgHandlerMap.end())
    {
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp time)
        {
            LOG_ERROR << "msgid " << msgid << " can not find handler";
        };
    }
    else
    {
        return _msgHandlerMap[msgid];
    }
}

void SQLService::stuLogin(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    string id = js["id"];
    string pwd = js["password"];
    Student stu = _stuModel.query(id);
    if (stu.getStudentID() == id && stu.getPassword() == pwd)
    {
        if (stu.getState() == "online")
        {
            json response;
            response["msgid"] = STU_LOGIN_MSG_ACK;
            response["errno"] = 1;
            response["errmsg"] = "该学生已登录";
            conn->send(response.dump());
        }
        else
        {
            {
                lock_guard<mutex> lock(_connMutex);
                _stuConnMap.insert({id, conn});
            }
            json response;
            response["msgid"] = STU_LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["studentId"] = stu.getStudentID();
            response["gender"] = stu.getGender();
            response["enteranceAge"] = stu.getEnteranceAge();
            response["enteranceYear"] = stu.getEnteranceYear();
            response["stuClass"] = stu.getClass();
            stu.setState("online");
            _stuModel.updateState(stu);
            conn->send(response.dump());
        }
    }
    else
    {
        json response;
        response["msgid"] = STU_LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "学生ID错误或者密码错误";
        conn->send(response.dump());
    }
}

void SQLService::stuReg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    Student stu;
    stu.setStudentName(js["StudentName"]);
    stu.setPassword(js["password"]);
    stu.setGender(js["gender"] == "male" ? Gender::male : Gender::female);
    stu.setEnteranceAge(js["entranceAge"].get<int>());
    stu.setEnteranceYear(js["entranceYear"].get<int>());
    stu.setClass(js["class"]);
    stu.setState("offline");

    bool state = _stuModel.insert(stu);

    if (state)
    {
        json response;
        response["msgid"] = STU_REG_MAG_ACK;
        response["errno"] = 0;
        response["id"] = stu.getStudentID();
        conn->send(response.dump());
    }
    else
    {
        json response;
        response["msgid"] = STU_REG_MAG_ACK;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}

void SQLService::teaLogin(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    string id = js["id"];
    string pwd = js["password"];
    Teacher teacher = _teaModel.query(id);
    if (teacher.getTeacherId() == id && teacher.getPassword() == pwd)
    {
        if (teacher.getState() == "online")
        {
            json response;
            response["msgid"] = TEA_LOGIN_MSG_ACK;
            response["errno"] = 1;
            response["errmsg"] = "该教师已登录";
            conn->send(response.dump());
        }
        else
        {
            {
                lock_guard<mutex> lock(_connMutex);
                _teaConnMap.insert({id, conn});
            }
            json response;
            response["msgid"] = TEA_LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["teacherId"] = teacher.getTeacherId();
            response["teacherName"] = teacher.getTeacherName();
            response["courseIds"] = teacher.getCourseIDs();
            teacher.setState("online");
            _teaModel.updateState(teacher);
            conn->send(response.dump());
        }
    }
    else
    {
        json response;
        response["msgid"] = TEA_LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "教师ID错误或者密码错误";
        conn->send(response.dump());
    }
}

void SQLService::teaReg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    Teacher teacher;
    teacher.setTeacherName(js["TeacherName"]);
    teacher.setPassword(js["password"]);
    teacher.setCourseIDs(js["courseIds"]);
    teacher.setState("offline");

    bool state = _teaModel.insert(teacher);

    if (state)
    {
        json response;
        response["msgid"] = TEA_REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = teacher.getTeacherId();
        conn->send(response.dump());
    }
    else
    {
        json response;
        response["msgid"] = TEA_REG_MSG_ACK;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}

void SQLService::adminLogin(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    string id = js["id"];
    string pwd = js["password"];
    Admin admin = _adminModel.query(id);
    if (admin.getAdminID() == id && admin.getPassword() == pwd)
    {
        if (admin.getState() == "online")
        {
            json response;
            response["msgid"] = AD_LOGIN_MSG_ACK;
            response["errno"] = 1;
            response["errmsg"] = "该管理员已登录";
            conn->send(response.dump());
        }
        else
        {
            {
                lock_guard<mutex> lock(_connMutex);
                _adminConnMap.insert({id, conn});
            }
            json response;
            response["msgid"] = AD_LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["adminId"] = admin.getAdminID();
            response["adminName"] = admin.getAdminName();
            admin.setState("online");
            _adminModel.updateState(admin);
            conn->send(response.dump());
        }
    }
    else
    {

        json response;
        response["msgid"] = AD_LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "管理员ID错误或者密码错误";
        conn->send(response.dump());
    }
}

void SQLService::adminReg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    Admin admin;
    admin.setAdminName(js["AdminName"]);
    admin.setPassword(js["password"]);
    admin.setState("offline");

    bool state = _adminModel.insert(admin);

    if (state)
    {
        json response;
        response["msgid"] = AD_REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = admin.getAdminID();
        conn->send(response.dump());
    }
    else
    {
        json response;
        response["msgid"] = AD_REG_MSG_ACK;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}

void SQLService::stuClientCloseException(const TcpConnectionPtr &conn)
{
    Student stu;
    {
        lock_guard<mutex> lock(_connMutex);
        for (auto it = _stuConnMap.begin(); it != _stuConnMap.end(); ++it)
        {
            if (it->second == conn)
            {
                stu.setStudentID(it->first);
                _stuConnMap.erase(it);
                break;
            }
        }
    }
    stu.setState("offline");
    _stuModel.updateState(stu);
}

void SQLService::teaClientCloseException(const TcpConnectionPtr &conn)
{
    Teacher tea;
    {
        lock_guard<mutex> lock(_connMutex);
        for (auto it = _teaConnMap.begin(); it != _teaConnMap.end(); ++it)
        {
            if (it->second == conn)
            {
                tea.setTeacherId(it->first);
                _teaConnMap.erase(it);
                break;
            }
        }
    }
    tea.setState("offline");
    _teaModel.updateState(tea);
}

void SQLService::adminClientCloseException(const TcpConnectionPtr &conn)
{
    Admin admin;
    {
        lock_guard<mutex> lock(_connMutex);
        for (auto it = _adminConnMap.begin(); it != _adminConnMap.end(); ++it)
        {
            if (it->second == conn)
            {
                admin.setAdminID(it->first);
                _adminConnMap.erase(it);
                break;
            }
        }
    }
    admin.setState("offline");
    _adminModel.updateState(admin);
}

void SQLService::reset()
{
    _stuModel.resetState();
    _teaModel.resetState();
    _adminModel.resetState();
}

void SQLService::stuQueryCourse(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    string courseId = js["courseId"];
    Course course = _stuModel.queryCourse(courseId);

    json response;
    response["msgid"] = STU_QUERY_COURSE_ACK;

    if (!course.getCourseId().empty())
    {
        response["errno"] = 0;
        response["courseId"] = course.getCourseId();
        response["courseName"] = course.getCourseName();
        response["teacherId"] = course.getTeacherId();
        response["credit"] = course.getCredit();
        response["grade"] = course.getGrade();
        response["canceledYear"] = course.getCanceledYear();
    }
    else
    {
        response["errno"] = 1;
        response["errmsg"] = "未找到任何课程信息";
    }

    conn->send(response.dump());
}
void SQLService::stuQueryAllCourse(const TcpConnectionPtr &conn, json &js, Timestamp time)
{

    vector<Course> courses = _stuModel.queryAllCourse();

    json response;
    response["msgid"] = STU_QUERY_ALL_COURSE_ACK;

    if (courses.empty())
    {
        response["errno"] = 2;
        response["errmsg"] = "未找到任何课程信息";
    }
    else
    {
        response["errno"] = 0;
        response["errmsg"] = "查询成功";

        // 将课程列表转换为JSON数组
        json courseArray = json::array();
        for (const auto &course : courses)
        {
            json courseJson;
            courseJson["courseID"] = course.getCourseId();
            courseJson["courseName"] = course.getCourseName();
            courseJson["teacherID"] = course.getTeacherId();
            courseJson["credit"] = course.getCredit();
            courseJson["grade"] = course.getGrade();
            courseJson["canceledYear"] = course.getCanceledYear();
            courseArray.push_back(courseJson);
        }
        response["courses"] = courseArray;
    }

    conn->send(response.dump());
}
void SQLService::stuQueryCourseChoosing(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    Student stu;
    stu.setStudentID(js["studentID"]);

    vector<CourseChoosing> vec = _stuModel.queryByStudent(stu);

    json response;
    response["msgid"] = STU_QUERY_COURSE_CHOOSING_ACK;

    if (!vec.empty())
    {
        response["errno"] = 0;
        json courseChoosings = json::array();
        for (const auto &cc : vec)
        {
            json item;
            item["courseID"] = cc.getCourseID();
            item["teacherID"] = cc.getTeacherID();
            item["chosenYear"] = cc.getChosenYear();
            item["score"] = cc.getScore();
            courseChoosings.push_back(item);
        }
        response["courseChoosings"] = courseChoosings;
    }
    else
    {
        response["errno"] = 1;
        response["errmsg"] = "未找到任何选课信息";
    }

    conn->send(response.dump());
}

void SQLService::stuQueryGrade(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    string studentID = js["studentID"];
    string courseID = js["courseID"];

    Student stu;
    stu.setStudentID(studentID);

    CourseChoosing courseChoosing = _stuModel.queryGrade(stu, courseID);

    json response;
    response["msgid"] = STU_QUERY_GRADE_ACK;

    if (!courseChoosing.getCourseID().empty() &&
        courseChoosing.getStudentID() == studentID)
    {
        response["errno"] = 0;
        response["errmsg"] = "success";
        response["courseID"] = courseChoosing.getCourseID();
        response["teacherID"] = courseChoosing.getTeacherID();
        response["chosenYear"] = courseChoosing.getChosenYear();
        response["score"] = courseChoosing.getScore();
    }
    else
    {
        response["errno"] = 1;
        response["errmsg"] = "查询成绩失败";
    }

    conn->send(response.dump());
}
void SQLService::teaUpdateScore(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    Teacher tea;
    tea.setTeacherId(js["teacherId"]);  // 从JSON获取教师ID
    string studentID = js["studentID"];
    string courseID = js["courseID"];
    double score = js["score"];

    bool state = _teaModel.updateScore(tea, studentID, courseID, score);

    json response;
    response["msgid"] = TEA_UPDATE_SCORE_ACK;
    response["errno"] = state ? 0 : 1;
    response["errmsg"] = state ? "更新成绩成功" : "更新成绩失败";

    // 返回更新后的关键信息
    if(state) {
        response["studentID"] = studentID;
        response["courseID"] = courseID;
        response["newScore"] = score;
    }

    conn->send(response.dump());
}
void SQLService::teaQueryCourseChoosing(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    string teacherID = js["id"];
    string courseID = js["courseID"];

    


    std::vector<CourseChoosing> courseList = _teaModel.queryByTeacherAndCourse(teacherID, courseID);
    json response;
    response["msgid"] = TEA_QUERY_COURSE_CHOOSING_ACK;
    
    if (!courseList.empty())
    {
        response["errno"] = 0;
        response["errmsg"] = "查询成功";
        json dataArr = json::array();
        
        // 遍历查询结果填充数据
        for (const auto &course : courseList)
        {
            json item;
            item["studentID"] = course.getStudentID();
            item["courseID"] = course.getCourseID();
            item["teacherID"] = course.getTeacherID();
            item["chosenYear"] = course.getChosenYear();
            item["score"] = course.getScore();
            dataArr.push_back(item);
        }
        response["data"] = dataArr;
    }
    else
    {
        response["errno"] = 1;
        response["errmsg"] = "未找到相关选课记录";
    }

    conn->send(response.dump());
}
void SQLService::adminInsertCourseChoosing(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    CourseChoosing courseChoosing;
    courseChoosing.setStudentID(js["studentID"]);
    courseChoosing.setCourseID(js["courseID"]);
    courseChoosing.setTeacherID(js["teacherID"]);
    courseChoosing.setChosenYear(js["chosenYear"]);
    courseChoosing.setScore(js["score"]);

    bool state = _adminModel.insertCourseChoosing(courseChoosing);

    json response;
    response["msgid"] = AD_INSERT_CHOOSING_ACK;
    response["errno"] = state ? 0 : 1;
    response["errmsg"] = state ? "添加成功" : "添加失败";

    // 成功时返回完整的选课信息
    if(state) {
        response["studentID"] = courseChoosing.getStudentID();
        response["courseID"] = courseChoosing.getCourseID();
        response["teacherID"] = courseChoosing.getTeacherID();
        response["chosenYear"] = courseChoosing.getChosenYear();
        response["score"] = courseChoosing.getScore();
    }

    conn->send(response.dump());
}

void SQLService::adminQueryAllCourseChoosing(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    // 直接查询所有选课记录
    std::vector<CourseChoosing> courseList = _adminModel.queryAllCouresChoosing();

    // 构造响应报文
    json response;
    response["msgid"] = AD_QUERY_ALL_CHOOSING_ACK;
    
    if (!courseList.empty())
    {
        response["errno"] = 0;
        response["errmsg"] = "查询成功";
        json dataArr = json::array();
        
        // 遍历所有选课记录
        for (const auto &course : courseList)
        {
            json item;
            item["studentID"] = course.getStudentID();
            item["courseID"] = course.getCourseID();
            item["teacherID"] = course.getTeacherID();
            item["chosenYear"] = course.getChosenYear();
            item["score"] = course.getScore();
            dataArr.push_back(item);
        }
        response["data"] = dataArr;
    }
    else
    {
        response["errno"] = 1;
        response["errmsg"] = "选课记录为空或查询失败";
    }

    conn->send(response.dump());
}
void SQLService::adminRemoveCourseChoosing(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    string studentID = js["studentID"];
    string courseID = js["courseID"];

    bool state = _adminModel.removeCourseChoosing(studentID, courseID);

    json response;
    response["msgid"] = AD_REMOVE_CHOOSING_ACK;
    
    if(state) 
    {
        // 仅在删除成功时返回记录标识
        response["errno"] = 0;
        response["errmsg"] = "删除成功";
        response["studentID"] = studentID;
        response["courseID"] = courseID;
    }
    else 
    {
        // 失败时不暴露具体ID信息
        response["errno"] = 1;
        response["errmsg"] = "删除失败"; 
    }

    conn->send(response.dump());
}
void SQLService::adminInsertCourse(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    Course course;
    // 从JSON解析课程信息
    course.setCourseName(js["courseName"]);
    course.setTeacherId(js["teacherId"]);
    course.setCredit(js["credit"].get<double>());
    course.setGrade(js["grade"].get<int>());
    course.setCanceledYear(js["canceledYear"].get<int>());

    json response;
    response["msgid"] = AD_INSERT_COURSE_ACK;

    // 调用管理员模型插入课程
    if (_adminModel.insertCourse(course))
    {
        response["errno"] = 0;
        response["courseId"] = course.getCourseId();  // 自动生成的课程ID
        response["errmsg"] = "课程添加成功";
    }
    else
    {
        response["errno"] = 1;
        response["errmsg"] = "课程添加失败";
    }

    conn->send(response.dump());
}



void SQLService::adminQueryAllCourse(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    // 查询所有课程信息
    std::vector<Course> courseList = _adminModel.queryAllCourse();

    // 构造响应报文
    json response;
    response["msgid"] = AD_QUERY_ALL_COURSE_ACK;
    
    if (!courseList.empty())
    {
        response["errno"] = 0;
        response["errmsg"] = "查询成功";
        json dataArr = json::array();
        
        // 遍历课程数据
        for (const auto &course : courseList)
        {
            json item;
            item["courseId"] = course.getCourseId();
            item["courseName"] = course.getCourseName();
            item["teacherId"] = course.getTeacherId();
            item["credit"] = course.getCredit();
            item["grade"] = course.getGrade();
            item["canceledYear"] = course.getCanceledYear();
            dataArr.push_back(item);
        }
        response["data"] = dataArr;
    }
    else
    {
        response["errno"] = 1;
        response["errmsg"] = "课程列表为空或查询失败";
    }

    conn->send(response.dump());
}
void SQLService::adminQueryCourse(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    string courseID = js["courseId"];  // 从JSON获取课程ID
    
    Course course = _adminModel.queryCourse(courseID);

    json response;
    response["msgid"] = AD_QUERY_COURSE_ACK;

    if (!course.getCourseId().empty())
    {
        response["errno"] = 0;
        response["errmsg"] = "success";
        response["courseId"] = course.getCourseId();
        response["courseName"] = course.getCourseName();
        response["teacherId"] = course.getTeacherId();
        response["credit"] = course.getCredit();
        response["grade"] = course.getGrade();
        response["canceledYear"] = course.getCanceledYear();
    }
    else
    {
        response["errno"] = 1;
        response["errmsg"] = "课程信息不存在";
    }

    conn->send(response.dump());
}

void SQLService::adminQueryStudentGrades(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    string studentID = js["studentID"];
    
    // 查询学生成绩数据
    vector<StudentGrade> gradeList = _adminModel.queryStudentGrades(studentID);

    // 构造响应报文
    json response;
    response["msgid"] = AD_QUERY_STUDENT_GRADES_ACK;

    if (!gradeList.empty())
    {
        response["errno"] = 0;
        response["errmsg"] = "查询成功";
        json dataArr = json::array();
        
        // 遍历查询结果
        for (const auto &grade : gradeList)
        {
            json item;
            item["studentID"] = grade.studentID;
            item["studentName"] = grade.studentName;
            item["className"] = grade.className;
            item["averageScore"] = grade.averageScore;
            dataArr.push_back(item);
        }
        response["data"] = dataArr;
    }
    else
    {
        response["errno"] = 1;
        response["errmsg"] = "未找到该学生的成绩记录";
    }

    conn->send(response.dump());
}

void SQLService::adminQueryAllStudentGrades(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    // 调用模型层获取所有学生成绩数据
    vector<StudentGrade> grades = _adminModel.queryAllStudentsGrades();

    json response;
    response["msgid"] = AD_QUERY_ALL_STUDENT_GRADES_ACK;

    if (!grades.empty())
    {
        response["errno"] = 0;
        response["errmsg"] = "success";
        
        json gradeArray = json::array();
        for (const auto &sg : grades)
        {
            json item;
            item["studentID"] = sg.studentID;
            item["studentName"] = sg.studentName;
            item["className"] = sg.className;
            item["averageScore"] = sg.averageScore;
            gradeArray.push_back(item);
        }
        response["studentGrades"] = gradeArray;
    }
    else
    {
        response["errno"] = 1;
        response["errmsg"] = "未找到成绩数据或数据库错误";
    }

    conn->send(response.dump());
}

void SQLService::adminQueryClassStudentsGrades(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    string className = js["className"];
    
    // 执行班级成绩查询
    vector<StudentGrade> gradeList = _adminModel.queryClassStudentsGrades(className);

    // 构造响应报文
    json response;
    response["msgid"] = AD_QUERY_CLASS_GRADES_ACK;
    
    if (!gradeList.empty())
    {
        response["errno"] = 0;
        response["errmsg"] = "班级成绩查询成功";
        json dataArr = json::array();
        
        // 遍历班级所有学生成绩
        for (const auto &student : gradeList)
        {
            json item;
            item["studentID"] = student.studentID;
            item["studentName"] = student.studentName;
            item["className"] = student.className;
            item["averageScore"] = student.averageScore;
            dataArr.push_back(item);
        }
        response["data"] = dataArr;
    }
    else
    {
        response["errno"] = 1;
        response["errmsg"] = "未找到该班级的成绩记录或班级不存在";
    }

    conn->send(response.dump());
}
void SQLService::adminQueryCoursesAverageScores(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    // 获取课程平均成绩数据
    vector<CourseAverage> averages = _adminModel.queryCoursesAverageScores();

    json response;
    response["msgid"] = AD_QUERY_COURSES_AVERAGE_ACK;

    if (!averages.empty())
    {
        response["errno"] = 0;
        response["errmsg"] = "success";
        
        json courseArray = json::array();
        for (const auto &ca : averages)
        {
            json item;
            item["courseID"] = ca.courseID;
            item["courseName"] = ca.courseName;
            item["averageScore"] = ca.averageScore;
            courseArray.push_back(item);
        }
        response["courseAverages"] = courseArray;
    }
    else
    {
        response["errno"] = 1;
        response["errmsg"] = "未找到课程成绩数据";
    }

    conn->send(response.dump());
}
void SQLService::adminQueryClassCoursesAverage(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    string className = js["className"];
    
    // 查询班级课程平均分（courseID -> averageScore）
    map<string, double> courseAvgMap = _adminModel.queryClassCoursesAverage(className);

    json response;
    response["msgid"] = AD_QUERY_CLASS_COURSES_AVERAGE_ACK;

    if (!courseAvgMap.empty())
    {
        response["errno"] = 0;
        response["errmsg"] = "班级课程平均分查询成功";
        json dataArr = json::array();
        
        // 遍历课程数据并补充课程名称
        for (const auto &[courseID, avgScore] : courseAvgMap)
        {
            // 查询课程详细信息
            Course course = _adminModel.queryCourse(courseID);
            
            json item;
            item["courseID"] = courseID;
            item["courseName"] = course.getCourseName();  // 从课程对象获取名称
            item["averageScore"] = avgScore;
            dataArr.push_back(item);
        }
        response["data"] = dataArr;
    }
    else
    {
        response["errno"] = 1;
        response["errmsg"] = "未找到该班级的课程成绩记录";
    }

    conn->send(response.dump());
}
void SQLService::adminRemoveCourse(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    string courseId = js["courseId"];

    bool state = _adminModel.removeCourse(courseId);

    json response;
    response["msgid"] = AD_REMOVE_COURSE_ACK;

    if (state) 
    {
        response["errno"] = 0;
        response["errmsg"] = "课程删除成功";
        response["deletedCourse"] = {
            {"courseId", courseId},
            {"updateTeachers", true}  // 表示已同步更新教师课程列表
        };
    }
    else 
    {
        response["errno"] = 1;
        response["errmsg"] = "课程删除失败";
    }

    conn->send(response.dump());
}
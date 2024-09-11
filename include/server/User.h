/**
 * @Description 用户类
 * @Version 1.0.0
 * @Date 2024/9/10 17:40
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */
#ifndef CLUSTERCHATSERVER_USER_H
#define CLUSTERCHATSERVER_USER_H

#include <string>

using namespace std;


class User {

public:

    User(int id = -1,string name = "",string pwd = "",string state = "offline");

    void setId(int id);

    void setName(string name);

    void setPwd(string pwd);

    void setState(string state);

    int getId() const;

    string getName() const;

    string getPassword() const;

    string getState() const;

private:
    int id;
    string name;
    string password;
    string state;
};


#endif //CLUSTERCHATSERVER_USER_H

/**
 * @Description 用户类
 * @Version 1.0.0
 * @Date 2024/9/10 17:40
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */
#ifndef CLUSTERCHATSERVER_USER_HPP
#define CLUSTERCHATSERVER_USER_HPP

#include <string>
#include "Public.hpp"

using namespace std;


class User {

public:

    User(int id = -1,string name = "",string pwd = "",string state = OFFLINE){
        this->id = id;
        this->name = name;
        this->password = password;
        this->state = state;
    }

    void setId(int id){
        this->id = id;
    }

    void setName(string name){
        this->name = name;
    }

    void setPwd(string pwd){
        this->password = pwd;
    }

    void setState(string state){
        this->state = state;
    }

    int getId() const{
        return id;
    }

    string getName() const{
        return name;
    }

    string getPwd() const{
        return password;
    }

    string getState() const{
        return state;
    }

protected:
    int id;
    string name;
    string password;
    string state;
};


#endif //CLUSTERCHATSERVER_USER_HPP

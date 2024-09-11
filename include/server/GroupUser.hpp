/**
 * @Description 群组用户类
 * @Version 1.0.0
 * @Date 2024/9/11 20:04
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */
#ifndef CLUSTERCHATSERVER_GROUPUSER_H
#define CLUSTERCHATSERVER_GROUPUSER_H

#include "User.hpp"

class GroupUser : public User{

public:
    void setRole(string role){
        this->role = role;
    }

    string getRole(){
        return this->role;
    }

private:
    string role;
};


#endif //CLUSTERCHATSERVER_GROUPUSER_H

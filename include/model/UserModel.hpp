/**
 * @Description User表的数据操作类
 * @Version 1.0.0
 * @Date 2024/9/10 18:18
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */
#ifndef CLUSTERCHATSERVER_USERMODEL_HPP
#define CLUSTERCHATSERVER_USERMODEL_HPP

#include <iostream>
#include "User.h"
#include "DataBase.hpp"

using namespace std;

class UserModel {
public:

    // 插入用户
    bool insert(User& user);

    // 根据用户号码查询用户信息
    User query(int id);

    // 更新用户的状态信息
    bool updateState(User user);

    // 重置用户状态信息
    // void resetState();

};


#endif //CLUSTERCHATSERVER_USERMODEL_HPP

/**
 * @Description User表的数据操作类
 * @Version 1.0.0
 * @Date 2024/9/10 18:18
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */
#ifndef CLUSTERCHATSERVER_USERMODEL_H
#define CLUSTERCHATSERVER_USERMODEL_H

#include <iostream>
#include "User.h"
#include "DataBase.h"

using namespace std;

class UserModel {
public:
    bool insert(User& user);
};


#endif //CLUSTERCHATSERVER_USERMODEL_H

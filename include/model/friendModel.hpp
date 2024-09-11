/**
 * @Description 维护好友信息的操作接口
 * @Version 1.0.0
 * @Date 2024/9/11 17:21
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */
#ifndef CLUSTERCHATSERVER_FRIENDMODEL_H
#define CLUSTERCHATSERVER_FRIENDMODEL_H

#include "User.hpp"
#include "DataBase.hpp"
#include <vector>

class friendModel {
public:
    // 添加好友关系
    void insert(int userId,int friendId);

    // 返回用户好友列表
    vector<User> query(int userId);
};


#endif //CLUSTERCHATSERVER_FRIENDMODEL_H

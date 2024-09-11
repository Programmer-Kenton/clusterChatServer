/**
 * @Description 提供离线消息表的操作接口
 * @Version 1.0.0
 * @Date 2024/9/11 15:36
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */
#ifndef CLUSTERCHATSERVER_OFFLINEMESSGEMODEL_H
#define CLUSTERCHATSERVER_OFFLINEMESSGEMODEL_H

#include <string>
#include <vector>
#include "DataBase.hpp"
using namespace std;


class offLineMessageModel {
public:
    // 存储用户的离线消息
    void insert(int userId,string msg);

    // 删除用户的离线消息
    void remove(int userId);

    // 查询用户的离线消息
    vector<string> query(int userId);
};


#endif //CLUSTERCHATSERVER_OFFLINEMESSGEMODEL_H

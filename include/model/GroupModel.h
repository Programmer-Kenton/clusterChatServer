/**
 * @Description 维护群组信息的操作接口
 * @Version 1.0.0
 * @Date 2024/9/11 20:18
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */
#ifndef CLUSTERCHATSERVER_GROUPMODEL_H
#define CLUSTERCHATSERVER_GROUPMODEL_H

#include "Group.hpp"
#include "DataBase.hpp"

class GroupModel {

public:
    // 创建群组
    bool createGroup(Group &group);

    // 加入群组
    void addGroup(int userId,int groupId,string role);

    // 查询用户所在群组信息
    vector<Group> queryGroups(int userId);

    // 根据指定的groupId查询群组用户id列表 除userId自己 主要用户群聊业务给群组其他成员群发消息
    vector<int> queryGroupUsers(int userId,int groupId);

};


#endif //CLUSTERCHATSERVER_GROUPMODEL_H

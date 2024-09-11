/**
 * @Description TODO
 * @Version 1.0.0
 * @Date 2024/9/11 20:18
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */

#include "GroupModel.h"

bool GroupModel::createGroup(Group &group) {
    char sql[1024] = {0};

    sprintf(sql,"insert into allgroup(groupname,groupdesc) values('%s','%s')",
            group.getName().c_str(),group.getDesc().c_str());

    LOG_INFO << "创建群组sql为:" << sql;

    MySQL mysql;
    if (mysql.connect()){
        if (mysql.update(sql)){
            group.setId(mysql_insert_id(mysql.getConnection()));
            LOG_INFO << "创建群组成功...";
            return true;
        }
    }

    return true;
}

void GroupModel::addGroup(int userId, int groupId, string role) {
    char sql[1024] = {0};

    sprintf(sql,"insert into groupuser values(%d,%d,'%s')",
            groupId,userId,role.c_str());

    LOG_INFO << "加入群聊的sql:" << sql;

    MySQL mysql;
    if (mysql.connect()){
        if (mysql.update(sql)){
            LOG_INFO << "加入群聊成功...";
        }
    }
}

vector<Group> GroupModel::queryGroups(int userId) {
    /**
     * 1.先根据userId在gourpuser表中查询出该用户所属的群组信息
     * 2.再根据群组信息 查询属于该群组的所有用户的userid并且和user表进行多表联合查询 查出用户的详细信息
     */
     char sql[1024] = {0};

    sprintf(sql,"select a.id,a.groupname,a.groupdesc from allgroup a inner join groupuser b on a.id = b.groupid where b.userid = %d",
            userId);

    LOG_INFO << "查询该用户所属组的群组信息sql:" << sql;

    vector<Group> groupVec;

    MySQL mysql;

    if (mysql.connect()){
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr){
            MYSQL_ROW row;
            // 查出userid所有的群组信息
            Group group;
            while ((row = mysql_fetch_row(res)) != nullptr){
                group.setId(atoi(row[0]));
                group.setName(row[1]);
                group.setDesc(row[2]);
                groupVec.push_back(group);
            }
            mysql_free_result(res);
        }
    }

    // 查询群组的用户信息
    for(Group &group : groupVec){
        sprintf(sql,"select a.id,a.name,a.state,b.grouprole from user a inner join groupuser b on b.userid = a.id where b.groupid = %d",
                group.getId());

        LOG_INFO << "查询群组的用户信息sql:" << sql;

        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr){
            MYSQL_ROW row;
            GroupUser user;
            while ((row = mysql_fetch_row(res)) != nullptr){
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                user.setRole(row[3]);
                group.getUsers().push_back(user);
            }
            mysql_free_result(res);
        }
    }

    return groupVec;
}

vector<int> GroupModel::queryGroupUsers(int userId, int groupId) {
    char sql[1024] = {0};

    vector<int> idVec;

    MySQL mysql;

    if (mysql.connect()){
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr){
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr){
                idVec.push_back(atoi(row[0]));
            }
            mysql_free_result(res);
        }
    }
    return idVec;
}

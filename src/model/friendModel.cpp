/**
 * @Description TODO
 * @Version 1.0.0
 * @Date 2024/9/11 17:21
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */
#include "friendModel.hpp"

void friendModel::insert(int userId, int friendId) {
    char sql[1024] = {0};

    sprintf(sql,"insert into friend values(%d,%d)",userId,friendId);

    LOG_INFO << "添加好友,拼接sql语句:" << sql;

    MySQL mysql;

    if (mysql.connect()){
        mysql.update(sql);
        LOG_INFO << "添加好友信息写入数据库成功...";
    }
}

vector<User> friendModel::query(int userId) {
    char sql[1024] = {0};

    sprintf(sql,"select u.id,u.name,u.state from user u inner join friend f on f.friendid = u.id where f.userid=%d",userId);

    vector<User> vec;

    MySQL mysql;

    if (mysql.connect()){
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr){
            MYSQL_ROW row;
            User user;
            while ((row = mysql_fetch_row(res)) != nullptr){
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                vec.push_back(user);
            }
            mysql_free_result(res);
            return vec;
        }
    }
}

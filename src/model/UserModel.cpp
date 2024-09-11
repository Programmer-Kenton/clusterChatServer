/**
 * @Description 实现User表的数据操作类
 * @Version 1.0.0
 * @Date 2024/9/10 18:18
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */

#include "UserModel.hpp"

bool UserModel::insert(User &user) {
    char sql[1024] = {0};

    sprintf(sql, "insert into user(name,password,state) values('%s','%s','%s')",
            user.getName().c_str(), user.getPwd().c_str(), user.getState().c_str());

    LOG_INFO << "插入用户操作,拼接好的sql为:" << sql;

    MySQL mysql;

    if (mysql.connect()){
        if (mysql.update(sql)){
            // 获取插入成功的用户数据生成的主键id
            user.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    return false;
}

User UserModel::query(int id) {
    char sql[1024] = {0};

    sprintf(sql,"select * from user where id = %d",id);

    LOG_INFO << "查询操作,拼接好的sql为:" << sql;

    MySQL mysql;

    if (mysql.connect()){
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr){
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row != nullptr){
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setPwd(row[2]);
                user.setState(row[3]);

                // 释放mysql查询结果的内存资源
                mysql_free_result(res);
                return user;
            }
        }
    }

    // User构造函数默认id为-1 用这个条件判断没有查询到结果
    return User();
}

bool UserModel::updateState(User user) {
    char sql[1024] = {0};

    sprintf(sql,"update user set state = '%s' where id = %d",user.getState().c_str(),user.getId());

    LOG_INFO << "修改用户状态,拼接好的sql为:" << sql;

    MySQL mysql;

    if (mysql.connect()){
        if (mysql.update(sql)){
            LOG_INFO << "登录操作...修改用户状态成功";
            return true;
        }
    }
    return false;
}

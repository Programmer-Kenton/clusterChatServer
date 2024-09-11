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

    sprintf(sql,"insert into user(name,password,state) values('%s','%s','%s')",
            user.getName().c_str(),user.getPassword().c_str(),user.getState().c_str());

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
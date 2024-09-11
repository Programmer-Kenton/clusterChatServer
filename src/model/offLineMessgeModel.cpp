/**
 * @Description TODO
 * @Version 1.0.0
 * @Date 2024/9/11 15:36
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */

#include "offLineMessageModel.hpp"

void offLineMessageModel::insert(int userId,string msg){
    char sql[1024] = {0};

    sprintf(sql,"insert into offlinemessage values(%d,'%s')",
            userId,msg.c_str());

    MySQL mysql;
    if (mysql.connect()){
        mysql.update(sql);
    }
}

void offLineMessageModel::remove(int userId) {
    char sql[1024] = {0};

    sprintf(sql,"delete from offlinemessage where userid = %d",userId);

    MySQL mysql;

    if (mysql.connect()){
        mysql.update(sql);
    }
}

vector<string> offLineMessageModel::query(int userId) {
    char sql[1024] = {0};

    sprintf(sql,"select * from offlinemessage where userid = %d",userId);

    vector<string> vec;

    MySQL mysql;
    if (mysql.connect()){
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr){
            // 把userId用户的所有离线消息放入vec容器中并返回
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr){
                vec.push_back(row[1]);
            }
            mysql_free_result(res);
            return vec;
        }
    }

    return vec;
}

/**
 * @Description 数据库操作类
 * @Version 1.0.0
 * @Date 2024/9/10 16:45
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */
#ifndef CLUSTERCHATSERVER_DATABASE_HPP
#define CLUSTERCHATSERVER_DATABASE_HPP

#include <muduo/base/Logging.h>
#include <mysql/mysql.h>
#include <string>

using namespace std;

class MySQL {

    // {"msgid":2,"name":"LCX","password":"123456"}
public:
    // 初始化数据库连接
    MySQL();

    // 释放数据库连接资源
    ~MySQL();

    // 连接数据库
    bool connect();

    // 更新操作
    bool update(string sql);

    // 查询操作
    MYSQL_RES* query(string sql);

    // 获取连接
    MYSQL *getConnection();

private:
    // 加载数据库配置文件信息
    bool loadConfigFile();

private:
    // 连接对象
    MYSQL *_conn;

    // ip地址
    string _ip;

    // 端口号
    unsigned short _port;

    // 连接用户
    string _username;

    // 连接密码
    string _password;

    // 连接的数据库名
    string _dbname;
};


#endif //CLUSTERCHATSERVER_DATABASE_HPP

/**
 * @Description 实现数据库操作类
 * @Version 1.0.0
 * @Date 2024/9/10 16:45
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */

#include "DataBase.h"

DataBase::DataBase() {
    _conn = mysql_init(nullptr);
    loadConfigFile();
}

DataBase::~DataBase() {
    if (_conn != nullptr) mysql_close(_conn);
}

bool DataBase::connect() {
    MYSQL *p = mysql_real_connect(_conn,_ip.c_str(),_username.c_str(),_password.c_str(),_dbname.c_str(),_port, nullptr,0);

    if (p != nullptr){
        mysql_query(_conn,"set names gbk");
    }
    LOG_INFO << "连接数据成功..." << "username = " << _username << " dbname = " << _dbname << " port" << _port;

    return p;
}

bool DataBase::update(string sql) {
    if (mysql_query(_conn,sql.c_str())){
        LOG_ERROR << __FILE__ << ':' << __LINE__ << ':' << sql << "更新失败";
        return false;
    }
    return true;
}

MYSQL_RES *DataBase::query(string sql) {
    if (mysql_query(_conn,sql.c_str())){
        LOG_ERROR << __FILE__ << ':' << __LINE__ << ':' << sql << "查询失败";
        return nullptr;
    }
    return mysql_use_result(_conn);
}

bool DataBase::loadConfigFile() {
    FILE *pf = fopen("mysql.ini","r");
    if (pf == nullptr){
        LOG_ERROR << "mysql.ini file is not exist";
        return false;
    }

    while(!feof(pf)){
        char line[1024] = {0};
        fgets(line,1024,pf);
        string str = line;
        // 从0下标开始找到第一个出现'='的下标
        int idx = str.find('=',0);
        // 无效的配置
        if (idx == -1) continue;
        int endidx = str.find('\n',idx);
        string key = str.substr(0,idx);
        string value = str.substr(idx + 1,endidx - idx - 1);

        if (key == "ip"){
            _ip = value;
        } else if (key == "port"){
            _port = atoi(value.c_str());
        } else if (key == "username"){
            _username = key;
        } else if (key == "dbname"){
            _dbname = value;
        } else if (key == "password"){
            _password = value;
        }
    }

    return true;
}

MYSQL *DataBase::getConnection() {
    return _conn;
}

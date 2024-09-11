/**
 * @Description 实现数据库操作类
 * @Version 1.0.0
 * @Date 2024/9/10 16:45
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */

#include "DataBase.hpp"

MySQL::MySQL() {
    _conn = mysql_init(nullptr);
    LOG_INFO << "开始加载数据库配置...";
    if (loadConfigFile()){
        LOG_INFO << "数据库配置加载完成... ip = " << _ip << " username = " << _username <<" password = " << _password;
    }else{
        LOG_ERROR << "数据库配置加载失败...";
    }
}

MySQL::~MySQL() {
    if (_conn != nullptr) mysql_close(_conn);
}

bool MySQL::connect() {
    MYSQL *p = mysql_real_connect(_conn,_ip.c_str(),_username.c_str(),_password.c_str(),_dbname.c_str(),_port, nullptr,0);

    if (p != nullptr){
        mysql_query(_conn,"set names gbk");
        LOG_INFO << "数据库连接数据成功...";
    }


    return p;
}

bool MySQL::update(string sql) {
    if (mysql_query(_conn,sql.c_str())){
        LOG_ERROR << __FILE__ << ':' << __LINE__ << ':' << sql << " 更新失败";
        return false;
    }
    return true;
}

MYSQL_RES *MySQL::query(string sql) {
    if (mysql_query(_conn,sql.c_str())){
        LOG_ERROR << __FILE__ << ':' << __LINE__ << ':' << sql << "查询失败";
        return nullptr;
    }
    return mysql_use_result(_conn);
}

bool MySQL::loadConfigFile() {
    FILE *pf = fopen("mysql.ini","r");

    // 打印当前工作目录
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        LOG_INFO << "Current working directory: " << cwd;
    }

    if (pf == nullptr){
        LOG_ERROR << "文件空指针原因:" << strerror(errno);
        return false;
    }

    char line[1024];  // 定义line，用于存储读取的每一行数据

    while (fgets(line, 1024, pf) != nullptr) {
        string str = line;
        // 去掉末尾换行符
        str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
        str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());

        // 从0下标开始找到第一个出现'='的下标
        int idx = str.find('=', 0);
        // 无效的配置
        if (idx == -1) continue;

        string key = str.substr(0, idx);
        string value = str.substr(idx + 1);

        // 解析配置项
        if (key == "ip") {
            _ip = value;
        } else if (key == "port") {
            _port = atoi(value.c_str());
        } else if (key == "username") {
            _username = value;  // 修正错误
        } else if (key == "dbname") {
            _dbname = value;
        } else if (key == "password") {
            _password = value;
        }
    }
    return true;
}

MYSQL *MySQL::getConnection() {
    return _conn;
}

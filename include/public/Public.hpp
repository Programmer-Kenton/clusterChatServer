/**
 * @Description 公共类 定义公用宏
 * @Version 1.0.0
 * @Date 2024/9/10 15:43
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */
#ifndef CLUSTERCHATSERVER_PUBLIC_H
#define CLUSTERCHATSERVER_PUBLIC_H

#include <string>

// 函数回调消息类型
enum EnMsgType{
    // 登录消息
    LOGIN_MSG = 1,
    LOGIN_MSG_ACK = 2,
    // 注册消息
    REG_MSG = 3,
    // 注册响应消息
    REG_MSG_ACK = 4
};

// 回调消息
const std::string MSGID = "msgid";
const std::string ERRNO = "errno";
const std::string ERRMSG = "errmsg";

// 用户信息
const std::string ID = "id";
const std::string PWD = "password";
const std::string NAME = "name";

// 成功/失败操作
const int SUCCESS = 0;
const int FAILED = 1;
const int OTHER = 2;

// 用户在线状态
const std::string ONLINE = "online";
const std::string OFFLINE = "offline";

// 回显给前端的错误信息
const std::string INLOGIN = "该账号已经登录,请重新输入账号";
const std::string LOGIN_ERROR = "用户名/密码错误,请重新输入账号";

#endif //CLUSTERCHATSERVER_PUBLIC_H

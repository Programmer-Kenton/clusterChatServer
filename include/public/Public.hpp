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
    LOGIN_MSG = 1, // 登录消息
    REG_MSG = 2, // 注册消息
    REG_MSG_ACK = 3 // 注册响应消息
};

// 回调消息
const std::string MSGID = "msgid";
const std::string ERRNO = "errno";

// 用户信息
const std::string ID = "id";
const std::string PWD = "password";
const std::string NAME = "name";

#endif //CLUSTERCHATSERVER_PUBLIC_H

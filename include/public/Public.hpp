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
    // 注册消息
    REG_MSG = 2
};

// 回调消息
const std::string MSGID = "msgid";

// 发送方信息
const std::string ID = "id";

#endif //CLUSTERCHATSERVER_PUBLIC_H

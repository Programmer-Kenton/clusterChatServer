/**
 * @Description 实现业务处理模块
 * @Version 1.0.0
 * @Date 2024/9/10 15:31
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */

#include "ChatService.hpp"

ChatService *ChatService::instance() {
    static ChatService service;
    return &service;
}

void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    LOG_INFO << "do login service---登录成功";
}

void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    LOG_INFO << "do reg service---注册成功";
}

ChatService::ChatService() {
    // 初始化时插入回调消息及绑定的函数
    _msgHandlerMap.insert({LOGIN_MSG,std::bind(&ChatService::login,this,_1,_2,_3)});
    _msgHandlerMap.insert({REG_MSG,std::bind(&ChatService::reg,this,_1,_2,_3)});
}

MsgHandler ChatService::getHandler(int msgId) {
    // 记录错误日志 msgId没有对对应的事件处理回调
    auto it = _msgHandlerMap.find(msgId);
    if (it == _msgHandlerMap.end()){
        // 返回一个默认的空处理器 空操作
        return [=](const TcpConnectionPtr& conn,json& js,Timestamp time){
            LOG_ERROR << "msgId:" << msgId << "can not find handler";
        };
    }else{
        return _msgHandlerMap[msgId];
    }
}

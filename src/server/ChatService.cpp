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
    string name = js[NAME];
    string pwd = js[PWD];

    User user;
    user.setName(name);
    user.setPwd(pwd);
    json response;
    bool state = _userModel.insert(user);
    if (state){
        // 注册成功
        response[MSGID] = REG_MSG_ACK;
        response[ERRNO] = 0;
        response[ID] = user.getId();
        conn->send(response.dump());
        LOG_INFO << "用户名为:" << user.getName() << " 注册成功...";
    }else{
        // 注册失败
        response[MSGID] = REG_MSG_ACK;
        response[ERRNO] = 1;
        conn->send(response.dump());
        LOG_INFO << "用户名为:" << user.getName() << " 注册失败...";
    }
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

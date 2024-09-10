/**
 * @Description 业务处理模块
 * @Version 1.0.0
 * @Date 2024/9/10 15:31
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */
#ifndef CLUSTERCHATSERVER_CHATSERVICE_HPP
#define CLUSTERCHATSERVER_CHATSERVICE_HPP

#include <unordered_map>
#include <functional>
#include <muduo/net/TcpConnection.h>
#include <muduo/base/Logging.h>
#include <string>
#include "json.hpp"
#include "Public.hpp"

using namespace muduo;
using namespace muduo::net;
using namespace std;
using json = nlohmann::json;

// 处理消息的事件回调方法类型
using MsgHandler = std::function<void(const TcpConnectionPtr& conn,json& js,Timestamp time)>;


class ChatService {
public:

    // 获取单例对象的接口函数
    static ChatService* instance();

    // 登录业务
    void login(const TcpConnectionPtr& conn,json& js,Timestamp time);

    // 注册业务
    void reg(const TcpConnectionPtr& conn,json& js,Timestamp time);

    // 获取消息对应的处理器
    MsgHandler getHandler(int msgId);

private:

    // 注册消息以及对应的handler回调操作
    ChatService();

    // 存储消息id和其对应的事件处理方法
    unordered_map<int,MsgHandler> _msgHandlerMap;
};


#endif //CLUSTERCHATSERVER_CHATSERVICE_HPP

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
#include <string>
#include <map>
#include "json.hpp"
#include "Public.hpp"
#include "UserModel.hpp"
#include "offLineMessageModel.hpp"
#include "friendModel.hpp"
#include "GroupModel.h"
#include "Redis.hpp"


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

    // 一对一聊天业务
    void oneChat(const TcpConnectionPtr &conn,json &js,Timestamp time);

    // 处理注销业务
    void loginOut(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // 获取消息对应的处理器
    MsgHandler getHandler(int msgId);

    // 添加好友业务
    void addFriend(const TcpConnectionPtr& conn,json &js,Timestamp time);

    // 创建群组业务
    void createGroup(const TcpConnectionPtr& conn, json &js, Timestamp time);

    // 加入群组聊天
    void addGroup(const TcpConnectionPtr &conn,json &js,Timestamp time);

    // 群组聊天
    void groupChat(const TcpConnectionPtr &conn,json &js,Timestamp time);

    // 从redis消息队列中获取订阅的消息 usrId作为redis的key msg作为redis的value
    void handleRedisSubscribeMessage(int userId,string msg);

    // 处理客户端异常退出
    void clientCloseException(const TcpConnectionPtr& conn);

    // 处理服务器异常宕机 业务重置方法
    void serverCloseException();

private:

    // 注册消息以及对应的handler回调操作
    ChatService();

    // 存储消息id和其对应的事件处理方法
    unordered_map<int,MsgHandler> _msgHandlerMap;

    // 存储在线用户的通信连接
    unordered_map<int,TcpConnectionPtr> _userConnMap;

    // 定义互斥锁 保证_userConnMap的线程安全
    mutex _connMutex;

    // 用户数据操作对象
    UserModel _userModel;

    // 离线消息数据操作对象
    offLineMessageModel _offlineMsgModel;

    // 好友信息数据操作对象
    friendModel _friendModel;

    // 群组信息数据操作对象
    GroupModel _groupModel;

    // redis操作对象
    Redis _redis;
};


#endif //CLUSTERCHATSERVER_CHATSERVICE_HPP

/**
 * @Description 服务器通信模块
 * @Version 1.0.0
 * @Date 2024/9/10 14:46
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */
#ifndef CLUSTERCHATSERVER_CHATSERVER_HPP
#define CLUSTERCHATSERVER_CHATSERVER_HPP

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include "ChatService.hpp"

using namespace std::placeholders;


class ChatServer {

public:
    // 初始化通信服务器对象
    ChatServer(EventLoop* loop,const InetAddress& listenAddr,const string& nameArg);

    // 启动服务
    void start();

private:
    // 上报链接相关信息的回调函数
    void onConnection(const TcpConnectionPtr& conn);

    // 上报读写事件相关的回调函数
    void onMessage(const TcpConnectionPtr& conn,Buffer* buffer,Timestamp time);

private:
    // 组合的muduo库 实现服务器功能的类对象
    TcpServer _server;
    // 指向事件循环对象的指针
    EventLoop *_loop;
};


#endif //CLUSTERCHATSERVER_CHATSERVER_HPP

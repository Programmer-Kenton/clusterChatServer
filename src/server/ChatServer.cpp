/**
 * @Description 实现通信服务器类
 * @Version 1.0.0
 * @Date 2024/9/10 14:46
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */
#include "ChatServer.hpp"

ChatServer::ChatServer(EventLoop *loop, const InetAddress &listenAddr, const string &nameArg)
        : _server(loop, listenAddr, nameArg),
          _loop(loop) {
    // 注册链接回调
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection,this,_1));
    // 注册消息回调
    _server.setMessageCallback(std::bind(&ChatServer::onMessage,this,_1,_2,_3));

    // 设置线程数量
    _server.setThreadNum(4);
}

void ChatServer::start() {
    _server.start();
}

void ChatServer::onConnection(const muduo::net::TcpConnectionPtr &conn) {
    // 客户端断开连接
    if(!conn->connected()){
        conn->shutdown();
    }
}

void ChatServer::onMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buffer, muduo::Timestamp time) {
    string buf = buffer->retrieveAllAsString();
    // 数据的反序列化
    json js = json::parse(buf);
    // 完全解耦网络模块和业务模块
    // 通过js["msgId"]获取->业务handler->conn js time
    auto msgHandler = ChatService::instance()->getHandler(js[MSGID].get<int>());
    // 回调消息绑定好的事件处理器 来执行响应的业务处理
    msgHandler(conn,js,time);
}
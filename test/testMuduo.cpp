/**
 * @Description 测试muduo库
 * @Version 1.0.0
 * @Date 2024/9/10 10:41
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */

/**
 * muduo网络库给用户提供两个主要的类
 * TcpServer用于编写服务器程序
 * TcpClient 用于编写客户端程序
 * 好处:能把网络I/O的代码和业务代码区分开 用户的连接/中断及读写事件
*/


#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>
#include <iostream>
#include <functional>
#include <string>

using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;

/**
* 基于Muduo网络库开发服务器程序
 * 1.组合TcpServer对象
 * 2.创建EventLoop事件循环对象的指针
 * 3.明确TcpServer构造函数需要什么参数 输出ChatServer的构造函数
 * 4.在当前服务器类的构造函数中 注册处理函数连接的回调函数和处理读写事件的回调函数
 * 5.设置合适的服务端线程数量 muduo库会自动划分合适的I/O线程和Worker线程
*/

class ChatServer{
public:
    /**
     *
     * @param loop 事件循环
     * @param listenAddr Ip +Port
     * @param nameArg 服务器的名字
     */
    ChatServer(EventLoop *loop,const InetAddress& listenAddr,const string& nameArg) : _server(loop,listenAddr,nameArg),_loop(loop){
        // 给服务器注册用户连接的创建和断开回调
        _server.setConnectionCallback(std::bind(&ChatServer::onConnection,this,_1));
        // 给服务器注册用户读写事件回调
        _server.setMessageCallback(std::bind(&ChatServer::onMessage,this,_1,_2,_3));
        // 设置服务器端的线程数量
        _server.setThreadNum(4);
    }

    // 开启事件循环
    void start(){
        _server.start();
    }

private:
    // 处理用户的连接创建和断开 epoll listenfd accept
    void onConnection(const TcpConnectionPtr& conn){
        if (conn->connected()){
            cout << conn->peerAddress().toIpPort() << "->" << conn->localAddress().toIpPort() << " state:online" << endl;
        }else{
            cout << conn->peerAddress().toIpPort() << "->" << conn->localAddress().toIpPort() << " state:offline" << endl;
            conn->shutdown(); // close(fd)
            _loop->quit();
        }
    }

    /**
     * 处理用户的读写事件
     * @param conn 连接
     * @param buf 缓冲区
     * @param time 接收到数据的时间
     */
    void onMessage(const TcpConnectionPtr& conn,Buffer* buffer,Timestamp time){
        string buf = buffer->retrieveAllAsString();
        // 打印序列化后的数据
        cout << "recv data:" << buf << "time:" << time.toString() << endl;
        // 发送序列化后的数据
        conn->send(buf);
        LOG_INFO
    }


private:
    TcpServer _server;
    EventLoop *_loop;

};


int main(){
    EventLoop loop;

    InetAddress addr("127.0.0.1",6000);

    ChatServer server(&loop,addr,"ChatServer");

    server.start();

    // epoll_wait 以阻塞方式等待新用户连接,已连接用户的读写事件等操作
    loop.loop();

    return 0;
}
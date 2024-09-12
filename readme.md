# C++高性能网络集群服务器通信



大家好，我是天津市大学软件学院飞猪6。开学这段时间学习完Linux系统&&网络编程，着手做了集群服务器通信项目。



## 技术栈

- Json序列化和反序列化 
- muduo网络库开发 
- nginx源码编译安装和环境部署 
- nginx的tcp负载均衡器配置 
- redis缓存服务器编程实践 
- 基于发布-订阅的服务器中间件redis消息队列编程实践 
- MySQL数据库编程 
- CMake构建编译环境 
- Github托管项目



## 项目需求

1. 客户端新用户注册
2. 客户端用户登录 
3. 添加好友和添加群组
4. 好友聊天 
5.  群组聊天 
6. 离线消息 
7. Nginx配置TCP负载均衡 
8. 集群聊天系统支持客户端跨服务器通信



## 项目目标 

1. 掌握服务器的网络I/O模块，业务模块，数据模块分层的设计思想 
2. 掌握C++ muduo网络库的编程以及实现原理 
3. 掌握Json的编程应用 
4. 掌握nginx配置部署tcp负载均衡器的应用以及原理 
5. 掌握服务器中间件的应用场景和基于发布-订阅的redis编程实践以及应用原理 
6. 掌握CMake构建自动化编译环境 
1. 掌握Github管理项目





## 开发环境

1. Ubuntu Linux环境 

2. 安装Json开发库 

3. 安装boost + muduo网络库开发环境

   参考我的博客： 待编写...

4. 安装redis环境 

5. 安装mysql数据库环境 

6. 安装nginx 

7. 安装CMake环境

​	

## 配置远程开发环境

使用CLion连接服务器开发，待完善...

<img src=".\picture\01.png" alt="01项目目录结构" style="zoom:80%;" />





## Json介绍

Json是一种轻量级的数据交换格式（也叫数据序列化方式）。Json采用完全独立于编程语言的文本格式 来存储和表示数据。简洁和清晰的层次结构使得 Json 成为理想的数据交换语言。 易于人阅读和编写，同时也易于机器解析和生成，并有效地提升网络传输效率。



### 一个优秀的Json三方库

JSON for Modern C++ 是一个由德国大牛 nlohmann 编写的在 C++ 下使用的 JSON 库。

具有以下特点

- 直观的语法
- 整个代码由一个头文件组成 json.hpp，没有子项目，没有依赖关系，没有复杂的构建系统，使用起来非常方便
- 使用 C++ 11 标准编写
- 使用 json 像使用 STL 容器一样 STL 和 json 容器之间可以相互转换
- 严谨的测试：所有类都经过严格的单元测试，覆盖了 100％ 的代码，包括所有特殊的行为。
- 此外，还检查了 Valgrind 是否有内存泄漏。为了保持高质量，该项目遵循核心基础设施倡议(CII) 的最佳实践



### Json序列化及反序列化

在网络中，常用的数据传输序列化格式有XML，Json，ProtoBuf，在公司级别的项目中，大量的在使用 ProtoBuf作为数据序列化的方式，以其数据压缩编码传输，占用带宽小，同样的数据信息，是Json的 1/10，XML的1/20，但是使用起来比Json稍复杂一些，所以项目中我们选择常用的Json格式来打包传输数据。 





## muduo网络库编程

muduo源码编译安装 muduo库源码编译安装和环境搭建，

参考我的博客： 待编写...



### 测试基于muduo的客户端服务器编程

muduo网络库的编程很容易，要实现基于muduo网络库的服务器和客户端程序，只需要简单的组合 TcpServer和TcpClient就可以，代码实现如下：

```cpp
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
```



### 测试用muduo中的线程池做计算任务

采用muduo进行服务器编程，如果遇到需要开辟多线程单独来处理复杂的计算任务或者其它阻塞任务 等，不需要直接调pthread_create来创建线程，muduo库提供的ThreadPool线程池管理类已经把Linux 的线程创建完全封装起来了，如果想研究源码，可以剖析muduo中ThreadPool.cc和Thread.cc。

```cpp
// 客户端输入界面，在单独的线程中接收用户输入进行发送操作
void userClient(const muduo::net::TcpConnectionPtr &con);
muduo::ThreadPool _pool;
```

```cpp
/*
连接服务器成功后，开启和服务器的交互通信功能
*/
if (con->connected()) // 和服务器连接成功{
LOG_INFO << "connect server success!";
	// 启动线程专门处理用户的输入操作
	_pool.run(bind(&ChatClient::userClient, this, con));
}else {
	// 和服务器连接失败
}
```



### muduo的日志系统

在开发软件产品过程中，日志的输出非常重要，可以记录很多软件运行过程中的信息，方便定位调试问 题，跟踪统计信息等等，muduo库提供的日志级别有：

```cpp
#define LOG_TRACE if (muduo::Logger::logLevel() <= muduo::Logger::TRACE) \
  muduo::Logger(__FILE__, __LINE__, muduo::Logger::TRACE, __func__).stream()
#define LOG_DEBUG if (muduo::Logger::logLevel() <= muduo::Logger::DEBUG) \
  muduo::Logger(__FILE__, __LINE__, muduo::Logger::DEBUG, __func__).stream()
#define LOG_INFO if (muduo::Logger::logLevel() <= muduo::Logger::INFO) \
  muduo::Logger(__FILE__, __LINE__).stream()
#define LOG_WARN muduo::Logger(__FILE__, __LINE__, muduo::Logger::WARN).stream()
#define LOG_ERROR muduo::Logger(__FILE__, __LINE__, muduo::Logger::ERROR).stream()
#define LOG_FATAL muduo::Logger(__FILE__, __LINE__, muduo::Logger::FATAL).stream()
#define LOG_SYSERR muduo::Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL muduo::Logger(__FILE__, __LINE__, true).stream()
```



## muduo网络库的多线程模型

### 网络服务器编程常用模型

**方案1 ： accept + read/write**  

不是并发服务器 

**方案2 ： accept + fork - process-pre-connection**  

适合并发连接数不大，计算任务工作量大于fork的开销 

**方案3 ：accept + thread thread-pre-connection**  

比方案2的开销小了一点，但是并发造成线程堆积过多 

**方案4： muduo的网络设计：reactors in threads - one loop per thread** 

方案的特点是one loop per thread，有一个main reactor负载accept连接，然后把连接分发到某个sub reactor（采用round-robin的方式来选择sub reactor），该连接的所用操作都在那个sub reactor所处 的线程中完成。多个连接可能被分派到多个线程中，以充分利用CPU。Reactor poll的大小是固定的，根据CPU的数目确定。



## 服务器集群 待完善...

### 负载均衡器 一致性哈希算法

单台服务器受限于硬件资源，其性能是有上限的，当单台服务器不能满足应用场景的并发需求量时，就需要考虑部署多个服务器共同处理客户端的并发请求，但是客户端怎么知道去连接具体哪台服务器呢？ 此时就需要一台负载均衡器，通过预设的负载算法，指导客户端连接服务器。 负载均衡器有基于客户端的负载均衡和服务器的负载均衡。 普通的基于哈希的负载算法，并不能满足负载均衡所要求的单调性和平衡性，但一致性哈希算法非常好 的保持了这两种特性，所以经常用在需要设计负载算法的应用场景当中。 



### nginx配置tcp负载均衡

在服务器快速集群环境搭建中，都迫切需要一个能拿来即用的负载均衡器，nginx在1.9版本之前，只支 持http协议web服务器的负载均衡，从1.9版本开始以后，nginx开始支持tcp的长连接负载均衡，但是 nginx默认并没有编译tcp负载均衡模块，编写它时，需要加入--with-stream参数来激活这个模块。

nginx编译安装需要先安装pcre、openssl、zlib等库，也可以直接编译执行下面的configure命令，根 据错误提示信息，安装相应缺少的库。

nginx.conf配置文件

```nginx
stream{

    # TCP的负载均衡配置
    upstream MyServer{
        hash $remote_addr consistent;
        server 127.0.0.1:6000 weight=1 max_fails=3 fail_timeout=30s;
        server 127.0.0.1:6002 weight=1 max_fails=3 fail_timeout=30s;
    }

    server{
        proxy_connect_timeout 1s;
        listen 8000;
        proxy_pass MyServer;
        tcp_nodelay on;
    }
}
```



## 服务器中间件-基于发布订阅的Redis 待完善...

### 集群服务器之间的通信设计

redis的发布-订阅机制：发布-订阅模式包含了两种角色，分别是消息的发布者和消息的订阅者。订阅 者可以订阅一个或者多个频道channel，发布者可以向指定的频道channel发送消息，所有订阅此频道的 订阅者都会收到此消息。



## 数据库设计

### 表设计

**User表** 

| 字段名称 | 字段类型                 | 字段说明     | 约束                        |
| -------- | ------------------------ | ------------ | --------------------------- |
| id       | int                      | 用户id       | primary key、auto_increment |
| name     | varchar(50)              | 用户名       | not null,unique             |
| password | vatchar(50)              | 用户密码     | not null                    |
| state    | enum('online','offline') | 当前登录状态 | default 'offline'           |

**Friend表** 

| 字段名称 | 字段类型 | 字段说明 | 约束               |
| -------- | -------- | -------- | ------------------ |
| userid   | int      | 用户id   | not null、联合主键 |
| friendid | int      | 好友id   | not null、联合主键 |

**AllGroup表** 

| 字段名称  | 字段类型     | 字段说明   | 约束                        |
| --------- | ------------ | ---------- | --------------------------- |
| id        | int          | 组id       | primary key、auto_increment |
| groupname | varchar(50)  | 组名称     | not null、unique            |
| groupdesc | varchar(200) | 组功能描述 | default ' '                 |

**GroupUser表** 

| 字段名称  | 字段类型                 | 字段说明 | 约束               |
| --------- | ------------------------ | -------- | ------------------ |
| groupid   | int                      | 组id     | not null、联合主键 |
| userid    | int                      | 组员id   | not null、联合主键 |
| grouprole | enum('creator','normal') | 组内角色 | default 'normal'   |

**OfflineMessage表**

| 字段名称 | 字段类型    | 字段说明                 | 约束     |
| -------- | ----------- | ------------------------ | -------- |
| userid   | int         | 用户id                   | not null |
| message  | varchar(50) | 离线消息(存储Json字符串) | not null |


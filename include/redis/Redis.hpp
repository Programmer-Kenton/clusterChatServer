/**
 * @Description redis作为集群服务器通信的基于发布-订阅消息队列
 * @Version 1.0.0
 * @Date 2024/9/12 15:33
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */
#ifndef CLUSTERCHATSERVER_REDIS_HPP
#define CLUSTERCHATSERVER_REDIS_HPP

#include <hiredis/hiredis.h>
#include <thread>
#include <functional>

using namespace std;

class Redis {

public:

    Redis();

    ~Redis();

    // 连接redis服务器
    bool connect();

    // 向redis指定的通道channel发布消息
    bool publish(int channel,string message);

    // 向redis指定的通道subscribe订阅消息
    bool subscribe(int channel);
private:
    // hiredis同步上下文对象 负责publish消息
    // redisCon
};


#endif //CLUSTERCHATSERVER_REDIS_HPP

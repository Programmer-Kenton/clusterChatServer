/**
 * @Description 服务器端主函数
 * @Version 1.0.0
 * @Date 2024/9/10 14:59
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */

#include "ChatServer.hpp"
#include <signal.h>

using namespace std;

// 处理服务器ctrl+c结束后 重置user的状态信息
void resetHandler(int){
    ChatService::instance()->serverCloseException();
    exit(0);
}


int main(){

    signal(SIGINT,resetHandler);

    EventLoop loop;
    InetAddress addr("127.0.0.1",6000);
    ChatServer server(&loop,addr,"ChatServer");

    server.start();

    loop.loop();

    return 0;
}

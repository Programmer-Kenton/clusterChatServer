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


int main(int argc,char *argv[]){

    if (argc < 3){
        cerr << "command invalid! example: ./ChatServer 127.0.0.1 6000" << endl;
        exit(-1);
    }

    // 解析通过命令行参数传递的ip和port
    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);

    signal(SIGINT,resetHandler);

    EventLoop loop;
    InetAddress addr(ip, port);
    ChatServer server(&loop, addr, "ChatServer");

    server.start();
    loop.loop();

    return 0;
}

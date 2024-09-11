/**
 * @Description 客户端主程序
 * @Version 1.0.0
 * @Date 2024/9/11 21:31
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */

#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <unistd.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "json.hpp"
#include "Group.hpp"
#include "User.hpp"
#include "Public.hpp"

using namespace std;
using json = nlohmann::json;

// 记录当前系统登录的用户信息
User g_currentUser;

// 记录当前登录用户的好友列表信息
vector<User> g_currentUserFriendList;

// 记录当前登录用户的群组列表消息
vector<Group> g_currentUserGroupList;

// 显示当前当前登录成功用户的基本信息
void showCurrentUserData();

// 接收线程
void readTaskHandler(int clientFd);

// 获取系统事件 聊天信息需要添加时间信息
std::string getCurrentTime();

// 主聊天页面程序
void mainMenu(int clientFd);

// 聊天客户段程序实现 main线程用作发送消息线程 子线程用作接收消息线程
int main(int argc,char *argv[]){
    if (argc < 3){
        cerr << "command invalid example: ./ChatClient 127.0.0.1 6000" << endl;
        exit(-1);
    }

    // 解析通过命令行参数传递的ip和port
    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);

    // 创建client端的socket
    int clientFd = socket(AF_INET, SOCK_STREAM,0);
    if (-1 == clientFd){
        cerr << "socket create error" << endl;
        exit(-1);
    }

    // 填写client需要连接的server信息ip+port
    sockaddr_in server;
    memset(&server,0, sizeof(sockaddr_in));

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);

    // client和server进行连接
    if (-1 == connect(clientFd,(sockaddr*)&server, sizeof(sockaddr_in))){
        cerr << "connect server error" << endl;
        close(clientFd);
        exit(-1);
    }

    // main线程用于接受用户输入 负责发送数据
    for(;;){
        // 显示首页面菜单 登录 注册 退出
        cout << "================================" << endl;
        cout << "1.login" << endl;
        cout << "2.register" << endl;
        cout << "3.quit" << endl;
        cout << "================================" << endl;
        cout << "choice: ";
        int choice = 0;
        cin >> choice;
        cin.get(); // 读取缓冲区中的回车

        switch (choice) {
            case 1: // login业务
            {
                int id = 0;
                char pwd[50] = {0};
                cout << "userid:";
                cin >> id;
                cin.get(); // 读掉缓冲区的回车
                cout << "userpassword:";
                cin.getline(pwd,50);

                json js;
                js[MSGID] = LOGIN_MSG;
                js[ID] = id;
                js[PWD] = pwd;
                string request = js.dump();

                int len = send(clientFd,request.c_str(), strlen(request.c_str()) + 1,0);
                if (len == -1){
                    cerr << "send login msg error" << request << endl;
                }else{
                    char buffer[1024] = {0};
                    len = recv(clientFd,buffer,1024,0);
                    if (-1 == len){
                        cerr << "recv login response error" << endl;
                    }else{
                        json responseJs = json::parse(buffer);
                        if (0 != responseJs[ERRNO].get<int>()){
                            // 登录失败
                            cerr << responseJs[ERRMSG] << endl;
                        }else{
                            // 登录成功
                            // 记录当前用户的id和name
                            g_currentUser.setId(responseJs[ID].get<int>());
                            g_currentUser.setName(responseJs[NAME]);

                            // 记录当前用户的好友列表信息
                            if (responseJs.contains(FRIENDS)){
                                vector<string> vec = responseJs[FRIENDS];
                                User user;
                                for(string &str : vec){
                                    json js = json::parse(str);
                                    user.setId(js[ID].get<int>());
                                    user.setName(js[NAME]);
                                    user.setState(js[STATE]);
                                    g_currentUserFriendList.push_back(user);
                                }
                            }
                            // 记录当前用户的群组列表消息
                            if (responseJs.contains(GROUPS)){
                                vector<string> vec1 = responseJs[GROUPS];
                                Group group;
                                for(string &groupstr : vec1){
                                    json grpjs = json::parse(groupstr);
                                    group.setId(grpjs[ID].get<int>());
                                    group.setName(grpjs[GROUPNAME]);
                                    group.setDesc(grpjs[GROUPDESC]);

                                    vector<string> vec2 = grpjs[USERS];
                                    GroupUser user;
                                    for(string &userstr : vec2){
                                        json js  = json::parse(userstr);
                                        user.setId(js[ID].get<int>());
                                        user.setName(js[NAME]);
                                        user.setState(js[STATE]);
                                        user.setRole(js[ROLE]);
                                        group.getUsers().push_back(user);
                                    }

                                    g_currentUserGroupList.push_back(group);
                                }
                            }

                            // 显示登录用户的基本信息
                            showCurrentUserData();

                            // 显示当前用户的离线消息 个人聊天或者群组消息
                            if (responseJs.contains(OFFLINEMSG)){
                                vector<string> vec = responseJs[OFFLINEMSG];
                                for(string &str : vec){
                                    json js = json::parse(str);
                                    // time + [id] + name + " said: " + xxx
                                    cout << js[TIME] << " [" << js[ID] << "]" << js[NAME] << " said: " << js[MSG] << endl;
                                }
                            }

                            // 登录成功 启动接受线程负责接受数据
                            std::thread readTask(readTaskHandler,clientFd);
                            readTask.detach();
                            // 进入聊天主菜单页面
                            mainMenu(clientFd);
                        }
                    }
                }
            }
                break;

            case 2: // register业务
            {
                char name[50] = {0};
                char pwd[50] = {0};
                cout << "username:";
                cin.getline(name,50);
                cout << "userpassword:";
                cin.getline(pwd,50);

                json js;
                js[MSGID] = REG_MSG;
                js[name] = name;
                js[PWD] = pwd;
                string request = js.dump();

                int len = send(clientFd,request.c_str(), strlen(request.c_str()) + 1,0);
                if (len == -1){
                    cerr << "send reg msg error:" << request << endl;
                }else{
                    char buffer[1024] = {0};
                    len = recv(clientFd,buffer,1024,0);
                    if (-1 == len){
                        cerr << "recv reg response error" << endl;
                    } else{
                        json responseJs = json::parse(buffer);
                        if (0 != responseJs[ERRNO].get<int>()){
                            // 注册失败
                            cerr << name << "is already exist,register error" << endl;
                        }else{
                            // 注册成功
                            cout << name << "register success,userid is " << responseJs[ID] << ",do not forget it" << endl;
                        }
                    }
                }
            }
                break;

            case 3: // quit业务
                close(clientFd);
                exit(0);

            default:
                cerr << "invalid input" << endl;
                break;
        }
    }

    return 0;
}

void showCurrentUserData(){
    cout << "==========================================login user==========================================" << endl;
    cout << "current login user => id:" << g_currentUser.getId() << " name:" << g_currentUser.getName() << endl;
    if (!g_currentUserFriendList.empty()){
        for(User &user : g_currentUserFriendList){
            cout << user.getId() << ' ' << user.getName() << ' ' << user.getState() << endl;
        }
    }
    cout << "==========================================group list==========================================" << endl;
    if (!g_currentUserFriendList.empty()){
        for(Group &group : g_currentUserGroupList){
            cout << group.getId() << ' ' << group.getName() << ' ' << group.getDesc() << endl;
            for(GroupUser &user : group.getUsers()){
                 cout << user.getId() << ' ' << user.getName() << ' ' << user.getState() << user.getRole() << endl;
            }
        }
    }
    cout << "==============================================================================================" << endl;
}

void readTaskHandler(int clientFd){

}

string getCurrentTime(){

}

void mainMenu(int clientFd){
//    help();
//
//    char buffer[1024] = {0};
//    while (isMainMenuRunning){
//        cin.getline(buffer, 1024);
//        string commandbuf(buffer);
//        string command; // 存储命令
//        int idx = commandbuf.find(":");
//        if (-1 == idx)
//        {
//            command = commandbuf;
//        }
//        else
//        {
//            command = commandbuf.substr(0, idx);
//        }
//        auto it = commandHandlerMap.find(command);
//        if (it == commandHandlerMap.end())
//        {
//            cerr << "invalid input command!" << endl;
//            continue;
//        }
//
//        // 调用相应命令的事件处理回调，mainMenu对修改封闭，添加新功能不需要修改该函数
//        it->second(clientfd, commandbuf.substr(idx + 1, commandbuf.size() - idx)); // 调用命令处理方法
//    }
}
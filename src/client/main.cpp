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

// 控制主菜单页面程序
bool isMainMenuRunning = false;

// 用于读写线程之间的通信
sem_t rwsem;

// 记录登录状态
atomic_bool g_isLoginSuccess{false};

// 处理注册的响应逻辑
void doRegResponse(json &responseJs);

// 处理登录的响应逻辑
void doLoginResponse(json &responseJs);

// "help" command handler
void help(int fd = 0, string str = "");
// "chat" command handler
void chat(int, string);
// "addfriend" command handler
void addfriend(int, string);
// "creategroup" command handler
void creategroup(int, string);
// "addgroup" command handler
void addgroup(int, string);
// "groupchat" command handler
void groupchat(int, string);
// "loginOut" command handler
void loginout(int, string);

// 系统支持的客户端命令列表
unordered_map<string, string> commandMap = {
        {"help", "显示所有支持的命令，格式help"},
        {"chat", "一对一聊天，格式chat:friendid:message"},
        {"addfriend", "添加好友，格式addfriend:friendid"},
        {"creategroup", "创建群组，格式creategroup:groupname:groupdesc"},
        {"addgroup", "加入群组，格式addgroup:groupid"},
        {"groupchat", "群聊，格式groupchat:groupid:message"},
        {"loginOut", "注销，格式loginout"}};

// 注册系统支持的客户端命令处理
unordered_map<string, function<void(int, string)>> commandHandlerMap = {
        {"help", help},
        {"chat", chat},
        {"addfriend", addfriend},
        {"creategroup", creategroup},
        {"addgroup", addgroup},
        {"groupchat", groupchat},
        {"loginOut", loginout}};


// 聊天客户段程序实现 main线程用作发送消息线程 子线程用作接收消息线程
int main(int argc,char *argv[]){
    if (argc < 3){
        cerr << "command invalid! example: ./ChatClient 127.0.0.1 6000" << endl;
        exit(-1);
    }

    // 解析通过命令行参数传递的ip和port
    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);

    // 创建client端的socket
    int clientFd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == clientFd){
        cerr << "socket create error" << endl;
        exit(-1);
    }

    // 填写client需要连接的server信息ip+port
    sockaddr_in server;
    memset(&server, 0, sizeof(sockaddr_in));

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);

    // client和server进行连接
    if (-1 == connect(clientFd, (sockaddr *)&server, sizeof(sockaddr_in))){
        cerr << "connect server error" << endl;
        close(clientFd);
        exit(-1);
    }

    // 初始化读写线程通信用的信号量
    sem_init(&rwsem, 0, 0);

    // 连接服务器成功，启动接收子线程
    std::thread readTask(readTaskHandler, clientFd); // pthread_create
    readTask.detach();                               // pthread_detach

    // main线程用于接收用户输入，负责发送数据
    for (;;){
        // 显示首页面菜单 登录、注册、退出
        cout << "========================" << endl;
        cout << "1. login" << endl;
        cout << "2. register" << endl;
        cout << "3. quit" << endl;
        cout << "========================" << endl;
        cout << "choice:";
        int choice = 0;
        cin >> choice;
        cin.get(); // 读掉缓冲区残留的回车

        switch (choice)
        {
            case 1: // login业务
            {
                int id = 0;
                char pwd[50] = {0};
                cout << "userid:";
                cin >> id;
                cin.get(); // 读掉缓冲区残留的回车
                cout << "userpassword:";
                cin.getline(pwd, 50);

                json js;
                js[MSGID] = LOGIN_MSG;
                js[ID] = id;
                js[PWD] = pwd;
                string request = js.dump();

                g_isLoginSuccess = false;

                int len = send(clientFd, request.c_str(), strlen(request.c_str()) + 1, 0);
                if (len == -1){
                    cerr << "send login msg error:" << request << endl;
                }

                sem_wait(&rwsem); // 等待信号量，由子线程处理完登录的响应消息后，通知这里

                if (g_isLoginSuccess){
                    // 进入聊天主菜单页面
                    isMainMenuRunning = true;
                    mainMenu(clientFd);
                }
            }
                break;
            case 2: // register业务
            {
                char name[50] = {0};
                char pwd[50] = {0};
                cout << "username:";
                cin.getline(name, 50);
                cout << "userpassword:";
                cin.getline(pwd, 50);

                json js;
                js[MSGID] = REG_MSG;
                js[NAME] = name;
                js[PWD] = pwd;
                string request = js.dump();

                int len = send(clientFd, request.c_str(), strlen(request.c_str()) + 1, 0);
                if (len == -1){
                    cerr << "send reg msg error:" << request << endl;
                }

                sem_wait(&rwsem); // 等待信号量，子线程处理完注册消息会通知
            }
                break;
            case 3: // quit业务
                close(clientFd);
                sem_destroy(&rwsem);
                exit(0);

            default:
                cerr << "invalid input!" << endl;
                break;
        }
    }
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
    for (;;){
        char buffer[1024] = {0};
        int len = recv(clientFd, buffer, 1024, 0);  // 阻塞了
        if (-1 == len || 0 == len){
            close(clientFd);
            exit(-1);
        }

        // 接收ChatServer转发的数据，反序列化生成json数据对象
        json js = json::parse(buffer);
        int msgtype = js[MSGID].get<int>();
        if (ONE_CHAT_MSG == msgtype){
            cout << js[TIME].get<string>() << " [" << js[ID] << "]" << js[NAME].get<string>()
                 << " said: " << js[MSG].get<string>() << endl;
            continue;
        }

        if (GROUP_CHAT_MSG == msgtype)
        {
            cout << "群消息[" << js[GROUPID] << "]:" << js[TIME].get<string>() << " [" << js[ID] << "]" << js[NAME].get<string>()
                 << " said: " << js[MSG].get<string>() << endl;
            continue;
        }

        if (LOGIN_MSG_ACK == msgtype){
            doLoginResponse(js); // 处理登录响应的业务逻辑
            sem_post(&rwsem);    // 通知主线程，登录结果处理完成
            continue;
        }

        if (REG_MSG_ACK == msgtype){
            doRegResponse(js);
            sem_post(&rwsem);    // 通知主线程，注册结果处理完成
            continue;
        }
    }
}

string getCurrentTime(){
    auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm *ptm = localtime(&tt);
    char date[60] = {0};
    sprintf(date, "%d-%02d-%02d %02d:%02d:%02d",
            (int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,
            (int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
    return std::string(date);
}

void mainMenu(int clientFd){
    help();

    char buffer[1024] = {0};
    while (isMainMenuRunning){
        cin.getline(buffer, 1024);
        string commandbuf(buffer);
        string command; // 存储命令
        int idx = commandbuf.find(":");
        if (-1 == idx){
            command = commandbuf;
        }
        else{
            command = commandbuf.substr(0, idx);
        }
        auto it = commandHandlerMap.find(command);
        if (it == commandHandlerMap.end()){
            cerr << "invalid input command!" << endl;
            continue;
        }

        // 调用相应命令的事件处理回调，mainMenu对修改封闭，添加新功能不需要修改该函数
        it->second(clientFd, commandbuf.substr(idx + 1, commandbuf.size() - idx)); // 调用命令处理方法
    }
}

void doRegResponse(json &responseJs) {
    if (0 != responseJs[ERRNO].get<int>()){
        // 注册失败
        cerr << "name is already exist, register error!" << endl;
    }
    else{
        // 注册成功
        cout << "name register success, userid is " << responseJs[ID]
             << ", do not forget it!" << endl;
    }
}

void doLoginResponse(json &responseJs) {
    if (0 != responseJs[ERRNO].get<int>()){
        // 登录失败
        cerr << responseJs[ERRMSG] << endl;
        g_isLoginSuccess = false;
    }
    else{
        // 登录成功
        // 记录当前用户的id和name
        g_currentUser.setId(responseJs[ID].get<int>());
        g_currentUser.setName(responseJs[NAME]);

        // 记录当前用户的好友列表信息
        if (responseJs.contains(FRIENDS)){
            // 初始化
            g_currentUserFriendList.clear();

            vector<string> vec = responseJs[FRIENDS];
            User user;
            for (string &str : vec){
                json js = json::parse(str);
                user.setId(js[ID].get<int>());
                user.setName(js[NAME]);
                user.setState(js[STATE]);
                g_currentUserFriendList.push_back(user);
            }
        }

        // 记录当前用户的群组列表信息
        if (responseJs.contains(GROUPS)){
            // 初始化
            g_currentUserGroupList.clear();

            vector<string> vec1 = responseJs[GROUPS];
            Group group;
            for (string &groupstr : vec1){
                json grpjs = json::parse(groupstr);
                group.setId(grpjs[ID].get<int>());
                group.setName(grpjs[GROUPNAME]);
                group.setDesc(grpjs[GROUPDESC]);

                vector<string> vec2 = grpjs[USERS];
                for (string &userstr : vec2){
                    GroupUser user;
                    json js = json::parse(userstr);
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

        // 显示当前用户的离线消息  个人聊天信息或者群组消息
        if (responseJs.contains(OFFLINEMSG)){
            vector<string> vec = responseJs[OFFLINEMSG];
            for (string &str : vec){
                json js = json::parse(str);
                // time + [id] + name + " said: " + xxx
                if (ONE_CHAT_MSG == js[MSGID].get<int>()){
                    cout << js[TIME].get<string>() << " [" << js[ID] << "]" << js[NAME].get<string>()
                         << " said: " << js[MSG].get<string>() << endl;
                }
                else{
                    cout << "群消息[" << js[GROUPID] << "]:" << js[TIME].get<string>() << " [" << js[ID] << "]" << js[NAME].get<string>()
                         << " said: " << js[MSG].get<string>() << endl;
                }
            }
        }

        g_isLoginSuccess = true;
    }
}

// "help" command handler
void help(int, string){
    cout << "show command list >>> " << endl;
    for (auto &p : commandMap){
        cout << p.first << " : " << p.second << endl;
    }
    cout << endl;
}

// "addfriend" command handler
void addfriend(int clientFd, string str){
    int friendid = atoi(str.c_str());
    json js;
    js[MSGID] = ADD_FRIEND_MSG;
    js[ID] = g_currentUser.getId();
    js[FRIENDID] = friendid;
    string buffer = js.dump();

    int len = send(clientFd, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
    if (-1 == len){
        cerr << "send addfriend msg error -> " << buffer << endl;
    }
}

// "chat" command handler
void chat(int clientFd, string str){
    int idx = str.find(":"); // friendid:message
    if (-1 == idx){
        cerr << "chat command invalid!" << endl;
        return;
    }

    int friendid = atoi(str.substr(0, idx).c_str());
    string message = str.substr(idx + 1, str.size() - idx);

    json js;
    js[MSGID] = ONE_CHAT_MSG;
    js[ID] = g_currentUser.getId();
    js[NAME] = g_currentUser.getName();
    js[TO] = friendid;
    js[MSG] = message;
    js[TIME] = getCurrentTime();
    string buffer = js.dump();

    int len = send(clientFd, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
    if (-1 == len)
    {
        cerr << "send chat msg error -> " << buffer << endl;
    }
}

// "creategroup" command handler  groupname:groupdesc
void creategroup(int clientFd, string str){
    int idx = str.find(":");
    if (-1 == idx){
        cerr << "creategroup command invalid!" << endl;
        return;
    }

    string groupname = str.substr(0, idx);
    string groupdesc = str.substr(idx + 1, str.size() - idx);

    json js;
    js[MSGID] = CREATE_GROUP_MSG;
    js[ID] = g_currentUser.getId();
    js[GROUPNAME] = groupname;
    js[GROUPDESC] = groupdesc;
    string buffer = js.dump();

    int len = send(clientFd, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
    if (-1 == len){
        cerr << "send creategroup msg error -> " << buffer << endl;
    }
}

// "addgroup" command handler
void addgroup(int clientFd, string str){
    int groupid = atoi(str.c_str());
    json js;
    js[MSGID] = ADD_GROUP_MSG;
    js[ID] = g_currentUser.getId();
    js[GROUPID] = groupid;
    string buffer = js.dump();

    int len = send(clientFd, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
    if (-1 == len){
        cerr << "send addgroup msg error -> " << buffer << endl;
    }
}

// "groupchat" command handler   groupid:message
void groupchat(int clientFd, string str)
{
    int idx = str.find(":");
    if (-1 == idx){
        cerr << "groupchat command invalid!" << endl;
        return;
    }

    int groupid = atoi(str.substr(0, idx).c_str());
    string message = str.substr(idx + 1, str.size() - idx);

    json js;
    js[MSGID] = GROUP_CHAT_MSG;
    js[ID] = g_currentUser.getId();
    js[NAME] = g_currentUser.getName();
    js[GROUPID] = groupid;
    js[MSG] = message;
    js[TIME] = getCurrentTime();
    string buffer = js.dump();

    int len = send(clientFd, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
    if (-1 == len){
        cerr << "send groupchat msg error -> " << buffer << endl;
    }
}

// "loginOut" command handler
void loginout(int clientFd, string){
    json js;
    js[MSGID] = LOGIN_OUT_MSG;
    js[ID] = g_currentUser.getId();
    string buffer = js.dump();

    int len = send(clientFd, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
    if (-1 == len){
        cerr << "send loginOut msg error -> " << buffer << endl;
    }
    else{
        isMainMenuRunning = false;
    }
}
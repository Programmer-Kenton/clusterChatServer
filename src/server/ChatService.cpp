/**
 * @Description 实现业务处理模块
 * @Version 1.0.0
 * @Date 2024/9/10 15:31
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */

#include "ChatService.hpp"

// {"msgid":3,"name":"LCX","password":"123456"} 注册
// {"msgid":1,"id":1,"password":"123456"} LCX登录
// {"msgid":1,"id":2,"password":"123456"} lisi登录
// {"msgid":5,"id":1,"from":"LCX","to":2,"msg":"hello"} LCX给lisi发消息
// {"msgid":6,"id":1,"friendid":2} LCX添加lisi

ChatService *ChatService::instance() {
    static ChatService service;
    return &service;
}

void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int id = js[ID];
    string pwd = js[PWD];


    User user = _userModel.query(id);

    LOG_INFO << "打印查找的user信息 id = " << user.getId() << ",pwd = "
             << user.getPwd() << ",state = " << user.getState();

    json response;

    if (user.getId() == id && user.getPwd() == pwd){
        if (user.getState() == ONLINE){
            // 该用户已登录 不允许重复登录
            response[MSGID] = LOGIN_MSG_ACK;
            response[ERRNO] = OTHER;
            response[ERRMSG] = INLOGIN;
            conn->send(response.dump());
        }else{

            {
                // 登录成功 记录用户连接信息
                lock_guard<mutex> lock(_connMutex);
                _userConnMap.insert({id,conn});
                LOG_INFO << "用户id:" << id << " 连接信息被缓存...";
            }

            // 登录成功后 向redis订阅channel(id)
            _redis.subscribe(id);
            // 别的服务器会把发送给id用户的信息发送到redis id是数据库自动生成唯一的
            // 对应redis唯一的键 值为传递的信息
            LOG_INFO << "登录成功...向redis订阅id为 " << id << "的channel";

            // 登录成功 更新用户状态信息
            user.setState(ONLINE);
            _userModel.updateState(user);
            response[MSGID] = LOGIN_MSG_ACK;
            response[ERRNO] = SUCCESS;
            response[ID] = user.getId();
            response[NAME] = user.getName();

            // 查询用户是否有离线消息
            vector<string> vec = _offlineMsgModel.query(id);
            if (!vec.empty()){
                response[OFFLINEMSG] = vec;
                // 读取用户的离线消息后把该用户的所有离线消息从数据库从删除掉
                _offlineMsgModel.remove(id);
            }

            // 查询用户的好友信息并返回
            vector<User> userVec = _friendModel.query(id);
            if (!userVec.empty()){
                vector<string> vec2;
                json js;
                for(User &user : userVec){
                    js[ID] = user.getId();
                    js[NAME] = user.getName();
                    js[STATE] = user.getState();
                    vec2.push_back(js.dump());
                }
                response[FRIENDS] = vec2;
                LOG_INFO << "登录...查询用户的好友信息并返回";
            }

            conn->send(response.dump());
            LOG_INFO << "用户名为:" << user.getName() << " 登录成功...";
        }
    }else{
        // 登录失败
        response[MSGID] = LOGIN_MSG_ACK;
        response[ERRNO] = FAILED;
        response[ERRMSG] = LOGIN_ERROR;
        conn->send(response.dump());
        LOG_INFO << "用户名为:" << user.getName() << " 登录失败...";
    }
}

void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    string name = js[NAME];
    string pwd = js[PWD];

    User user;
    user.setName(name);
    user.setPwd(pwd);
    json response;
    bool state = _userModel.insert(user);
    if (state){
        // 注册成功
        response[MSGID] = REG_MSG_ACK;
        response[ERRNO] = SUCCESS;
        response[ID] = user.getId();
        conn->send(response.dump());
        LOG_INFO << "用户名为:" << user.getName() << " 注册成功...";
    }else{
        // 注册失败
        response[MSGID] = REG_MSG_ACK;
        response[ERRNO] = FAILED;
        conn->send(response.dump());
        LOG_INFO << "用户名为:" << user.getName() << " 注册失败...";
    }
}

ChatService::ChatService() {
    // 初始化时插入回调消息及绑定的函数
    _msgHandlerMap.insert({LOGIN_MSG,std::bind(&ChatService::login,this,_1,_2,_3)});
    _msgHandlerMap.insert({LOGIN_OUT_MSG, std::bind(&ChatService::loginOut, this, _1, _2, _3)});
    _msgHandlerMap.insert({REG_MSG,std::bind(&ChatService::reg,this,_1,_2,_3)});
    _msgHandlerMap.insert({ONE_CHAT_MSG,std::bind(&ChatService::oneChat,this,_1,_2,_3)});
    _msgHandlerMap.insert({ADD_FRIEND_MSG,std::bind(&ChatService::addFriend,this,_1,_2,_3)});

    // 群组业务管理相关事件处理回调注册
    _msgHandlerMap.insert({CREATE_GROUP_MSG, std::bind(&ChatService::createGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_GROUP_MSG, std::bind(&ChatService::addGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, _1, _2, _3)});

    // 连接redis数据库
    if (_redis.connect()){
        // 设置上报消息的回调
        _redis.init_notify_handler(std::bind(&ChatService::handleRedisSubscribeMessage,this,_1,_2));
    }
}

MsgHandler ChatService::getHandler(int msgId) {
    // 记录错误日志 msgId没有对对应的事件处理回调
    auto it = _msgHandlerMap.find(msgId);
    if (it == _msgHandlerMap.end()){
        // 返回一个默认的空处理器 空操作
        return [=](const TcpConnectionPtr& conn,json& js,Timestamp time){
            LOG_ERROR << "msgId:" << msgId << "can not find handler";
        };
    }else{
        return _msgHandlerMap[msgId];
    }
}

void ChatService::clientCloseException(const TcpConnectionPtr &conn) {
    User user;
    {
        lock_guard<mutex> lock(_connMutex);


        for(auto it = _userConnMap.begin();it != _userConnMap.end();it++){
            if (it->second == conn){
                // 从map表中删除用户的连接信息
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }

    // 用户注销 相当于下线 在redis中取消订阅通道
    _redis.unsubscribe(user.getId());

    // 更新用户的状态信息
    if (user.getId() != -1){
        user.setState(OFFLINE);
        _userModel.updateState(user);
        LOG_INFO << "客户端异常退出...更改用户为离线状态";
    }

}

void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int toId = js[TO].get<int>();
    string msg = js[MSG];

    // LOG_INFO << "测试接收客户端通信信息... 接收用户id:" << toId << " 接收信息:" << msg;
    // 标识用户是否在线
    // bool userState = false;

    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toId);
        if (it != _userConnMap.end()){
            // toId用户在线 转发消息
            // userState = true;
            // 服务器主动推送消息给toId用户
            it->second->send(js.dump());
            LOG_INFO << "给目标用户id:" << toId << " 推送消息:" << msg;
            return;
        }
    }

    // 查询toId是否在线 如果在线就是登录在别的服务器上
    User user = _userModel.query(toId);
    if (user.getState() == ONLINE){
        // 发送消息键为toId的订阅者发送消息
        _redis.publish(toId,js.dump());
        return;
    }

    // toId用户不在线 存储离线消息
    _offlineMsgModel.insert(toId,js.dump());
}

void ChatService::serverCloseException() {
    LOG_INFO << "服务器宕机...开始清理用户状态";
    User user;

    for(auto it = _userConnMap.begin();it != _userConnMap.end();it++){
        user.setId(it->first);
        user.setState(ONLINE);
        if (user.getId() != -1 && user.getState() != OFFLINE){
            user.setState(OFFLINE);
            LOG_INFO << "状态更改成功...准备保存到数据库";
            bool success = _userModel.updateState(user);
            if (success){
                LOG_INFO << "用户状态成功写入数据库";
            }
        }
        _userConnMap.erase(it);
    }

    if (_userConnMap.size() == 0){
        LOG_INFO << "服务器宕机...所有用户都改为离线状态";
    }
}

void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int userId = js[ID].get<int>();
    int friendId = js[FRIENDID].get<int>();

    // 存户好友信息
    _friendModel.insert(userId,friendId);
}

void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int userId = js[ID].get<int>();
    string name = js[GROUPNAME];
    string desc = js[GROUPDESC];

    // 存储新创建的群组消息
    Group group(-1,name,desc);
    if (_groupModel.createGroup(group)){
        LOG_INFO << "创建群组成功..." << " 群名为:" << name;
        // 存储群组创建人信息
        _groupModel.addGroup(userId,group.getId(),"creator");
        LOG_INFO << "创建群组并成功加入群组聊天...";
    }
}

void ChatService::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int userId = js[ID].get<int>();
    int groupId = js[GROUPID].get<int>();
    _groupModel.addGroup(userId,groupId,"normal");
}

void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int userId = js[ID].get<int>();
    int groupId = js[GROUPID].get<int>();
    vector<int> useridVec = _groupModel.queryGroupUsers(userId,groupId);
    for(int id : useridVec){
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(id);
        if (it != _userConnMap.end()){
            // 转发群消息
            it->second->send(js.dump());
        }else{

            // 查询toId是否在线
            User user = _userModel.query(id);
            if (user.getState() == ONLINE){
                _redis.publish(id,js.dump());
            }else{
                // 存储离线群消息
                _offlineMsgModel.insert(id,js.dump());
            }
        }
    }
}

void ChatService::loginOut(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int userid = js[ID].get<int>();

    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(userid);
        if (it != _userConnMap.end())
        {
            _userConnMap.erase(it);
        }
    }

    // 用户注销，相当于就是下线，在redis中取消订阅通道
    _redis.unsubscribe(userid);

    // 更新用户的状态信息
    User user(userid, "", "", "offline");
    _userModel.updateState(user);
}

void ChatService::handleRedisSubscribeMessage(int userId, string msg) {
    // json js = json::parse(msg.c_str()); 不需要反序列化 另一台服务器接收信息会做

    lock_guard<mutex> lock(_connMutex);
    auto it = _userConnMap.find(userId);
    if (it != _userConnMap.end()){
        it->second->send(msg);
        return;
    }

    // 存储用户的离线消息
    _offlineMsgModel.insert(userId,msg);
}

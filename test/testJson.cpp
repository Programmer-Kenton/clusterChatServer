/**
 * @Description 测试Json序列化及反序列化
 * @Version 1.0.0
 * @Date 2024/9/10 9:44
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

// json序列化示例1 普通序列化
void fun1(){
    json js;
    js["msg_type"] = 2;
    js["from"] = "Kenton";
    js["to"] = "Fly Pig";
    js["msg"] = "Coding C++";

    // 把Json序列化为字符串
    string sendBuf = js.dump();

    cout << "sendBuf = " << sendBuf << endl;

    cout << js << endl;
}


// json序列化示例2
void fun2(){
    json js;
    // 添加数组
    js["id"] = {1,2,3,4,5};
    // 添加key-value
    js["name"] = "zhang san";
    // 添加对象
    js["msg"]["zhang san"] = "hello world";
    js["msg"]["li si"] = "Hello TJISE";
    // 上面等同于下面一句话
    js["msg"] = {{"zhang san","hello world"},{"li si","Hello World"}};
    cout << js << endl;
}

// json序列化示例3-序列化容器
int fun3(){
    json js;

    // 直接序列化一个vector容器
    vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(5);
    js["list"] = vec;

    // 直接序列化一个map容器
    map<int,string> m;
    m.insert({1,"天工"});
    m.insert({2,"天软"});
    m.insert({3,"天津现代职业技术学院"});

    js["path"] = m;

    cout << js << endl;
}


// 测试反序列化
string func1(){
    json js;
    js["msg_type"] = 2;
    js["from"] = "Kenton";
    js["to"] = "Fly Pig";
    js["msg"] = "Coding C++";

    // 把Json序列化为字符串
    string sendBuf = js.dump();

    cout << "sendBuf = " << sendBuf << endl;

    cout << js << endl;

    return sendBuf;
}


string func3(){
    json js;

    // 直接序列化一个vector容器
    vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(5);
    js["list"] = vec;

    // 直接序列化一个map容器
    map<int,string> m;
    m.insert({1,"天工"});
    m.insert({2,"天软"});
    m.insert({3,"天津现代职业技术学院"});

    js["path"] = m;

    cout << js << endl;

    string sendBuf = js.dump();

    return sendBuf;
}

int main(){
    // 测试fun1
    fun1();

    cout << "--------------------------------------------------------------------------------------" << endl;

    // 测试fun2
    fun2();

    cout << "--------------------------------------------------------------------------------------" << endl;

    // 测试fun3
    fun3();

    cout << "--------------------------------------------------------------------------------------" << endl;

    // 测试数据反序列化 保留序列化前的数据类型
    string recvBuf = func1();
    json jsBuf = json::parse(recvBuf);
    cout << jsBuf["msg_type"] << endl;
    cout << jsBuf["from"] << endl;
    cout << jsBuf["to"] << endl;
    cout << jsBuf["msg"] << endl;

    cout << "--------------------------------------------------------------------------------------" << endl;

    cout << "测试反序列化容器..." << endl;

    string recvBuf2 = func3();

    vector<int> vec = jsBuf["list"];
    for(int &v : vec){
        cout << v << ' ';
    }
    cout << endl;

    map<int,string> myMap = jsBuf["path"];
    for(auto &p : myMap){
        cout << p.first << ' ' << p.second << endl;
    }
    cout << endl;
}
/**
 * @Description 群组类
 * @Version 1.0.0
 * @Date 2024/9/11 20:03
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */
#ifndef CLUSTERCHATSERVER_GROUP_H
#define CLUSTERCHATSERVER_GROUP_H

#include <string>
#include <vector>
#include "GroupUser.hpp"

using namespace std;

class Group {

public:
    Group(int id = -1, const string name = "", const string desc = "");


    int getId() const;

    void setId(int id);

    string getName() const;

    void setName(const string &name);

    string getDesc() const;

    string setDesc(const string desc);

    vector<GroupUser> &getUsers();

private:
    int id;
    string name;
    string desc;
    vector<GroupUser> users;
};


#endif //CLUSTERCHATSERVER_GROUP_H

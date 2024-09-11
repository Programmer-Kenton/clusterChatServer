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
    Group(int id = -1, const string name = "", const string desc = ""){
        this->id = id;
        this->name = name;
        this->desc = desc;
    }


    int getId() const{
        return this->id;
    }

    void setId(int id){
        this->id = id;
    }

    string getName() const{
        return this->name;
    }

    void setName(const string &name){
        this->name = name;
    }

    string getDesc() const{
        return this->desc;
    }

    string setDesc(const string desc){
        return this->desc;
    }

    vector<GroupUser> &getUsers(){
        return this->users;
    }

private:
    int id;
    string name;
    string desc;
    vector<GroupUser> users;
};


#endif //CLUSTERCHATSERVER_GROUP_H

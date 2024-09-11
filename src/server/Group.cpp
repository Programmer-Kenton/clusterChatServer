/**
 * @Description TODO
 * @Version 1.0.0
 * @Date 2024/9/11 20:03
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */
#include "Group.hpp"

Group::Group(int id, const string name, const string desc) {
    this->id = id;
    this->name = name;
    this->desc = desc;
}

int Group::getId() const {
    return this->id;
}

void Group::setId(int id) {
    this->id = id;
}

string Group::getName() const {
    return this->name;
}

void Group::setName(const string &name) {
    this->name = name;
}

string Group::getDesc() const {
    return this->desc;
}

string Group::setDesc(const string desc) {
    return this->desc;
}

vector<GroupUser> &Group::getUsers(){
    return this->users;
}




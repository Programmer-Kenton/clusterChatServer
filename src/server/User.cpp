/**
 * @Description 实现用户类
 * @Version 1.0.0
 * @Date 2024/9/10 17:40
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */

#include "User.h"

User::User(int id, string name, string pwd, string state) {
    this->id = id;
    this->name = name;
    this->password = password;
    this->state = state;
}

void User::setId(int id) {

    this->id = id;
}

void User::setName(string name) {
    this->name = name;
}

void User::setPwd(string pwd) {
    this->password = pwd;
}

void User::setState(string state) {
    this->state = state;
}

int User::getId() const {
    return id;
}

string User::getName() const {
    return name;
}

string User::getPwd() const {
    return password;
}

string User::getState() const {
    return state;
}

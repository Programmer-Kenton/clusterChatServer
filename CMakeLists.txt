cmake_minimum_required(VERSION 3.30)
project(clusterChatServer)

set(CMAKE_CXX_STANDARD 20)

add_executable(clusterChatServer main.cpp)

# 项目需要使用的第三方库头文件路径
include_directories(${PROJECT_SOURCE_DIR}/thirdparty)

# 测试模块 Json序列化及反序列化
add_executable(testJson test/testJson.cpp thirdparty/json.hpp)
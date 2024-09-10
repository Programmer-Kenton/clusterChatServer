cmake_minimum_required(VERSION 3.30)
project(clusterChatServer)

set(CMAKE_CXX_STANDARD 20)

add_executable(clusterChatServer main.cpp)

# 项目需要使用的第三方库头文件路径
include_directories(${PROJECT_SOURCE_DIR}/thirdparty)

# 配置最终的可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 加载子目录
# 加载测试模块
add_subdirectory(test)
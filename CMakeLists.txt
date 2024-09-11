cmake_minimum_required(VERSION 3.30)
project(clusterChatServer)

set(CMAKE_CXX_STANDARD 20)

# 配置编译选项
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

# 配置最终的可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 项目需要使用的第三方库头文件路径
include_directories(${PROJECT_SOURCE_DIR}/thirdparty)

# 加载自定义的头文件
include_directories(${PROJECT_SOURCE_DIR}/include/db)
include_directories(${PROJECT_SOURCE_DIR}/include/model)
include_directories(${PROJECT_SOURCE_DIR}/include/public)
include_directories(${PROJECT_SOURCE_DIR}/include/server)

## 加载子目录
## 加载测试子模块
add_subdirectory(test)

# 加载项目子模块
add_subdirectory(src)





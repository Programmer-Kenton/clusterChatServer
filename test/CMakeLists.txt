# 定义一个TEST_LIST变量包含了该目录下所有的源文件
# aux_source_directory(. TEST_LIST)


# 指定生成可执行文件

# 测试模块 Json序列化及反序列化
add_executable(testJson testJson.cpp)
# 测试模块 Muduo网络
add_executable(testMuduo testMuduo.cpp)

#指定可执行程序链接时需要依赖的库文件
target_link_libraries(testMuduo muduo_net muduo_base)
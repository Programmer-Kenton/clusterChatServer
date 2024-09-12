#!/bin/bash
# 自动化编译构建项目脚本

# 删除build目录下的文件
echo "Cleaning build directory..."
rm -rf `pwd`/build/*

# 根绝当前路径下的CmakeLists.txt构建项目放入build目录
cmake -S `pwd` -B ./build

# 进入build目录并执行make编译
echo "Compiling project..."
cd `pwd`/build && make

echo "Build completed successfully."


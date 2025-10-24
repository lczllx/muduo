#主配置文件，负责整个项目的构建配置
cmake_minimum_required(VERSION 3.0)
project(lczmuduo)

# 设置编译选项
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -fPIC -Wall -Wextra -pthread")

# 设置库输出路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 配置头文件信息
include_directories(${PROJECT_SOURCE_DIR}/include)

# 加载子目录
add_subdirectory(src)
add_subdirectory(example)
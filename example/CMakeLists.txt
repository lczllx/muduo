# 示例程序目录的CMakeLists.txt
add_executable(test test.cc)
target_link_libraries(test lczmuduo)

# 十万并发连接压测客户端（不依赖 muduo，纯 socket+epoll）
add_executable(concurrent_test concurrent_test.cc)

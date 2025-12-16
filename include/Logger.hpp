#ifndef MUDUO_LOGGER_H
#define MUDUO_LOGGER_H

#include <string>
#include <iostream>

namespace muduo {

enum LogLevel {
    DEBUG = 0,
    INFO,
    WARN,
    ERROR,
    FATAL
};

class Logger {
private:
    LogLevel _level;//日志级别
    std::string _message;//日志消息
    
    static const char* LevelToString(LogLevel level);//将日志级别转换为字符串
    static std::string GetCurrentTime();//获取当前时间

public:
    Logger(LogLevel level);
    ~Logger();
    
    // 支持函数调用语法
    template<typename... Args>
    void operator()(const char* format, Args... args) {
        char buffer[4096];
        snprintf(buffer, sizeof(buffer), format, args...);
        message_ = buffer;
    }
    
    // 支持无参数的调用
    void operator()(const char* msg);
    void operator()(const std::string& msg);
};

}  

// 日志宏 - 返回临时对象，支持函数调用语法
#define L_DEBUG muduo::Logger(muduo::DEBUG)
#define L_INFO  muduo::Logger(muduo::INFO)
#define L_WARN  muduo::Logger(muduo::WARN)
#define L_ERROR muduo::Logger(muduo::ERROR)
#define L_FATAL muduo::Logger(muduo::FATAL)

#endif  
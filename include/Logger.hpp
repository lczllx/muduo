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
    LogLevel _level;
    std::string _message;
    const char* _file;
    int _line;
    
    static const char* LevelToString(LogLevel level);//将日志级别转换为字符串
    static std::string GetCurrentTime();//获取当前时间

public:
    Logger(LogLevel level, const char* file, int line);
    ~Logger();
    
    // 支持函数调用语法
    template<typename... Args>
    void operator()(const char* format, Args... args) {
        char buffer[4096];
        snprintf(buffer, sizeof(buffer), format, args...);
        _message = buffer;
    }
    
    // 支持无参数的调用
    void operator()(const char* msg);
    void operator()(const std::string& msg);
};

}  

// 日志宏 - 带文件名和行号
#define L_DEBUG muduo::Logger(muduo::DEBUG, __FILE__, __LINE__)
#define L_INFO  muduo::Logger(muduo::INFO,  __FILE__, __LINE__)
#define L_WARN  muduo::Logger(muduo::WARN,  __FILE__, __LINE__)
#define L_ERROR muduo::Logger(muduo::ERROR, __FILE__, __LINE__)
#define L_FATAL muduo::Logger(muduo::FATAL, __FILE__, __LINE__)

#endif  
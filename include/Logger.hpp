#ifndef MUDUO_LOGGER_H
#define MUDUO_LOGGER_H

#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <thread>

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
    LogLevel level_;
    std::stringstream ss_;
    
    static const char* LevelToString(LogLevel level) {
        switch(level) {
            case DEBUG: return "DEBUG";
            case INFO:  return "INFO";
            case WARN:  return "WARN";
            case ERROR: return "ERROR";
            case FATAL: return "FATAL";
            default:    return "UNKNOWN";
        }
    }
    
    static std::string GetCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream time_ss;
        time_ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        time_ss << "." << std::setfill('0') << std::setw(3) << ms.count();
        return time_ss.str();
    }

public:
    Logger(LogLevel level) : level_(level) {}
    
    ~Logger() {
        if (level_ >= INFO) {  // 只输出 INFO 及以上级别的日志
            std::cout << "[" << GetCurrentTime() << "]"
                      << "[" << std::this_thread::get_id() << "]"
                      << "[" << LevelToString(level_) << "] "
                      << ss_.str() << std::endl;
        }
        
        if (level_ == FATAL) {
            std::abort();
        }
    }
    
    template<typename T>
    Logger& operator<<(const T& msg) {
        ss_ << msg;
        return *this;
    }
};

}  // namespace muduo

// 日志宏
#define L_DEBUG muduo::Logger(muduo::DEBUG)
#define L_INFO  muduo::Logger(muduo::INFO)
#define L_WARN  muduo::Logger(muduo::WARN)
#define L_ERROR muduo::Logger(muduo::ERROR)
#define L_FATAL muduo::Logger(muduo::FATAL)

#endif  // MUDUO_LOGGER_H
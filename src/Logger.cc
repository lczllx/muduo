#include "../include/Logger.hpp"
#include <sstream>
#include <chrono>
#include <iomanip>
#include <thread>
#include <cstdio>
#include <cstdlib>

namespace muduo {

const char* Logger::LevelToString(LogLevel level) {
    switch(level) {
        case DEBUG: return "DEBUG";
        case INFO:  return "INFO";
        case WARN:  return "WARN";
        case ERROR: return "ERROR";
        case FATAL: return "FATAL";
        default:    return "UNKNOWN";
    }
}

std::string Logger::GetCurrentTime() {
    auto now = std::chrono::system_clock::now();//获取当前时间
    auto time_t = std::chrono::system_clock::to_time_t(now);//将时间转换为时间戳
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;//获取毫秒
    
    std::stringstream time_ss;
    time_ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    time_ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    return time_ss.str();
}

Logger::Logger(LogLevel level, const char* file, int line)
    : _level(level), _file(file), _line(line) {}

Logger::~Logger() {
    if (_level == ERROR || _level == FATAL) {
        std::cout << "[" << GetCurrentTime() << "]"
                  << "[" << std::this_thread::get_id() << "]"
                  << "[" << LevelToString(_level) << "] "
                  << _file << ":" << _line << " "
                  << _message << std::endl;
    }
    
    if (_level == FATAL) {
        std::abort();
    }
}

void Logger::operator()(const char* msg) {
    _message = msg;
}

void Logger::operator()(const std::string& msg) {
    _message = msg;
}

}  // namespace muduo


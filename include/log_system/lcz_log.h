/* 全局接口：日志宏 + 默认 Logger 获取器。日志库已改名为 dlmuduo 私有命名空间
 * （namespace dlmuduo + DLMUDUO_* 宏），避免与宿主工程自带的 lcz 日志库同 TU 冲突。 */
#pragma once
#include "utility.hpp"
#include "level.hpp"
#include "message.hpp"
#include "Logformat.hpp"
#include "sink.hpp"
#include "Logger.hpp"
#include "buffer.hpp"
#include "looper.hpp"

namespace dlmuduo {

inline Logger::ptr getLogger(const std::string& name = "root_logger") {
    Logger::ptr logger = LoggerManager::getInstance().getLogger(name);
    if (logger == nullptr) {
        logger = LoggerManager::getInstance().rootLogger();
    }
    return logger;
}

inline Logger::ptr getrootLogger() {
    return LoggerManager::getInstance().rootLogger();
}

}  // namespace dlmuduo

#define DLMUDUO_DEBUG(fmt, ...) dlmuduo::getrootLogger()->Debug(__FILE__, static_cast<size_t>(__LINE__), fmt, ##__VA_ARGS__)
#define DLMUDUO_INFO(fmt, ...)  dlmuduo::getrootLogger()->Info(__FILE__, static_cast<size_t>(__LINE__), fmt, ##__VA_ARGS__)
#define DLMUDUO_WARN(fmt, ...)  dlmuduo::getrootLogger()->Warn(__FILE__, static_cast<size_t>(__LINE__), fmt, ##__VA_ARGS__)
#define DLMUDUO_ERROR(fmt, ...) dlmuduo::getrootLogger()->Error(__FILE__, static_cast<size_t>(__LINE__), fmt, ##__VA_ARGS__)
#define DLMUDUO_FATAL(fmt, ...) dlmuduo::getrootLogger()->Fatal(__FILE__, static_cast<size_t>(__LINE__), fmt, ##__VA_ARGS__)

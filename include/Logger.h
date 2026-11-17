#pragma once

#include <fstream>
#include <string>

// 日志级别（加分项 B-06：分级输出）
enum class LogLevel { Info, Warn, Error };

// 全局唯一日志器：同时输出到控制台和文件（加分项 B-04）
class Logger {
public:
    static Logger& instance();

    // 打开日志文件；成功返回 true。父目录不存在会自动创建。
    bool setFile(const std::string& path);

    // 写一条日志，格式：[YYYY-MM-DD HH:MM:SS][LEVEL][module] message
    void log(LogLevel level, const std::string& module, const std::string& message);

private:
    Logger() = default;
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    static const char* levelName(LogLevel level);

    std::ofstream file_;   // 未打开时只输出控制台
};

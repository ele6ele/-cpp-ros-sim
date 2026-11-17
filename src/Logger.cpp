#include "Logger.h"

#include <ctime>
#include <filesystem>
#include <iostream>
#include <sstream>

namespace {

// 取当前时间，格式 YYYY-MM-DD HH:MM:SS（与 Message 的时间戳格式一致）
std::string currentTime() {
    time_t t = time(nullptr);
    tm tmv{};
    localtime_s(&tmv, &t);

    char buf[32];
    strftime(buf, sizeof buf, "%Y-%m-%d %H:%M:%S", &tmv);
    return buf;
}

}  // namespace

Logger& Logger::instance() {
    static Logger theOne;   // 第一次调用时创建，之后一直复用
    return theOne;
}

bool Logger::setFile(const std::string& path) {
    // 先确保父目录存在（例如 logs/），否则 ofstream 会静默失败
    const std::size_t slash = path.find_last_of("/\\");
    if (slash != std::string::npos) {
        std::error_code ec;   // 用 error_code 版本，失败不抛异常
        std::filesystem::create_directories(path.substr(0, slash), ec);
    }

    file_.open(path, std::ios::app);   // 追加：多次运行不覆盖历史日志
    return file_.is_open();
}

const char* Logger::levelName(LogLevel level) {
    switch (level) {
    case LogLevel::Info:  return "INFO";
    case LogLevel::Warn:  return "WARN";
    case LogLevel::Error: return "ERROR";
    }
    return "INFO";
}

void Logger::log(LogLevel level, const std::string& module, const std::string& message) {
    std::ostringstream oss;
    oss << '[' << currentTime() << "][" << levelName(level) << "][" << module << "] " << message;
    const std::string line = oss.str();

    std::cout << line << std::endl;          // ① 控制台

    if (file_.is_open()) {                   // ② 文件（同一份字符串，保证两边一致）
        file_ << line << std::endl;
        file_.flush();                       // 立即落盘，防止异常退出丢日志
    }
}

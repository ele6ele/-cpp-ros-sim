#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>   // SetConsoleOutputCP：中文输出必需，否则控制台按 GBK 解释 UTF-8 → 乱码

#include <iostream>
#include <string>

#include "Exception.h"
#include "Logger.h"
#include "Message.h"
#include "Node.h"
#include "Publisher.h"
#include "RosCore.h"
#include "Subscriber.h"
#include "TemperatureMessage.h"

namespace {

void initConsole() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}

// 阶段标题也走日志，保证控制台和文件内容一致
void stage(const std::string& title) {
    Logger::instance().log(LogLevel::Info, "Main", "=== " + title + " ===");
}

}  // namespace

int main() {
    initConsole();   // 必须在任何输出之前调用

    // 日志落盘（加分项 B-04）：写 logs/run.log，追加模式
    if (!Logger::instance().setFile("logs/run.log")) {
        std::cout << "[提示] 日志文件打开失败，本次仅输出到控制台\n";
    }

    try {
        // 5 个节点：名字必须各不相同（重名会被 register 拦下，那是阶段 3 才故意做的事）
        Publisher  tempPublisher("温度发布者", "temp1");
        Publisher  humPublisher ("湿度发布者", "hum1");

        Subscriber screen  ("显示屏",   "temp1");
        Subscriber recorder("日志记录", "temp1");

        Subscriber humPanel("湿度面板", "hum1");

        // ---------- 阶段 1：初始化 ----------
        stage("阶段 1：初始化");
        try {
            tempPublisher.init();
            humPublisher.init();

            screen.init();
            recorder.init();
            humPanel.init();
        } catch (const NodeException& e) {
            Logger::instance().log(LogLevel::Warn, "Main", std::string("节点注册被拦截：") + e.what());
        }

        // ---------- 阶段 2：连续发布 ----------
        stage("阶段 2：连续发布");
        try {
            tempPublisher.publish("16℃");
            tempPublisher.publish("18℃");
            tempPublisher.publish("19℃");

            humPublisher.publish("60%");

            // 加分项 B-01：发一条"自定义类型"的消息（带温度、湿度），订阅者按类型处理
            TemperatureMessage reading(26.5, 61.0);
            RosCore::instance().forwardMessage("temp1", reading);
        } catch (const RosException& e) {
            Logger::instance().log(LogLevel::Error, "Main", std::string("发布失败：") + e.what());
        }

        // ---------- 阶段 3：异常与边界演示 ----------
        stage("阶段 3：异常与边界演示");

        //一、不可重名
        try {
            Publisher tempPublisher0("温度发布者", "temp0");
            tempPublisher0.init();
        } catch (const NodeException& e) {
            Logger::instance().log(LogLevel::Warn, "Main", std::string("重名被拦截：") + e.what());
        }

        //二、标题不可为空
        try {
            Publisher tempPublisher01("临时节点", "  ");   // 名字合法，话题空白
            tempPublisher01.init();
        } catch (const TopicException& e) {
            Logger::instance().log(LogLevel::Warn, "Main", std::string("空白话题被拦截：") + e.what());
        }

        //三、消息内容不可为空
        try {
            Message NONE = Message("   ");   //消息内容为空白
            RosCore::instance().forwardMessage("示例标题", NONE);
        } catch (const MessageException& e) {
            Logger::instance().log(LogLevel::Warn, "Main", std::string("空白消息被拦截：") + e.what());
        }

        //四、消息无人订阅（话题存在，但没有任何订阅者）
        Publisher alarmPublisher("示警发布者", "alarm1");
        alarmPublisher.init();
        alarmPublisher.publish("示警消息");

        //五、不存在的话题
        try {
            Message ONE = Message("示例消息");
            RosCore::instance().forwardMessage("示例标题", ONE);
        } catch (const TopicException& e) {
            Logger::instance().log(LogLevel::Warn, "Main", std::string("不存在的话题被拦截：") + e.what());
        }

        // ---------- 阶段 4：统一关闭 ----------
        stage("阶段 4：统一关闭");
        RosCore::instance().shutdownAll();
    }
    catch (const RosException& e) {     // 顶层兜底：项目异常（加分项 B-05）
        Logger::instance().log(LogLevel::Error, "Main", std::string("未捕获的项目异常：") + e.what());
    }
    catch (const std::exception& e) {   // 标准库异常
        Logger::instance().log(LogLevel::Error, "Main", std::string("标准库异常：") + e.what());
    }
    catch (...) {                       // 其它未知异常
        Logger::instance().log(LogLevel::Error, "Main", "未知异常");
    }

    return 0;
}

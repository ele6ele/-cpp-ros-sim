#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>   // SetConsoleOutputCP：中文输出必需，否则控制台按 GBK 解释 UTF-8 → 乱码

#include <iostream>
#include <string>

#include "Exception.h"
#include "Message.h"
#include "Node.h"
#include "Publisher.h"
#include "RosCore.h"
#include "Subscriber.h"

namespace {

void initConsole() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}

// ============================================================
//  选项 1：完整流程演示（原来的五阶段，原样保留）
//  每次进入都会用一套新的局部节点，结束时统一关闭，可反复运行
// ============================================================
void runAutoDemo() {
    std::cout << "\n########## 完整流程演示 ##########\n";

    // 5 个节点：名字必须各不相同（重名会被 register 拦下，那是阶段 4 才故意做的事）
    Publisher  tempPublisher("温度发布者", "temp1");
    Publisher  humPublisher ("湿度发布者", "hum1");

    Subscriber screen  ("显示屏",   "temp1");
    Subscriber recorder("日志记录", "temp1");

    Subscriber humPanel("湿度面板", "hum1");

    // ---------- 阶段 1：初始化 ----------
    std::cout << "\n=== 阶段 1：初始化 ===\n";
    try {
        tempPublisher.init();
        humPublisher.init();

        screen.init();
        recorder.init();
        humPanel.init();
    } catch (const NodeException& e) {
        std::cout << "[OK] 重复注册被拦截：" << e.what() << "\n";
    } catch (const RosException& e) {
        std::cout << "[外层兜底] " << e.what() << "\n";
    }

    // ---------- 阶段 2：连续发布 ----------
    std::cout << "\n=== 阶段 2：连续发布 ===\n";
    try {
        tempPublisher.publish("16℃");
        tempPublisher.publish("18℃");
        tempPublisher.publish("19℃");

        humPublisher.publish("60%");
    } catch (const RosException& e) {
        std::cout << "[外层兜底] " << e.what() << "\n";
    }

    // ---------- 阶段 3：无人订阅的话题 ----------
    std::cout << "\n=== 阶段 3：无人订阅的话题 ===\n";
    Message ONE = Message("示例消息");
    RosCore::instance().forwardMessage("示例标题", ONE);

    // ---------- 阶段 4：异常演示 ----------
    std::cout << "\n=== 阶段 4：异常演示 ===\n";
    //一、不可重名
    try {
        Publisher tempPublisher0("温度发布者", "temp0");
        tempPublisher0.init();
    } catch (const NodeException& e) {
        std::cout << "[OK]重名消息被拦截：" << e.what() << "\n";
    }

    //二、标题不可为空
    try {
        Publisher tempPublisher01("临时节点", "  ");   // 名字合法，话题空白
        tempPublisher01.init();
    } catch (const TopicException& e) {
        std::cout << "[OK]空白标题被拦截：" << e.what() << "\n";
    }

    //三、消息内容不可为空
    try {
        Message NONE = Message("   ");
        RosCore::instance().forwardMessage("示例标题", NONE);
    } catch (const MessageException& e) {
        std::cout << "[WARN]" << e.what() << "\n";
    }

    // ---------- 阶段 5：统一关闭 ----------
    std::cout << "\n=== 阶段 5：统一关闭 ===\n";
    RosCore::instance().shutdownAll();
}

// ============================================================
//  选项 2~6：单项错误演示
//  每个都自成一个场景：造对象 -> 触发错误 -> 接住 -> 清理
// ============================================================

void demoEmptyName() {
    std::cout << "\n--- 空节点名 ---\n";
    try {
        Publisher p("", "demo_topic");
        std::cout << "[FAIL] 空节点名居然通过了\n";
    } catch (const NodeException& e) {
        std::cout << "[OK] 被拦截（NodeException）：" << e.what() << "\n";
    }
}

void demoBlankTopic() {
    std::cout << "\n--- 空白话题名 ---\n";
    try {
        Publisher p("临时演示节点", "   ");
        std::cout << "[FAIL] 空白话题居然通过了\n";
    } catch (const TopicException& e) {
        std::cout << "[OK] 被拦截（TopicException）：" << e.what() << "\n";
    }
}

void demoEmptyMessage() {
    std::cout << "\n--- 空消息内容 ---\n";
    try {
        Message m("   ");
        std::cout << "[FAIL] 空消息居然通过了\n";
    } catch (const MessageException& e) {
        std::cout << "[OK] 被拦截（MessageException）：" << e.what() << "\n";
    }
}

void demoDuplicateName() {
    std::cout << "\n--- 重复注册节点名 ---\n";
    Publisher a("重名测试节点", "dup1");
    RosCore::instance().registerPublisher(a);      // 第一次：成功
    try {
        RosCore::instance().registerPublisher(a);  // 第二次：应抛 NodeException
        std::cout << "[FAIL] 重复注册居然成功了\n";
    } catch (const NodeException& e) {
        std::cout << "[OK] 被拦截（NodeException）：" << e.what() << "\n";
    }
    // 关键：a 是局部对象，函数返回前必须把名册清空，否则名册里会留下悬空指针
    RosCore::instance().shutdownAll();
}

void demoUnknownTopic() {
    std::cout << "\n--- 转发到无人订阅的话题 ---\n";
    Message m("没人听");
    RosCore::instance().forwardMessage("/no/such/topic", m);   // 只 WARN，不抛异常
    std::cout << "（上面那条 WARN 就是「转发到无人话题」的结果，程序没有崩溃）\n";
}

void demoCustomInput() {
    std::cout << "\n--- 自己输入节点名与话题名 ---\n";
    std::string name, topic;
    std::cout << "请输入节点名（直接回车 = 空串）：";
    if (!std::getline(std::cin, name)) return;
    std::cout << "请输入话题名（直接回车 = 空串）：";
    if (!std::getline(std::cin, topic)) return;

    try {
        Publisher p(name, topic);      // 只构造、不注册，避免局部对象进名册
        std::cout << "[合法] 构造成功：" << p.getName()
                  << " -> " << p.getTopic() << "（本次未注册）\n";
    } catch (const RosException& e) {
        std::cout << "[OK] 被拦截：" << e.what() << "\n";
    }
}

// ============================================================
//  菜单
// ============================================================
void printMenu() {
    std::cout << "\n============== C++ 模拟 ROS 节点系统 ==============\n"
                 "  1  完整流程演示（初始化→收发→异常→关闭）\n"
                 "  2  空节点名        -> NodeException\n"
                 "  3  空白话题名      -> TopicException\n"
                 "  4  空消息内容      -> MessageException\n"
                 "  5  重复注册节点名  -> NodeException\n"
                 "  6  转发到无人话题  -> WARN（不抛异常）\n"
                 "  7  自己输入名字/话题\n"
                 "  0  退出\n"
                 "===================================================\n";
}

// 一步执行完，停一下让用户看清输出，再回菜单
// 返回 false 表示输入结束（如重定向了空输入），主循环应退出
bool pauseAndBack() {
    std::cout << "\n按回车返回菜单（输入 0 退出）：";
    std::string line;
    if (!std::getline(std::cin, line)) return false;
    return line.empty() || line[0] != '0';
}

}  // namespace

int main() {
    initConsole();   // 必须在任何输出之前调用

    for (;;) {
        printMenu();
        std::cout << "请选择：";

        std::string line;
        if (!std::getline(std::cin, line)) break;   // 输入结束 -> 退出
        if (line.empty()) continue;                 // 空行 -> 重新显示菜单

        switch (line[0]) {
        case '0':
            std::cout << "\n退出前统一关闭所有节点：\n";
            RosCore::instance().shutdownAll();
            return 0;

        case '1': runAutoDemo();                       break;
        case '2': demoEmptyName();                     break;
        case '3': demoBlankTopic();                    break;
        case '4': demoEmptyMessage();                  break;
        case '5': demoDuplicateName();                 break;
        case '6': demoUnknownTopic();                  break;
        case '7': demoCustomInput();                   break;

        default:
            std::cout << "\n（没有这个选项，请重新输入）\n";
            continue;
        }

        if (!pauseAndBack()) break;   // 每步之后停一下，回菜单
    }

    // 输入结束（Ctrl+Z / 重定向空输入）也要清理一次，避免名册里留悬空指针
    RosCore::instance().shutdownAll();
    return 0;
}

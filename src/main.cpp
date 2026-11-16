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

// 把控制台代码页切成 UTF-8（65001）。Windows 中文环境固定写法，照抄即可。
void initConsole() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}

// ============================================================
//  单项错误演示（供交互菜单调用）
//  注意：这里都用"构造后不注册"或"直接调用注册接口"的方式，
//        避免临时对象进名册后销毁、留下悬空指针。
// ============================================================

void demoEmptyName() {
    try {
        Publisher p("", "demo_topic");           // 空节点名 -> Node 构造函数抛
        std::cout << "  [FAIL] 空节点名居然通过了\n";
    } catch (const NodeException& e) {
        std::cout << "  [OK] NodeException：" << e.what() << "\n";
    }
}

void demoBlankTopic() {
    try {
        Publisher p("临时演示节点", "   ");        // 全空白话题 -> Publisher 构造函数抛
        std::cout << "  [FAIL] 空白话题居然通过了\n";
    } catch (const TopicException& e) {
        std::cout << "  [OK] TopicException：" << e.what() << "\n";
    }
}

void demoEmptyMessage() {
    try {
        Message m("");                            // 空内容 -> Message 构造函数抛
        std::cout << "  [FAIL] 空消息居然通过了\n";
    } catch (const MessageException& e) {
        std::cout << "  [OK] MessageException：" << e.what() << "\n";
    }
}

void demoDuplicateName(Publisher& registered) {
    try {
        RosCore::instance().registerPublisher(registered);   // 把已注册的节点再登记一次
        std::cout << "  [FAIL] 重复注册居然成功了\n";
    } catch (const NodeException& e) {
        std::cout << "  [OK] NodeException：" << e.what() << "\n";
    }
}

void demoUnknownTopic() {
    Message m("没人听");
    RosCore::instance().forwardMessage("/no/such/topic", m);   // 只打 WARN，不抛异常
    std::cout << "  （上面那条 WARN 就是「转发到无人话题」的结果，程序没有崩）\n";
}

// 现场手动输入：只构造、不注册（避免局部对象进名册后销毁造成悬空）
void demoManualInput() {
    std::string name, topic;
    std::cout << "  请输入节点名（直接回车 = 空串）：";
    if (!std::getline(std::cin, name)) return;
    std::cout << "  请输入话题名（直接回车 = 空串）：";
    if (!std::getline(std::cin, topic)) return;

    try {
        Publisher p(name, topic);
        std::cout << "  [合法] 构造成功：" << p.getName()
                  << " -> " << p.getTopic() << "（未注册）\n";
    } catch (const RosException& e) {
        std::cout << "  [OK] 被拦截：" << e.what() << "\n";
    }
}

void runInteractiveDemo(Publisher& registered) {
    std::cout << "\n========== 交互式错误演示（现场演示用）==========\n"
                 "  1  空节点名              -> NodeException\n"
                 "  2  空白话题名            -> TopicException\n"
                 "  3  空消息内容            -> MessageException\n"
                 "  4  重复注册已有节点名    -> NodeException\n"
                 "  5  转发到不存在的话题    -> WARN（不抛异常）\n"
                 "  6  手动输入名字 / 话题   -> 现场试\n"
                 "  0  退出演示\n";

    for (;;) {
        std::cout << "\n请选择（0 退出）：";
        std::string line;
        if (!std::getline(std::cin, line)) break;   // 输入结束（如重定向了空输入）→ 自动退出
        if (line.empty()) break;                    // 直接回车也退出
        switch (line[0]) {
        case '1': demoEmptyName(); break;
        case '2': demoBlankTopic(); break;
        case '3': demoEmptyMessage(); break;
        case '4': demoDuplicateName(registered); break;
        case '5': demoUnknownTopic(); break;
        case '6': demoManualInput(); break;
        case '0': return;
        default:  std::cout << "  （没有这个选项）\n"; break;
        }
    }
}

}  // namespace

int main() {
    initConsole();   // 必须在任何输出之前调用

    // 5 个节点：名字必须各不相同
    Publisher  tempPublisher("温度发布者", "temp1");
    Publisher  humPublisher ("湿度发布者", "hum1");

    Subscriber screen  ("显示屏",   "temp1");
    Subscriber recorder("日志记录", "temp1");

    Subscriber humPanel("湿度面板", "hum1");

    // ---------- 阶段 1：初始化 ----------
    std::cout << "=== 阶段 1：初始化 ===\n";
    try {
        tempPublisher.init();
        humPublisher.init();

        screen.init();
        recorder.init();
        humPanel.init();
    } catch (const NodeException& e) {
        std::cout << "[错误] 节点注册失败：" << e.what() << "\n";
    }

    // ---------- 阶段 2：连续发布 ----------
    std::cout << "\n=== 阶段 2：连续发布 ===\n";
    try {
        tempPublisher.publish("16℃");
        tempPublisher.publish("18℃");
        tempPublisher.publish("19℃");
        humPublisher.publish("60%");
    } catch (const RosException& e) {
        std::cout << "[错误] 发布失败：" << e.what() << "\n";
    }

    // ---------- 阶段 3：无人订阅的话题 ----------
    std::cout << "\n=== 阶段 3：无人订阅的话题 ===\n";
    Message nobody("示例消息");
    RosCore::instance().forwardMessage("/no/such/topic", nobody);

    // ---------- 阶段 4：异常演示 ----------
    std::cout << "\n=== 阶段 4：异常演示 ===\n";

    // ① 节点重名：init 时注册会抛 NodeException
    try {
        Publisher dup("温度发布者", "temp0");
        dup.init();
        std::cout << "[FAIL] 重名节点居然注册成功了\n";
    } catch (const NodeException& e) {
        std::cout << "[OK] 节点重名被拦截：" << e.what() << "\n";
    }

    // ② 空白话题：名字合法、话题全空白 -> 构造函数抛 TopicException
    try {
        Publisher badTopic("临时节点", "   ");
        std::cout << "[FAIL] 空白话题居然通过了\n";
    } catch (const TopicException& e) {
        std::cout << "[OK] 空话题被拦截：" << e.what() << "\n";
    }

    // ③ 空消息：内容为空串 -> 构造函数抛 MessageException
    try {
        Message badMessage("");
        std::cout << "[FAIL] 空消息居然通过了\n";
    } catch (const MessageException& e) {
        std::cout << "[OK] 空消息被拦截：" << e.what() << "\n";
    }

    // 交互菜单：现场演示时可用；直接回车或输入 0 结束
    runInteractiveDemo(tempPublisher);

    // ---------- 阶段 5：统一关闭 ----------
    std::cout << "\n=== 阶段 5：统一关闭 ===\n";
    RosCore::instance().shutdownAll();

    return 0;
}

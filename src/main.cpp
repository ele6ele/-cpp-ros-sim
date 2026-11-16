#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>   // SetConsoleOutputCP：中文输出必需，否则控制台按 GBK 解释 UTF-8 → 乱码

#include <iostream>

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
//  【新增】交互式错误演示：现场演示用
//  这些函数只负责"演示一个错误"，不改变你原有流程
// ============================================================

void demoEmptyName() {
    try {
        Publisher p("", "demo_topic");
        std::cout << "  [FAIL] 空节点名居然通过了\n";
    } catch (const NodeException& e) {
        std::cout << "  [OK] NodeException：" << e.what() << "\n";
    }
}

void demoBlankTopic() {
    try {
        Publisher p("临时演示节点", "   ");
        std::cout << "  [FAIL] 空白话题居然通过了\n";
    } catch (const TopicException& e) {
        std::cout << "  [OK] TopicException：" << e.what() << "\n";
    }
}

void demoEmptyMessage() {
    try {
        Message m("");
        std::cout << "  [FAIL] 空消息居然通过了\n";
    } catch (const MessageException& e) {
        std::cout << "  [OK] MessageException：" << e.what() << "\n";
    }
}

void demoDuplicateName(Publisher& registered) {
    try {
        RosCore::instance().registerPublisher(registered);
        std::cout << "  [FAIL] 重复注册居然成功了\n";
    } catch (const NodeException& e) {
        std::cout << "  [OK] NodeException：" << e.what() << "\n";
    }
}

void demoUnknownTopic() {
    Message m("没人听");
    RosCore::instance().forwardMessage("/no/such/topic", m);
    std::cout << "  （上面那条 WARN 就是「转发到无人话题」的结果，程序没有崩）\n";
}

// 现场手动输入：只构造、不注册
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
        if (!std::getline(std::cin, line)) break;
        if (line.empty()) break;
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

    // 5 个节点：名字必须各不相同（重名会被 register 拦下，那是阶段 4 才故意做的事）
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
        std::cout << "[OK] 重复注册被拦截：" << e.what() << "\n";
    } catch (const RosException& e) {
        std::cout << "[外层兜底] " << e.what() << "\n";
    }

    // ---------- 阶段 2：连续发布 ----------
    std::cout << "\n=== 阶段 2：连续发布 ===\n";
    try{

        tempPublisher.publish("16℃");
        tempPublisher.publish("18℃");
        tempPublisher.publish("19℃");

        humPublisher.publish("60%");
    }catch (const RosException& e) {
        std::cout << "[外层兜底] " << e.what() << "\n";
    }

    // ---------- 阶段 3：无人订阅的话题 ----------
    std::cout << "\n=== 阶段 3：无人订阅的话题 ===\n";

    Message ONE = Message("示例消息");
    RosCore::instance().forwardMessage("示例标题",ONE);

    // ---------- 阶段 4：异常演示 ----------
    std::cout << "\n=== 阶段 4：异常演示 ===\n";

        //一、不可重名
        try{
            Publisher  tempPublisher0("温度发布者", "temp0");
            tempPublisher0.init();
        }catch(const NodeException& e){
            std::cout<<"[OK]重名消息被拦截："<<e.what()<<"\n";
        }

        //二、标题不可为空

        try{
            Publisher  tempPublisher01("临时节点", "  ");   // 名字合法，话题空白
            tempPublisher01.init();
        }catch(const TopicException& e){
            std::cout<<"[OK]空白标题被拦截："<<e.what()<<"\n";
        }

        //三、消息内容不可为空

        try{
            Message NONE = Message("   ");                  // 你原来的写法：全空白。Message 升级校验后同样会被拦
            RosCore::instance().forwardMessage("示例标题",NONE);
        }catch(const MessageException& e){
            std::cout<<"[WARN]"<<e.what()<<"\n";
        }

    // ---------- 阶段 4.5：交互式演示（新增）----------
    runInteractiveDemo(tempPublisher);

    // ---------- 阶段 5：统一关闭 ----------
    std::cout << "\n=== 阶段 5：统一关闭 ===\n";
    RosCore::instance().shutdownAll();

    return 0;
}

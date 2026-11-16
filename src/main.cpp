#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>   // SetConsoleOutputCP：中文输出必需，否则控制台按 GBK 解释 UTF-8 → 乱码

#include <iostream>
#include <string>
#include <vector>

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

// 读一行；返回 false 表示输入结束（EOF），主循环应当退出
bool readLine(const char* prompt, std::string& out) {
    std::cout << prompt;
    return static_cast<bool>(std::getline(std::cin, out));
}

// 空串或全空白都算"没填"
bool isBlank(const std::string& s) {
    return s.find_first_not_of(" \t\n\r") == std::string::npos;
}

// 反复询问，直到拿到非空白内容；输入 q 取消
bool askNonBlank(const char* prompt, const char* fieldName, std::string& out) {
    for (;;) {
        if (!readLine(prompt, out)) return false;
        if (out == "q" || out == "Q") return false;
        if (!isBlank(out)) return true;
        std::cout << "[错误] " << fieldName << "不能为空或全为空白，请重新输入。\n";
    }
}

// ============================================================
//  选项 1：作为发布者发布消息
//  输错不会退出程序，而是回到"上一步"重新输入
// ============================================================
void publishFlow() {
    std::cout << "\n---------- 作为发布者发布消息 ----------\n"
                 "（任意一步输入 q 可取消，返回菜单）\n";

    std::string topic, content;

    // 第一步：标题（话题名）—— 空/全空白会报错并重新询问
    if (!askNonBlank("请输入标题（话题名）：", "标题", topic)) {
        std::cout << "（已取消，返回菜单）\n";
        return;
    }

    // 第二步：内容 —— 同样校验
    if (!askNonBlank("请输入消息内容：", "内容", content)) {
        std::cout << "（已取消，返回菜单）\n";
        return;
    }

    // 第三步：真正发布（构造时还会再校验一次话题，双保险）
    try {
        Publisher temp("交互发布者", topic);   // 未注册：避免临时节点残留在名册里
        temp.publish(content);                 // 内部构造 Message 并转发给订阅者
        std::cout << "（发布完成；若无人订阅该话题，上方会有一条 [WARN]）\n";
    } catch (const RosException& e) {
        std::cout << "[错误] 发布失败：" << e.what() << "\n";
    }
}

// ============================================================
//  选项 2：作为订阅者查看消息
// ============================================================
void showInboxFlow(const std::vector<Subscriber*>& subs) {
    std::cout << "\n---------- 作为订阅者查看消息 ----------\n";

    for (std::size_t i = 0; i < subs.size(); ++i) {
        std::cout << "  " << (i + 1) << "  " << subs[i]->getName()
                  << "（话题 " << subs[i]->getTopic()
                  << "，已收 " << subs[i]->getInbox().size() << " 条）\n";
    }

    std::string line;
    if (!readLine("请选择订阅者编号（0 返回菜单）：", line)) return;
    if (line.empty() || line[0] == '0') return;

    const int idx = line[0] - '1';
    if (idx < 0 || idx >= static_cast<int>(subs.size())) {
        std::cout << "[错误] 没有这个编号。\n";
        return;
    }
    subs[idx]->printInbox();
}

// ============================================================
//  选项 3：完整流程演示（原五阶段，原样保留）
//  每次进入都用一套新的局部节点，结束时统一关闭，可反复运行
// ============================================================
void runAutoDemo() {
    std::cout << "\n########## 完整流程演示 ##########\n";

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
        Publisher tempPublisher01("临时节点", "  ");
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

}  // namespace

int main() {
    initConsole();   // 必须在任何输出之前调用

    // 会话级的订阅者：整个程序运行期间一直存在
    Subscriber screen  ("显示屏",   "temp1");
    Subscriber recorder("日志记录", "temp1");
    std::vector<Subscriber*> subs{ &screen, &recorder };

    std::cout << "\n=== 系统启动：初始化会话订阅者 ===\n";
    screen.init();
    recorder.init();

    for (;;) {
        std::cout << "\n============== C++ 模拟 ROS 节点系统 ==============\n"
                     "  1  作为发布者发布消息\n"
                     "  2  作为订阅者查看消息\n"
                     "  3  完整流程演示（初始化→收发→异常→关闭）\n"
                     "  0  退出\n"
                     "===================================================\n"
                     "请选择：";

        std::string line;
        if (!std::getline(std::cin, line)) break;   // 输入结束 -> 退出
        if (line.empty()) continue;

        switch (line[0]) {
        case '1':
            publishFlow();
            break;

        case '2':
            showInboxFlow(subs);
            break;

        case '3':
            // 演示用的是另一套同名节点（显示屏/日志记录），先清空会话注册，避免重名冲突
            std::cout << "\n（进入完整演示：先清空当前会话注册）\n";
            RosCore::instance().shutdownAll();

            runAutoDemo();

            std::cout << "\n（演示结束：重新注册会话订阅者）\n";
            try {
                screen.init();
                recorder.init();
            } catch (const RosException& e) {
                std::cout << "[错误] 重新注册失败：" << e.what() << "\n";
            }
            break;

        case '0':
            std::cout << "\n退出前统一关闭：\n";
            RosCore::instance().shutdownAll();
            return 0;

        default:
            std::cout << "（没有这个选项，请重新输入）\n";
            break;
        }
    }

    RosCore::instance().shutdownAll();
    return 0;
}

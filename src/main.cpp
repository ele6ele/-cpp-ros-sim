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

// 读一行；返回 false 表示输入结束（EOF）
bool readLine(const char* prompt, std::string& out) {
    std::cout << prompt;
    return static_cast<bool>(std::getline(std::cin, out));
}

bool isBlank(const std::string& s) {
    return s.find_first_not_of(" \t\n\r") == std::string::npos;
}

bool isCancel(const std::string& s) {
    return s == "q" || s == "Q" || s == "0";   // q 或 0 都表示取消
}

// ============================================================
//  选项 1：作为发布者发布消息
//  引导式输入：节点名 -> 标题(话题) -> 内容
//  任何一步输入空白（或节点名重复）都会报错，并【回到那一步】重新输入
// ============================================================
void publishFlow() {
    std::cout << "\n---------- 作为发布者发布消息 ----------\n"
                 "（任意一步输入 q 可取消，返回菜单）\n";

    std::string name, topic, content;

    // 第 1 步：发布者节点名
    for (;;) {
        if (!readLine("请输入发布者节点名：", name)) return;
        if (isCancel(name)) { std::cout << "（已取消，返回菜单）\n"; return; }
        if (isBlank(name)) {
            std::cout << "[错误] 节点名不能为空或全为空白，请重新输入。\n";
            continue;                       // ← 回到这一步，重新问节点名
        }
        if (RosCore::instance().isNameRegistered(name)) {
            std::cout << "[错误] 节点名「" << name << "」已被占用，请换一个。\n";
            continue;                       // ← 同样回到这一步
        }
        break;
    }

    // 第 2 步：标题（话题名）
    for (;;) {
        if (!readLine("请输入标题（话题名）：", topic)) return;
        if (isCancel(topic)) { std::cout << "（已取消，返回菜单）\n"; return; }
        if (isBlank(topic)) {
            std::cout << "[错误] 标题不能为空或全为空白，请重新输入。\n";
            continue;                       // ← 回到这一步，重新问标题
        }
        break;
    }

    // 第 3 步：消息内容
    for (;;) {
        if (!readLine("请输入消息内容：", content)) return;
        if (isCancel(content)) { std::cout << "（已取消，返回菜单）\n"; return; }
        if (isBlank(content)) {
            std::cout << "[错误] 消息内容不能为空或全为空白，请重新输入。\n";
            continue;                       // ← 回到这一步，重新问内容
        }
        break;
    }

    // 三步都合法，真正发布
    try {
        // 临时发布者：只用于校验与打日志，不注册（注册后局部对象销毁会留下悬空指针）
        Publisher temp(name, topic);
        temp.publish(content);
        std::cout << "（发布完成；若该话题无人订阅，上方会有一条 [WARN]）\n";
    } catch (const RosException& e) {
        std::cout << "[错误] 发布失败：" << e.what() << "\n";
    }
}

// ============================================================
//  选项 2：作为订阅者查看消息
//  输入订阅者节点名 -> 显示它的收件箱
// ============================================================
void viewFlow(const std::vector<Subscriber*>& subs) {
    std::cout << "\n---------- 作为订阅者查看消息 ----------\n"
                 "当前订阅者：";
    for (std::size_t i = 0; i < subs.size(); ++i) {
        std::cout << subs[i]->getName() << "（话题 " << subs[i]->getTopic() << "）";
        if (i + 1 < subs.size()) std::cout << "、";
    }
    std::cout << "\n（输入 q 可取消，返回菜单）\n";

    std::string name;
    for (;;) {
        if (!readLine("请输入订阅者节点名：", name)) return;
        if (isCancel(name)) { std::cout << "（已取消，返回菜单）\n"; return; }
        if (isBlank(name)) {
            std::cout << "[错误] 节点名不能为空或全为空白，请重新输入。\n";
            continue;                       // ← 回到这一步
        }

        Subscriber* found = nullptr;
        for (Subscriber* s : subs) {
            if (s->getName() == name) { found = s; break; }
        }
        if (found == nullptr) {
            std::cout << "[错误] 没有叫「" << name << "」的订阅者，请重新输入。\n";
            continue;                       // ← 回到这一步
        }

        found->printInbox();
        return;
    }
}

// ============================================================
//  选项 3：完整流程演示（原五阶段，原样保留）
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

void printWelcome(const std::vector<Subscriber*>& subs) {
    std::cout << "\n============================================================\n"
                 "        C++ 模拟 ROS 节点系统（会话式演示）\n"
                 "------------------------------------------------------------\n"
                 "  发布者：向指定话题发送一条消息\n"
                 "  订阅者：接收消息并保存在收件箱，可随时查看\n"
                 "\n  当前在线订阅者：\n";
    for (Subscriber* s : subs) {
        std::cout << "    · " << s->getName() << "  （监听话题 " << s->getTopic() << "）\n";
    }
    std::cout << "============================================================\n";
}

}  // namespace

int main() {
    initConsole();   // 必须在任何输出之前调用

    // 会话级订阅者：整个程序运行期间一直存在
    Subscriber screen  ("显示屏",   "temp1");
    Subscriber recorder("日志记录", "temp1");
    std::vector<Subscriber*> subs{ &screen, &recorder };

    screen.init();
    recorder.init();

    printWelcome(subs);

    for (;;) {
        std::cout << "\n---------------- 菜单 ----------------\n"
                     "  1  作为发布者发布消息\n"
                     "  2  作为订阅者查看消息\n"
                     "  3  完整流程演示（初始化→收发→异常→关闭）\n"
                     "  0  退出\n"
                     "--------------------------------------\n"
                     "请选择：";

        std::string line;
        if (!std::getline(std::cin, line)) break;   // 输入结束 -> 退出
        if (line.empty()) continue;

        switch (line[0]) {
        case '1':
            publishFlow();
            break;

        case '2':
            viewFlow(subs);
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

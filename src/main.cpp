// ============================================================
//  程序入口 —— 当前是"步骤 3/4 集成测试"
//
//  覆盖内容：
//    * RosCore 单例唯一性
//    * 节点初始化时自动注册（Publisher::init / Subscriber::init）
//    * 1 发多收：按话题名匹配转发
//    * 无人订阅的话题 -> WARN
//    * 重复注册 -> NodeException
//    * 统一关闭 -> 所有节点各打一次关闭日志
//    * FR-22 空/空白节点名、FR-25 空话题
//
//  TODO(最后): 这是测试用 main，作业最终版由你自己重写成"演示场景"。
// ============================================================
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <iostream>
#include <string>

#include "Exception.h"
#include "Message.h"
#include "Node.h"
#include "Publisher.h"
#include "RosCore.h"
#include "Subscriber.h"

namespace {

// Windows 控制台默认 GBK，源码是 UTF-8（编译带 /utf-8），不切代码页中文会乱码
void initConsole() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}

// FR-22：非法节点名必须在构造阶段就被拦下，且程序不崩溃
void checkIllegalName(const std::string& badName) {
    try {
        Publisher bad(badName, "/sensor/temp");
        std::cout << "[FAIL] 名字「" << badName << "」居然通过了校验\n";
    } catch (const NodeException& e) {
        std::cout << "[OK] 非法节点名被拦截：" << e.what() << "\n";
    }
}

}  // namespace

int main() {
    initConsole();

    std::cout << "=== 步骤 3/4 集成测试：RosCore 注册 / 转发 / 统一关闭 ===\n\n";

    // ---------- 1. 单例唯一性 ----------
    std::cout << "--- 1. 单例检查 ---\n";
    std::cout << "&instance() = " << &RosCore::instance() << "\n";
    std::cout << "&instance() = " << &RosCore::instance() << "   <- 两次必须相同\n";

    try {
        // ---------- 2. 造节点 + 注册 ----------
        std::cout << "\n--- 2. 初始化（init 里自动注册）---\n";
        Publisher  pubTemp("温度发布者", "/sensor/temp");
        Publisher  pubHum ("湿度发布者", "/sensor/hum");
        Subscriber subDisp("显示屏",   "/sensor/temp");
        Subscriber subLog ("日志记录", "/sensor/temp");
        Subscriber subHum ("湿度面板", "/sensor/hum");

        pubTemp.init();
        pubHum.init();
        subDisp.init();
        subLog.init();
        subHum.init();

        // ---------- 3. 转发：一个话题两个听众 ----------
        std::cout << "\n--- 3. 发布到 /sensor/temp（应 2 人收到）---\n";
        pubTemp.publish("25.3C");

        // ---------- 4. 转发：另一个话题 ----------
        std::cout << "\n--- 4. 发布到 /sensor/hum（应 1 人收到）---\n";
        pubHum.publish("60%");

        // ---------- 5. 无人订阅 ----------
        std::cout << "\n--- 5. 发布到无人订阅的话题（应 WARN）---\n";
        Message orphan("没有人听我说话");
        RosCore::instance().forwardMessage("/sensor/none", orphan);

        // ---------- 6. 重复注册 ----------
        std::cout << "\n--- 6. 重复注册（应抛 NodeException）---\n";
        try {
            Publisher dup("温度发布者", "/sensor/other");
            dup.init();
            std::cout << "[FAIL] 同名节点居然注册成功了\n";
        } catch (const NodeException& e) {
            std::cout << "[OK] 重复注册被拦截：" << e.what() << "\n";
        }

        // ---------- 7. 统一关闭 ----------
        std::cout << "\n--- 7. 统一关闭 ---\n";
        RosCore::instance().shutdownAll();

        // ---------- 8. FR-22 / FR-25 非法输入 ----------
        std::cout << "\n--- 8. 非法节点名与空话题 ---\n";
        checkIllegalName("");
        checkIllegalName("   ");
        try {
            Publisher badTopic("坏发布者", "  ");
        } catch (const TopicException& e) {
            std::cout << "[OK] 空话题被拦截：" << e.what() << "\n";
        }
    } catch (const RosException& e) {
        std::cout << "[捕获项目异常] " << e.what() << "\n";
    } catch (const std::exception& e) {
        std::cout << "[捕获标准异常] " << e.what() << "\n";
    }

    return 0;
}

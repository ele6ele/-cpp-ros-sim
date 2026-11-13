// ============================================================
//  程序入口 —— 当前是 M1 的自检程序
//
//  M1 做了什么：
//    * Node 抽象基类：名字封装 + 只读访问 + init()/shutdown() 纯虚接口
//    * 名字校验：空 / 全空白 -> NodeException（构造时就拦下来）
//    * 异常体系：RosException -> NodeException
//
//  TODO(M2): 新增 include/Message.h + src/Message.cpp（消息 + 时间戳），
//            然后把这个自检程序换成真正的发布/订阅演示。
//  详细步骤见 docs/03-开发路线图.md
// ============================================================
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <iostream>
#include <string>
#include <utility>

#include "Exception.h"
#include "Node.h"
#include "Message.h"

namespace {

// Windows 控制台默认 GBK，源码是 UTF-8（编译带 /utf-8），不切代码页中文会乱码
void initConsole() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}

// ------------------------------------------------------------------
// 临时演示节点：只为证明 Node 能被子类化、能校验名字。
// M4 会被真正的 Publisher / Subscriber 取代，届时这个类删掉。
// （Node 有纯虚函数，所以不能直接 Node n("x") —— 这一点编译器会帮你保证）
// ------------------------------------------------------------------
class DemoNode : public Node {
public:
    explicit DemoNode(std::string name) : Node(std::move(name)) {}

    void init() override {
        inited_ = true;
        std::cout << "[INFO] 节点「" << getName() << "」初始化完成\n";
    }

    void shutdown() override {
        inited_ = false;
        std::cout << "[INFO] 节点「" <<getName() << "」已关闭\n";
    }
};

// 演示 FR-22：非法名字必须在构造阶段就被拦下，且程序不崩溃
void checkIllegalName(const std::string& badName) {
    try {
        DemoNode node(badName);
        std::cout << "[FAIL] 名字「" << badName << "」居然通过了校验\n";
    } catch (const NodeException& e) {
        std::cout << "[OK] 非法名字被拦截：" << e.what() << "\n";
    }
}

}  // namespace

int main() {
    initConsole();

    std::cout << "=== M1 自检：Node 基类 + 名称校验 ===\n\n";

    // 1) 正常名字：能创建，能走完 init -> shutdown 生命周期
    DemoNode node("pub_temp");
    node.init();
    node.shutdown();

    // 2) 非法名字：空串、全空白
    std::cout << "\n--- 非法名字测试（FR-22）---\n";
    checkIllegalName("");
    checkIllegalName("   ");

    std::cout << "\nM1 完成。下一步 M2：Message 消息封装与时间戳。\n";

    Message m("25.3C");
    std::cout << m.getContent() << " | " << m.getTimestamp() << "\n";
    std::cout << "长度=" << m.getTimestamp().size() << "\n";





    return 0;
}

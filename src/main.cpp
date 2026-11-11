// ============================================================
//  C++ 模拟 ROS 节点系统 —— 程序入口
//  目前只是一个能编译、能运行的起点，后续按 docs/03-开发路线图.md
//  一步步把各个模块接进来（每完成一步就 git commit 一次）。
// ============================================================
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <iostream>

namespace {

// Windows 控制台默认使用 GBK 代码页，而源码和程序内部统一按 UTF-8
// （编译参数里带了 /utf-8）。不切换代码页的话，中文日志会变成乱码。
void initConsole() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}

}  // namespace

int main() {
    initConsole();

    std::cout << "=== C++ 模拟 ROS 节点系统（作业骨架）===\n";
    std::cout << "当前是可运行的起点，请按 docs/03-开发路线图.md 逐步补全：\n";
    std::cout << "  M1  Node 基类：名称校验 + init/shutdown 接口\n";
    std::cout << "  M2  Message：消息内容 + YYYY-MM-DD HH:MM:SS 时间戳\n";
    std::cout << "  M3  RosCore 单例：注册表 + 节点管理\n";
    std::cout << "  M4  Publisher / Subscriber + 话题消息转发\n";
    std::cout << "  M5  异常体系（NodeException 等）与日志分级\n";
    std::cout << "  M6  加分项：自定义消息 / 服务端客户端 / 动态增删 / 日志落盘\n";
    std::cout << "\n详细需求见 docs/01-需求文档.md，架构见 docs/02-架构设计.md。\n";
    return 0;
}

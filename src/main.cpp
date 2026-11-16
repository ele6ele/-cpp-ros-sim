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

    // ---------- 阶段 5：统一关闭 ----------
    std::cout << "\n=== 阶段 5：统一关闭 ===\n";
    RosCore::instance().shutdownAll();

    return 0;
}

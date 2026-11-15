#pragma once

#include <string>
#include <vector>

// 前向声明：这里只用到它们的【指针/引用】，不需要完整定义。
// 这样 Publisher.h <-> RosCore.h 就不会互相 include 造成循环依赖。
class Publisher;
class Subscriber;
class Message;
class Node;

class RosCore {
private:
    std::vector<Publisher*>  publishers_;    // 发布名册：谁在哪个话题喊话
    std::vector<Subscriber*> subscribers_;   // 听众名册：谁在听哪个话题
    std::vector<Node*>       nodes_;         // 总名册：统一关闭时用

    RosCore() = default;
    ~RosCore() = default;
    RosCore(const RosCore&) = delete;             // 禁拷贝：防止出现第二份名单
    RosCore& operator=(const RosCore&) = delete; // 禁赋值

public:
    static RosCore& instance();   // static：不需要对象就能调用

    void registerPublisher(Publisher& pub);
    void registerSubscriber(Subscriber& sub);

    void forwardMessage(const std::string& topic, const Message& msg);

    void shutdownAll();
};

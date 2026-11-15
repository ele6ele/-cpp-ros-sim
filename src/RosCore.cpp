#include <iostream>
#include "Exception.h"
#include "Message.h"
#include "Node.h"
#include "Publisher.h"
#include "Subscriber.h"
#include "RosCore.h"

// 单例：函数内静态变量，第一次调用时创建，之后一直返回同一个，程序结束时自动销毁
RosCore& RosCore::instance() {
    static RosCore theOne;
    return theOne;
}

void RosCore::registerPublisher(Publisher& pub) {
    // 1) 判重：两张名册都要查（节点名要求全局唯一）
    for (Publisher* named : publishers_) {
        if (named->getName() == pub.getName()) {
            throw NodeException("节点名已被注册：" + pub.getName());
        }
    }
    for (Subscriber* named : subscribers_) {
        if (named->getName() == pub.getName()) {
            throw NodeException("节点名已被注册：" + pub.getName());
        }
    }
    // 2) 登记进对应名册 + 总名册
    publishers_.push_back(&pub);
    nodes_.push_back(&pub);
}

void RosCore::registerSubscriber(Subscriber& sub) {
    for (Publisher* named : publishers_) {
        if (named->getName() == sub.getName()) {
            throw NodeException("节点名已被注册：" + sub.getName());
        }
    }
    for (Subscriber* named : subscribers_) {
        if (named->getName() == sub.getName()) {
            throw NodeException("节点名已被注册：" + sub.getName());
        }
    }
    subscribers_.push_back(&sub);
    nodes_.push_back(&sub);
}

void RosCore::forwardMessage(const std::string& topic, const Message& msg) {
    int count = 0;

    for(Subscriber* sub1 : subscribers_){
        if (topic == sub1->getTopic()){
            sub1->onMessage(msg);
            ++count;
        }
    }
    if(count == 0){
        std::cout << "[WARN] 话题「" << topic << "」当前没有订阅者" << std::endl;
    }

}

void RosCore::shutdownAll() {
    for (Node* node : nodes_) {
        node->shutdown();   // 虚函数：自动调到 Publisher / Subscriber 各自的实现
    }
    publishers_.clear();
    subscribers_.clear();
    nodes_.clear();             //清空所有名单
    std::cout << "[系统] 所有节点已关闭" << std::endl;
}

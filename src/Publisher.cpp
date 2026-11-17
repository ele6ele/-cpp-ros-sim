#include <iostream>
#include <string>
#include "Exception.h"
#include "Logger.h"
#include "Message.h"
#include "Node.h"
#include "Publisher.h"
#include "RosCore.h"

// 传入节点名和话题名；话题名表明这是哪个频道，让 RosCore 发给正确的 Subscriber
Publisher::Publisher(const std::string& name, const std::string& topic)
    : Node(name), topic_(topic) {
    if (topic.find_first_not_of(" \t\n\r") == std::string::npos) {
        throw TopicException("话题名不能为空或全为空白");
    }
}

const std::string& Publisher::getTopic() const {
    return topic_;
}

void Publisher::init(){
    RosCore::instance().registerPublisher(*this);   // 先注册：失败就直接抛，不会留下"已上线"的假日志
    inited_ = true;
    Logger::instance().log(LogLevel::Info, "Publisher",
                           "节点「" + getName() + "」已在话题 " + topic_ + " 上线");
}

void Publisher::publish(const std::string& data) {
    Message msg{data};   // 传参并调用 Message 的构造函数，创建消息对象
    Logger::instance().log(LogLevel::Info, "Publisher",
                           getName() + " -> " + topic_ + " : " + msg.getContent());
    RosCore::instance().forwardMessage(topic_, msg);   //这里就调用了转发函数
}

void Publisher::shutdown() {
    Logger::instance().log(LogLevel::Info, "Publisher", "节点「" + getName() + "」下线");
    inited_ = false;
}

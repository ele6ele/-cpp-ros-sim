#include <iostream>
#include <string>

#include "Exception.h"
#include "Message.h"
#include "Node.h"
#include "Publisher.h"

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
    std::cout << "[启动] 发布者「" << getName() << "」已在话题 " << topic_ << " 上线" << std::endl;
    inited_ = true;
    // TODO(步骤4): RosCore::instance().registerPublisher(*this);
}

void Publisher::publish(const std::string& data) {
    Message msg{data};   // 传参并调用 Message 的构造函数，创建消息对象
    std::cout << "[发布] " << getName() << " -> " << topic_ << " : " << msg.getContent() << std::endl;
    // TODO(步骤4): RosCore::instance().publish(topic_, msg);
}

void Publisher::shutdown() {
    std::cout << "[关闭] 发布者「" << getName() << "」下线" << std::endl;
    inited_ = false;
}

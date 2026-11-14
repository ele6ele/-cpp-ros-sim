#include <iostream>
#include <string>
#include "Exception.h"
#include "Message.h"
#include "Node.h"
#include "Subscriber.h"

Subscriber::Subscriber(const std::string& name, const std::string& topic)
    : Node(name), topic_(topic) {
    if (topic.find_first_not_of(" \t\n\r") == std::string::npos) {
        throw TopicException("话题名不能为空或全为空白");
    }
}

const std::string& Subscriber::getTopic() const {
    return topic_;
}

void Subscriber::init(){
    std::cout << "[启动] 订阅者「" << getName() << "」开始监听 " << topic_ <<std::endl;
    inited_ = true;
    // TODO(步骤4): RosCore::instance().registerSubscriber(*this);
}

void Subscriber::onMessage(const Message& msg){
    std::cout << "[接收] 订阅者「" << getName() << "」收到 " << msg.getContent()
              << "（" << msg.getTimestamp() << "）" << std::endl;
}

void Subscriber::shutdown() {
    std::cout << "[关闭] 订阅者「" << getName() << "」停止监听" << std::endl;
    inited_ = false;
}

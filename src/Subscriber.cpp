#include <iostream>
#include <string>
#include "Exception.h"
#include "Message.h"
#include "Node.h"
#include "Subscriber.h"
#include "RosCore.h"

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
    RosCore::instance().registerSubscriber(*this);   // 先注册，成功后再宣布开始监听
    inited_ = true;
    std::cout << "[启动] 订阅者「" << getName() << "」开始监听 " << topic_ <<std::endl;
}

void Subscriber::onMessage(const Message& msg){
    inbox_.push_back(msg);   // 先存进收件箱，再打印（这样"查看消息"才有内容可看）

    std::cout << "[接收] 订阅者「" << getName() << "」收到 " << msg.getContent()//内容在 Message 里是 private 的,把整个对象传进来还得getter来调内容
              << "（" << msg.getTimestamp() << "）" << std::endl;
}

const std::vector<Message>& Subscriber::getInbox() const {
    return inbox_;
}

void Subscriber::printInbox() const {
    std::cout << "\n[" << getName() << "] 收件箱（话题 " << topic_
              << "，共 " << inbox_.size() << " 条）：\n";
    if (inbox_.empty()) {
        std::cout << "  （还没有收到任何消息）\n";
        return;
    }
    for (std::size_t i = 0; i < inbox_.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << inbox_[i].getContent()
                  << "（" << inbox_[i].getTimestamp() << "）\n";
    }
}

void Subscriber::shutdown() {
    std::cout << "[关闭] 订阅者「" << getName() << "」停止监听" << std::endl;
    inited_ = false;
}

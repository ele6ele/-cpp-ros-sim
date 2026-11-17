#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "Exception.h"
#include "Logger.h"
#include "Message.h"
#include "Node.h"
#include "Subscriber.h"
#include "TemperatureMessage.h"
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
    Logger::instance().log(LogLevel::Info, "Subscriber",
                           "节点「" + getName() + "」开始监听话题 " + topic_);
}

void Subscriber::onMessage(const Message& msg){
    // 加分项 B-01：按消息类型决定展示内容
    std::string shown = msg.getContent();
    const std::string kind = msg.type();

    if (kind == "temperature") {
        const TemperatureMessage* reading = dynamic_cast<const TemperatureMessage*>(&msg);
        if (reading != nullptr) {
            std::ostringstream oss;
            oss << "温度 " << std::fixed << std::setprecision(1) << reading->temperature()
                << "℃ / 湿度 " << std::fixed << std::setprecision(1) << reading->humidity() << "%";
            shown = oss.str();
        }
    }

    //内容在 Message 里是 private 的,把整个对象传进来还得getter来调内容
    Logger::instance().log(LogLevel::Info, "Subscriber",
                           "节点「" + getName() + "」收到 [" + kind + "] " + shown
                           + "（" + msg.getTimestamp() + "）");
}

void Subscriber::shutdown() {
    Logger::instance().log(LogLevel::Info, "Subscriber", "节点「" + getName() + "」停止监听");
    inited_ = false;
}

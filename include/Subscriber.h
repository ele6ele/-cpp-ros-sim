#pragma once
#include <string>
#include <vector>

#include "Message.h"   // 收件箱需要完整的 Message 类型
#include "Node.h"

class Subscriber:public Node{
    private:
         std::string topic_;
         std::vector<Message> inbox_;   // 收件箱：收到的每条消息按顺序存下来

    public:
        Subscriber(const std::string& name, const std::string& topic);

        const std::string& getTopic() const;

        void init() override;

        void onMessage(const Message& msg);

        const std::vector<Message>& getInbox() const;   // 供"查看消息"使用
        void printInbox() const;                        // 打印收件箱

        void shutdown() override;

};

#pragma once
#include <string>
#include "Node.h"

class Message;

class Subscriber:public Node{
    private:
         std::string topic_;

    public:
        Subscriber(const std::string& name, const std::string& topic);

        const std::string& getTopic() const;

        void init() override;

        void onMessage(const Message& msg);

        void shutdown() override;

};

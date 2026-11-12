#pragma once
#include <string>
#include "Exception.h"

class Message{
    private:
        static std::string now();

        std::string content_;//先声明成员变量不然没法接收content传的值
        std::string timestamp_;//必须private保证封装性不然谁都可以改

    public:
        explicit Message(const std::string& content);

        const std::string& getContent() const;
        const std::string& getTimestamp() const;

};
//头文件里函数声明，在.cpp里实现，方便其他模块调用

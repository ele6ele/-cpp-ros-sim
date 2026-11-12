#pragma once

#include <stdexcept>
#include <string>

class RosException : public std::runtime_error {            //继承，把runtime_error变成自己的
public:
    explicit RosException(const std::string& message) : std::runtime_error(message) {}
};

class NodeException : public RosException {         //其实再怎么继承最终都是交给runtime_error处理，但是能清晰的知道是哪里错了
public:
    explicit NodeException(const std::string& message) : RosException(message) {}
};

class MessageException : public RosException {
public:
    explicit MessageException(const std::string& message) : RosException(message) {}
};



#include <string>
#include "Exception.h"
#include "Node.h"

Node::Node(const std::string& name){  //形参只读，保证init函数内部不会改变这个变量
    if(name.find_first_not_of(" \t\n\r")==std::string::npos)throw NodeException("节点名不可为空");
    name_ = name;

}

const std::string& Node::getName() const{
    return name_;
}



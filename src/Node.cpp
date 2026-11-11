// ============================================================
//  Node 基类的实现（.h 里只写声明，真正的代码写在这里）
// ============================================================
#include "Node.h"

#include "Exception.h"

#include <utility>

Node::Node(std::string name) : name_(std::move(name)) {
    // 关键设计：在构造函数里就校验，保证"对象一旦创建成功，名字一定合法"，
    // 不给外部留下"半成品对象"。校验失败直接抛异常，构造函数不会执行完。
    validateName(name_);
}

void Node::validateName(const std::string& name) {
    // 说明：find_first_not_of 返回第一个"不属于这些字符"的位置。
    //       返回 npos 表示整串只由空白字符组成（或者本来就是空串）。
    if (name.empty() || name.find_first_not_of(" \t\n\r\f\v") == std::string::npos) {
        throw NodeException("节点名不能为空或全为空白字符");
    }
}

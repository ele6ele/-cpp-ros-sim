// ============================================================
//  异常体系（FR-22 ~ FR-25, B-05）
//
//  设计要点：
//    * 统一基类 RosException 派生自 std::runtime_error
//      —— 这样 catch (const std::exception&) 也能兜住，符合标准库习惯
//    * 每个子类对应一类错误，main 里可以分层捕获，打印清晰的语义信息
//
//  TODO(M5): 补齐 TopicException（话题不存在 / 空话题）
//            MessageException（空消息内容）
//            目前只放 M1 需要的 NodeException。
// ============================================================
#pragma once

#include <stdexcept>
#include <string>

// 项目内所有异常的统一基类
class RosException : public std::runtime_error {
public:
    explicit RosException(const std::string& message) : std::runtime_error(message) {}
};

// 节点相关错误：空节点名、重复注册、节点未初始化就使用……
class NodeException : public RosException {
public:
    explicit NodeException(const std::string& message) : RosException(message) {}
};

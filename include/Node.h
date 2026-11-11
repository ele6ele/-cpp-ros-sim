// ============================================================
//  Node —— 所有节点的抽象父类（FR-01 ~ FR-03）
//
//  职责：
//    1. 封装节点名，只提供只读访问（name() 返回 const 引用）
//    2. 定义 init() / shutdown() 两个纯虚接口，子类必须实现
//    3. 把"名字合法性"这种共性约束收敛到基类，子类不用重复写
//
//  注意：
//    * 基类析构必须是 virtual，否则用基类指针删除派生对象是未定义行为
//    * 不要在头文件里写长函数体（除了 inline 的小函数），否则容易重复定义
// ============================================================
#pragma once

#include <string>

class Node {
public:
    // explicit：禁止 std::string 隐式转换成 Node，避免不小心写错
    // 名字为空或全为空白字符时抛 NodeException
    explicit Node(std::string name);

    virtual ~Node() = default;

    // 节点名只读：返回 const 引用，零拷贝，且外部改不了
    const std::string& name() const noexcept { return name_; }

    // ---- 子类必须实现的两个生命周期接口 ----
    // 初始化：输出初始化日志，并向 RosCore 注册自身
    virtual void init() = 0;
    // 关闭：输出关闭日志
    virtual void shutdown() = 0;

protected:
    // 校验节点名（空 / 全空白 -> NodeException），构造函数里调用
    static void validateName(const std::string& name);

    std::string name_;
    bool inited_{false};   // 生命周期状态位，供子类判断"是否已初始化"
};

# 进度与协作约定

> 换新对话时，先让我读这个文件，就能立刻接上进度。

## 一、工程事实

| 项 | 值 |
| --- | --- |
| 仓库 | `C:\Users\ele\Documents\deepseek-harness\default-workspace\cpp-ros-sim` |
| 编译器 | MSVC 14.51（VS 18 Community），C++17，`/W4 /permissive-` |
| 构建 | `Ctrl+Shift+B` → `build.bat` → `build\ros_sim.exe` |
| 运行 | `run.bat` 或 `F5`（cppvsdbg） |
| 目录约定 | 声明放 `include/*.h`，实现放 `src/*.cpp`（`src` 下所有 `.cpp` 自动参与编译） |

## 二、已完成

| 模块 | 状态 | 提交 | 文件 |
| --- | --- | --- | --- |
| 工程骨架（构建/配置/文档） | ✅ | `56863d6` | `build.bat`、`.vscode/`、`docs/` |
| 异常体系 `RosException`/`NodeException` | ✅ | `6691693` | `include/Exception.h` |
| `Node` 抽象基类 + 名称校验 | ✅ | `9842063` | `include/Node.h`、`src/Node.cpp` |
| `Message`（内容 + 时间戳 + 空校验） | ✅ | `6dee637` | `include/Message.h`、`src/Message.cpp` |
| `MessageException` | ✅ | `6dee637` | `include/Exception.h` |

## 三、下一步（顺序已调整）

原路线图是 M3 `RosCore` → M4 `Publisher/Subscriber`，但 `RosCore::registerPublisher` 需要 `Publisher` 类型，
所以改为"先零件、后接线"，每步都能编译运行：

| 步骤 | 内容 | 验收 |
| --- | --- | --- |
| **0**（已完成 ✅）`Node` 重写练习 | 不看现有实现，按自己的理解重写 `include/Node.h` + `src/Node.cpp`；写完再和原实现对比差异 | 编译通过；空名字 / 全空白名字抛 `NodeException`；重新提交为 `refactor(node): 按自己的理解重写 Node 基类` |
| **1** `Publisher` | 继承 `Node`，加 `topic_`；构造校验非空；`init`/`shutdown` 打日志；`publish` 先只打印（暂不转发） | 能造出发布者、走完生命周期 |
| **2** `Subscriber` | 同上 + `onMessage(const Message&)` 打印内容与时间戳 | 能造出订阅者 |
| **3** `RosCore` | 单例（Meyers）、两张表、`registerPublisher/registerSubscriber`、`publish` 转发、`shutdownAll` | 单例地址唯一；注册后表里有节点 |
| **4** 接线 | `Publisher::init` 调注册、`Publisher::publish` 调转发、`Subscriber::init` 调注册 | **1 发 3 收跑通（截图）** |
| 5 | 补齐剩余异常（`TopicException`）与 4 类非法输入 | 故意写错不崩、提示可读 |
| 6 | `main` 演示场景 + 测试报告 + 用户手册 | 一键运行完整流程 |
| 7 | 加分项（日志分级落盘 / 自定义消息） | 选 1~2 个 |

## 四、已确定的设计决策

1. **时间戳不用 `static` 成员**：每条消息各自一份，构造时生成（`timestamp_ = now()`）。
2. **异常三层**：`std::runtime_error → RosException → Node/Topic/MessageException`，具体异常先捕获。
3. **暂不引入 `unordered_map`**：`RosCore` 先用 `vector` + `for` + `if (sub->topic() == topic)` 跑通，之后再重构成按话题查找（多一次 `refactor` 提交）。
4. **关闭自动补全**：`.vscode/settings.json` 里已配好，需要提示时按 `Ctrl+Space`；错误波浪线保留。
5. **`.vscode/c_cpp_properties.json` 已被 VS Code 重写并提交**，不要再点 `C/C++: 编辑配置(UI)`。

## 五、协作方式（模式 A）

- 我只讲"做什么、为什么、验收标准 + API 资料"，**不提前给实现代码**；
- 用户自己写，写完贴代码或报错原文，我只做评审；
- 若我贴了完整实现，用户回一句"模式 A"即可。

## 六、讲解风格

- 知识类内容用**笔记式规范**：速记 → 定义 → 术语 → 机制 → 语法规则 → 示例 → 易错点 → 自查清单 → 模板；
- **每个小节后附一段"通俗讲解"**（比喻 / 白话），便于理解后自己补笔记。

## 七、提交规范

- 一条提交 = 一件完整的事；提交前必须 `Ctrl+Shift+B` 通过（0 错误 0 警告）；
- 格式：`feat(模块): 说明` / `fix(...)` / `docs: ...` / `refactor(...)` / `chore(...)`；
- 在 VS Code 里：`Ctrl+Shift+G` → 文件旁 `+` 暂存 → 写消息 → `Ctrl+Enter`。

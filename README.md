# C++ 模拟 ROS 节点系统（大作业）

用纯 C++（不依赖真实 ROS 的 API）模拟 ROS 最核心的**节点 - 话题 - 消息**松耦合异步通信机制。
本项目已完成五个类、完整演示流程与异常处理，共 **30+ 次迭代提交**，体现了增量开发过程。

## 1. 项目状态

| 模块 | 文件 | 状态 |
| --- | --- | --- |
| 异常体系（`RosException` / `NodeException` / `TopicException` / `MessageException`） | `include/Exception.h` | ✅ |
| `Node` 抽象基类（名字封装、`init`/`shutdown` 纯虚、虚析构） | `include/Node.h`、`src/Node.cpp` | ✅ |
| `Message`（内容 + `YYYY-MM-DD HH:MM:SS` 时间戳、只读 getter、空内容校验） | `include/Message.h`、`src/Message.cpp` | ✅ |
| `Publisher`（话题属性、注册、发布） | `include/Publisher.h`、`src/Publisher.cpp` | ✅ |
| `Subscriber`（话题属性、注册、接收） | `include/Subscriber.h`、`src/Subscriber.cpp` | ✅ |
| `RosCore`（单例、两本名册、判重、按话题转发、统一关闭） | `include/RosCore.h`、`src/RosCore.cpp` | ✅ |
| 演示程序（初始化 → 连续发布 → 无人话题 → 异常演示 → 统一关闭） | `src/main.cpp` | ✅ |

## 2. 运行环境

| 组件 | 版本 / 位置 |
| --- | --- |
| 编译器 | MSVC 14.51（Visual Studio 18 Community，"使用 C++ 的桌面开发"） |
| Windows SDK | 10.0.26100.0 |
| 编辑器 | VS Code + `ms-vscode.cpptools` |
| 标准 | C++17（`/std:c++17 /W4 /permissive-`） |
| 第三方库 | 无 |

编译器未加入系统 PATH：`build.bat` 通过 `vswhere` 自动定位 VS 并加载 `vcvars64`，因此直接敲 `cl` 会提示找不到，属正常现象。

## 3. 快速开始

```powershell
.\build.bat      # 编译，产物在 build\ros_sim.exe
.\run.bat        # 编译并运行
.\clean.bat      # 清理产物
```

VS Code 中：

| 操作 | 快捷键 |
| --- | --- |
| 编译 | `Ctrl + Shift + B` |
| 调试运行 | `F5` |

程序**无需任何输入**，一次运行跑完全部五个阶段。

## 4. 运行效果（节选）

```
=== 阶段 1：初始化 ===
[启动] 发布者「温度发布者」已在话题 temp1 上线
[启动] 订阅者「显示屏」开始监听 temp1
...

=== 阶段 2：连续发布 ===
[发布] 温度发布者 -> temp1 : 16℃
[接收] 订阅者「显示屏」收到 16℃（2026-11-16 23:45:37）
[接收] 订阅者「日志记录」收到 16℃（2026-11-16 23:45:37）

=== 阶段 3：无人订阅的话题 ===
[WARN] 话题「示例标题」当前没有订阅者

=== 阶段 4：异常演示 ===
[OK]重名消息被拦截：节点名已被注册：温度发布者
[OK]空白标题被拦截：话题名不能为空或全为空白

=== 阶段 5：统一关闭 ===
[关闭] 发布者「温度发布者」下线
[系统] 所有节点已关闭
```

完整输出见 [`docs/04-用户手册.md`](docs/04-用户手册.md)。

## 5. 目录结构

```
cpp-ros-sim/
├── .vscode/               # 编译任务、调试配置、IntelliSense、编码设置
├── build.bat / run.bat / clean.bat
├── include/               # 头文件
│   ├── Node.h  Message.h  Publisher.h  Subscriber.h  RosCore.h  Exception.h
├── src/                   # 实现 + main.cpp（演示入口）
├── docs/
│   ├── 01-需求文档.md      # 需求条目化、验收标准、加分项清单
│   ├── 02-架构设计.md      # 分层、类关系、接口草案、设计决策
│   ├── 03-开发路线图.md    # 里程碑与提交计划
│   ├── 04-用户手册.md      # ★ 怎么编译、怎么运行、输出怎么读
│   ├── 05-测试报告.md      # ★ T-01~T-11 用例结果与已知问题
│   └── 06-每个模块怎么写.md # 开发思路笔记（思考清单）
├── tests/                 # 预留测试目录
└── PROGRESS.md            # 进度、设计决策、协作约定
```

## 6. Git 工作流

作业要求「增量开发痕迹，禁止一次性提交」，本项目按功能拆分提交：

```powershell
git add include/Node.h src/Node.cpp
git commit -m "feat(node): 实现 Node 基类与名称校验"
```

提交信息格式：`类型(范围): 说明`，类型取 `feat` / `fix` / `docs` / `test` / `refactor` / `chore`。

## 7. 编码要点（Windows 特有）

- **中文乱码**：源文件 UTF-8 + 编译 `/utf-8` + 运行时 `SetConsoleOutputCP(CP_UTF8)`，三者缺一不可。
- **时间戳**：`localtime_s(&tmValue, &t)` + `strftime(..., "%Y-%m-%d %H:%M:%S", ...)`，不用 `std::localtime`（MSVC 报 C4996）。
- **`min/max`**：`windows.h` 的宏会破坏 `std::min/std::max`，已定义 `NOMINMAX`。
- **头文件依赖**：`RosCore.h` 只用前向声明，完整定义放在 `.cpp`，避免与 `Publisher.h` 循环包含。
- **警告清零**：`/W4 /permissive-` 下保持 0 警告。

## 8. 文档索引

- [需求文档](docs/01-需求文档.md) / [架构设计](docs/02-架构设计.md) / [开发路线图](docs/03-开发路线图.md)
- [用户手册](docs/04-用户手册.md) / [测试报告](docs/05-测试报告.md) / [每模块写作笔记](docs/06-每个模块怎么写.md)

## 9. 加分项完成情况

| 编号 | 加分项 | 状态 | 说明 |
| --- | --- | --- | --- |
| B-01 | 自定义消息类型（温度/湿度/时间戳） | ❌ 未做 | `Message` 已预留虚析构，可扩展派生类 |
| B-02 | 服务/客户端同步通信 | ❌ 未做 | — |
| B-03 | 运行期动态增删节点 | ❌ 未做 | 需要注销接口（`RosCore` 目前只有注册） |
| B-04 | 消息收发日志持久化到文件 | ❌ 未做 | — |
| B-05 | 自定义异常类 + 全局捕获 + 异常日志入文件 | ⚠️ 部分 | 四个异常类已完成、`catch` 分层；**缺**全局捕获器与文件输出 |
| B-06 | 日志分级（INFO/WARN/ERROR）分类输出 | ❌ 未做 | 现在只有 `[启动]/[发布]/[接收]/[WARN]` 等文本前缀，无级别枚举与统一 `Logger` |

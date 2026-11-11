# C++ 模拟 ROS 节点系统（大作业）

用纯 C++（不依赖真实 ROS 的 API）模拟 ROS 最核心的**节点 - 话题 - 消息**松耦合异步通信机制。
重点考察 C++ 的面向对象设计、智能指针、STL 容器、异常处理与工程规范。

> 本仓库是**可编译、可运行、可断点调试的骨架**：环境已经调通，`src/main.cpp` 里是一个占位程序。
> 具体类怎么实现、怎么拆 commit，按 [`docs/03-开发路线图.md`](docs/03-开发路线图.md) 一步步来。

---

## 1. 环境

本机已就绪，不需要再装任何东西：

| 组件 | 版本 / 位置 |
| --- | --- |
| 编译器 | MSVC 14.51（Visual Studio 18 Community） |
| Windows SDK | 10.0.26100.0 |
| 编辑器 | VS Code + `ms-vscode.cpptools`（已安装） |
| Git | 2.56 |
| 标准 | C++17（`/std:c++17`） |

编译器没有加进系统 PATH，`build.bat` 会用 `vswhere` 自动找到 VS 并加载 `vcvars64`，所以**你在命令行里直接敲 `cl` 会提示找不到**，这是正常的，用下面的脚本入口就行。

## 2. 快速开始

```powershell
code C:\Users\ele\Documents\deepseek-harness\default-workspace\cpp-ros-sim
```

| 操作 | 快捷键 / 命令 |
| --- | --- |
| 编译 | `Ctrl + Shift + B`（默认 build 任务） |
| 编译并运行 | `Ctrl + Shift + P` → `Tasks: Run Task` → `run` |
| 断点调试 | `F5`（选“调试运行 (MSVC)”，会先自动编译） |
| 清理产物 | `Tasks: Run Task` → `clean` |
| 命令行 | `.\build.bat` / `.\run.bat` / `.\clean.bat` |

产物统一输出到 `build\ros_sim.exe`（已在 `.gitignore` 里忽略，不要提交）。

## 3. 目录结构

```
cpp-ros-sim/
├── .vscode/               # 编译任务、调试配置、IntelliSense、编码设置
├── build.bat              # 一键编译（自动加载 MSVC 环境）
├── run.bat                # 编译 + 运行（带 chcp 65001）
├── clean.bat              # 清理 build\
├── docs/
│   ├── 01-需求文档.md      # 需求条目 + 验收标准
│   ├── 02-架构设计.md      # 分层、类关系、接口草案、关键设计决策
│   └── 03-开发路线图.md    # 按里程碑拆分的实现顺序与提交计划
├── include/               # 头文件（.h）
├── src/                   # 实现（.cpp），main.cpp 是入口
└── tests/                 # 自测场景 / 测试用例
```

## 4. Git 工作流

作业明确要求「增量开发痕迹，禁止一次性提交所有代码」，所以一个功能一个 commit：

```powershell
git status                  # 先看清楚改了什么
git add include/Node.h src/Node.cpp
git commit -m "feat(node): 实现 Node 基类与名称校验"
```

提交信息用 `类型(范围): 说明` 的形式，类型取 `feat` / `fix` / `docs` / `test` / `refactor` / `chore`。
每个里程碑的参考提交信息写在 [`docs/03-开发路线图.md`](docs/03-开发路线图.md) 里。

## 5. 编码注意事项（Windows 特有坑）

- **中文乱码**：源文件是 UTF-8，编译带 `/utf-8`，程序启动时调用 `SetConsoleOutputCP(CP_UTF8)`（`main.cpp` 里已有 `initConsole()`），三者缺一就会乱码。
- **时间戳格式化**：不要用 `std::localtime`（MSVC 会报 C4996），用 `localtime_s(&tmBuf, &t)`；格式化用 `std::put_time(&tmBuf, "%Y-%m-%d %H:%M:%S")`。
- **`std::min / std::max`**：会被 `windows.h` 的宏破坏，已在编译参数里定义 `NOMINMAX`。
- **编译警告当参考**：`/W4 /permissive-` 已经打开，警告尽量清零，这是「严格 C++ 编码习惯」的一部分。
- 头文件用 `#pragma once` 或 include guard，`#include` 顺序：对应头文件 → 标准库 → 项目内。

## 6. 文档索引

- [需求文档](docs/01-需求文档.md)：需求条目化、验收标准、加分项清单
- [架构设计](docs/02-架构设计.md)：分层结构、类关系、接口草案、设计决策与取舍
- [开发路线图](docs/03-开发路线图.md)：M1–M7 的实现顺序、每步验收方式与 commit 计划

## 7. 进度

- [x] M1 Node 基类（已实现，见 `include/Node.h`）
- [ ] M2 Message 消息封装
- [ ] M3 RosCore 单例与注册表
- [ ] M4 Publisher / Subscriber 与消息转发
- [ ] M5 异常体系与日志分级
- [ ] M6 用户手册 + 测试用例
- [ ] M7 加分项

# SecRandom C++ FLTK

一个基于 **C++14 + FLTK** 实现的课堂公平随机点名与抽奖系统。

本仓库是 **中国科学技术大学《面向对象程序设计》2026 Spring 课程大作业** 项目，用于完成课程作业、课堂展示和学习交流。

项目灵感来自这门课本身的点名场景：课堂点名虽然是随机的，但实际体验中仍然容易让人觉得“好像不够公平”，例如有人短时间内连续被抽中，也有人很久没有被抽到。因此，我希望实现一个不只是“能随机选人”，而是能记录历史、考虑长期均衡的公平随机点名与抽奖工具。

本项目参考了 Python 版 [SECTL/SecRandom](https://github.com/SECTL/SecRandom) 的功能设计和公平抽取算法思想，并将其中适合作为 C++ 课程大作业的核心功能重新实现为 C++/FLTK 桌面程序。原项目功能完整、技术栈为 Python；本项目重点在于理解其公平抽取思路后，进行功能裁剪、模块化重构和 C++ 图形化实现。

## Quick Start

### Requirements

- C++14
- CMake 3.15+
- FLTK

macOS 上可使用 Homebrew 安装 FLTK：

```bash
brew install fltk
```

Linux 上可使用系统包管理器安装，例如：

```bash
sudo apt install libfltk1.3-dev
```

### Build

在项目根目录执行：

```bash
cmake -S . -B build
cmake --build build
```

构建成功后生成：

```text
build/SecRandomApp
```

### Run

```bash
./build/SecRandomApp
```

程序启动时会从 `data/students.csv` 读取学生名单。

### Prepare Student Data

学生名单文件：`data/students.csv`

```csv
学号,姓名,性别,分组
20260001,学生_1,女,第2组
20260002,学生_2,男,第1组
```

也可以在程序的“名单管理”窗口中点击“导入”，选择 CSV 文件导入。导入成功后会覆盖保存到 `data/students.csv`。

### Basic Usage

1. 启动程序后，在主窗口点击“随机点名”抽取 1 名学生。
2. 点名后可点击“出勤 / 请假 / 缺勤”记录状态。
3. 在设置窗口中调整抽奖人数，然后点击“随机抽奖”进行多人抽取。
4. 在主窗口选择分组后点击“分组抽取”，只从该分组中抽取。
5. 在“名单管理”窗口维护学生名单。
6. 在“历史记录”窗口查看或清空点名、抽奖历史。
7. 在“设置”窗口调整公平算法、重复抽取、差距阈值、权重等级、主题和备份恢复。

## Features

- 随机点名
- 随机抽奖
- 分组抽取
- 抽奖人数控制
- 公平抽取算法
- 是否允许重复抽取
- 简单权重等级设置
- 点名出勤记录
- 抽奖历史记录
- 学生名单增删改查
- CSV 名单导入与导出
- 数据备份与恢复
- 浅色 / 深色主题切换
- 基于 FLTK 的图形化界面

## Screens and Windows

程序包含四个主要窗口：

- `MainWindow`：主窗口，用于点名、抽奖、分组抽取和出勤记录。
- `StudentManageWindow`：名单管理窗口，用于新增、修改、删除、导入、保存和导出学生名单。
- `HistoryWindow`：历史记录窗口，用于查看和清空点名历史、抽奖历史。
- `SettingsWindow`：设置窗口，用于调整抽奖人数、公平算法、重复抽取、差距阈值、权重等级、主题、备份恢复等选项。

界面风格采用简约清新的课堂工具风格，具体规范见 [界面风格规范.md](界面风格规范.md)。

## Algorithm

本项目的公平抽取算法不是完全随机，也不是固定轮转，而是一种面向课堂点名和抽奖场景的启发式公平机制。

算法主要分为三步：

1. **候选池过滤**

   根据学生历史抽取次数计算平均值、最大值和最小值。优先选择抽取次数不高于平均值的学生进入候选池。当最大次数与最小次数差距超过阈值时，触发差距保护，临时排除抽取次数最高的学生。

2. **动态权重计算**

   对候选池中的学生计算动态权重。权重受以下因素影响：

   - 历史抽取次数；
   - 分组抽取次数；
   - 性别维度抽取次数；
   - 距离上次抽取的时间；
   - 是否近期刚被抽中；
   - 权重等级设置。

   历史抽取次数使用平方根形式进行负反馈：

   ```text
   F = sqrt(max_total_count + 1) / sqrt(current_total_count + 1)
   ```

   抽中过少的人权重相对提高，抽中过多的人权重相对降低。

3. **加权随机抽取**

   最终不直接选择权重最高的人，而是使用 `std::discrete_distribution` 按权重随机抽取。这样可以保留随机性，同时让长期结果更趋向均衡。

更详细的算法说明见 [算法思想与参考文献.md](算法思想与参考文献.md) 和 [Core-Algorithm.md](Core-Algorithm.md)。

## Project Structure

```text
.
├── CMakeLists.txt
├── include/
│   ├── AppController.h
│   ├── DataManager.h
│   ├── SecRandom.h
│   ├── Settings.h
│   ├── StudentRecords.h
│   ├── UIStyle.h
│   └── ui/
│       ├── MainWindow.h
│       ├── StudentManageWindow.h
│       ├── HistoryWindow.h
│       └── SettingsWindow.h
├── src/
│   ├── main.cpp
│   ├── AppController.cpp
│   ├── DataManager.cpp
│   ├── SecRandom.cpp
│   ├── Settings.cpp
│   ├── UIStyle.cpp
│   └── ui/
│       ├── MainWindow.cpp
│       ├── StudentManageWindow.cpp
│       ├── HistoryWindow.cpp
│       └── SettingsWindow.cpp
├── data/
│   ├── students.csv
│   ├── students_stats.csv
│   ├── attendance.csv
│   ├── draw_history.csv
│   └── settings.csv
└── docs...
```

## Architecture

项目采用简单的分层设计：

- `main.cpp`：程序入口，只负责创建 `AppController` 并启动程序。
- `AppController`：控制层，连接 UI、数据层、设置层和算法层。
- `DataManager`：数据层，负责 CSV 文件读写和学生数据管理。
- `SecRandomPicker`：算法层，实现普通随机和公平随机。
- `SettingsStore`：设置层，负责读写 `settings.csv`。
- `MainWindow` / `StudentManageWindow` / `HistoryWindow` / `SettingsWindow`：UI 层。
- `UIStyle`：统一界面样式。

这种结构避免将业务逻辑堆在按钮回调中，也方便后续维护和展示。

## Data Format

主要数据文件：

| 文件 | 作用 |
| --- | --- |
| `data/students.csv` | 学生名单 |
| `data/students_stats.csv` | 公平抽取统计数据 |
| `data/attendance.csv` | 点名出勤历史 |
| `data/draw_history.csv` | 抽奖历史 |
| `data/settings.csv` | 程序设置 |
| `data/backup/` | 备份目录 |

## Settings

设置窗口中可调整：

- 抽奖人数；
- 是否启用公平算法；
- 是否允许重复抽取；
- 差距阈值；
- 权重等级；
- 浅色 / 深色主题；
- 重置统计；
- 备份数据；
- 恢复数据。

## Notes About the Reference Project

本项目受到 Python 版 [SECTL/SecRandom](https://github.com/SECTL/SecRandom) 的主要启发。原项目功能更加完整，包含更多系统集成和桌面体验能力。本仓库不是原项目的直接代码移植，而是基于其公平抽取思想进行的 C++ 重新实现：

- 使用 C++ 类和头文件重新组织结构；
- 使用 CSV 和文件流实现数据持久化；
- 使用 C++ 标准库随机分布实现加权随机；
- 使用 FLTK 实现轻量图形界面；
- 只保留适合作为课程大作业展示的核心功能。

## License

本项目以 **GNU GPLv3** 协议开源。

本项目参考并重写自同样采用 GPLv3 协议的 Python 版 [SECTL/SecRandom](https://github.com/SECTL/SecRandom)。感谢原项目提供的功能设计与公平抽取算法思想。本仓库不是原项目的官方版本，而是面向 C++/FLTK 课程大作业场景的重新实现。

完整协议文本见 [LICENSE](LICENSE)。

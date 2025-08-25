# 从v2.0开始，本项目使用GNU通用公共许可证 v3 (GPLv3) 进行许可。有关详细信息，请参阅LICENSE文件。

# 物品信息管理系统 (Inventory Management System)

这是一个基于 C++ 和 Qt 6 开发的、功能完善的桌面物品信息管理系统。它提供了一个直观的图形用户界面（GUI），用于对物品库存进行增、删、改、查等操作，并使用 MySQL 数据库作为后端，支持网络化和多用户协作。

## 主要功能

* **物品管理**: 对物品信息进行全面的增、删、改、查（CRUD）操作。
* **高级搜索**: 支持按关键词（名称、供应商、位置）和物品类别进行组合搜索与筛选。
* **数据统计**: 一键查看库存总览，包括物品总类、总估值，以及按类别和状态分布的详细统计。
* **数据导出**: 可将当前物品列表一键导出为 CSV 文件，方便在 Excel 等表格软件中进行处理和分享。
* **外部化配置**: 数据库连接信息存储在外部 `config.ini` 文件中，安全且便于部署，无需重新编译程序即可更换数据库。
* **图形化设置**: 提供直观的设置对话框，用于测试和修改数据库连接参数。
* **专业外观**: 拥有自定义的应用程序图标。



## 技术栈

* **编程语言**: C++ 17
* **框架**: Qt 6 (核心模块: Widgets, Sql)
* **构建系统**: CMake
* **数据库**: MySQL

## 项目结构

本项目采用了分层和模块化的设计思想，将UI、业务逻辑和数据访问分离开来，使得代码结构清晰，易于维护和扩展。

```
.
├── app_icon.ico           # Windows 可执行文件图标
├── app_icon.png           # Qt 应用程序图标
├── CMakeLists.txt         # 项目构建配置文件
├── config.ini             # (首次运行后自动生成) 数据库配置文件
├── databasemanager.h/cpp  # 【模型层】数据库管理器，封装所有SQL操作
├── dbconfig.h             # 数据结构：用于传递数据库配置
├── inventorysystem.h/cpp  # 【视图/控制层】主窗口，负责UI布局和业务逻辑调度
├── itemformwidget.h/cpp   # 【视图层】左侧的物品信息输入表单控件
├── iteminfo.h             # 数据结构：用于传递物品信息
├── main.cpp               # 程序主入口
├── resources.qrc          # Qt 资源文件 (用于打包图标)
├── settingsdialog.h/cpp   # 【视图/控制层】数据库设置对话框
├── settingsmanager.h/cpp  # 配置管理器，专门负责读写 config.ini
└── win_icon.rc            # Windows 图标资源脚本
```

* **核心模块**:
    * `InventorySystem`: 主窗口，作为视图（View）和控制中心（Controller），响应用户操作，调用其他模块完成任务。
    * `DatabaseManager`: 数据模型（Model），作为应用程序与 MySQL 数据库沟通的唯一桥梁。
    * `SettingsManager`: 配置管理器，独立负责配置文件的读写，与主逻辑解耦。
* **UI组件**:
    * `ItemFormWidget`: 一个自包含的UI组件，负责物品信息的录入。
    * `SettingsDialog`: 一个独立的对话框，负责数据库设置的交互。
* **数据结构**:
    * `ItemInfo`: 定义了“物品”的数据结构。
    * `DbConfig`: 定义了“数据库连接配置”的数据结构。

## 如何开始

请按照以下步骤来编译和运行本项目。

### 1. 环境准备

* **Qt 6.x.x**: 确保已安装 Qt6，并选择了桌面开发组件（如 MinGW 或 MSVC）。
* **CMake**: 版本不低于 3.16。
* **MySQL Server**: 确保你的电脑上或网络中有一个正在运行的 MySQL 数据库服务。
* **Qt MySQL 驱动**: 这是最关键的一步。请确保你的 Qt 安装包含了 `QMYSQL` 驱动插件。同时，系统中需要安装 MySQL C Connector 开发库（`libmysqlclient-dev`）。

### 2. 创建数据库

在运行程序之前，你需要手动在你的 MySQL 服务器上创建一个空的数据库。可以使用以下 SQL 命令：
```sql
CREATE DATABASE inventory_db CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
```

### 3. 构建项目

使用 CMake 从命令行构建项目：

```bash
# 1. 在项目根目录下，创建一个 build 文件夹并进入
mkdir build
cd build

# 2. 运行 CMake 来生成构建文件
cmake ..

# 3. 运行构建命令来编译项目
cmake --build .
```
编译成功后，可执行文件将位于 `build` 目录下。

## 配置方法

本程序的数据库连接信息通过项目目录下的 `config.ini` 文件进行配置。

### 首次运行

当你第一次运行程序时，如果程序发现 `config.ini` 文件不存在，它会自动创建一个包含默认/示例配置的文件。内容如下：

```ini
[Database]
host=127.0.0.1
port=3306
dbname=inventory_db
user=root
password=your_password_here
```

此时，程序很可能会因为密码不正确而连接失败。你需要：

1.  **关闭程序**。
2.  **用文本编辑器打开 `config.ini` 文件**。
3.  将 `host`, `port`, `dbname`, `user`, `password` 修改为你真实的 MySQL 连接信息。
4.  **保存文件并重新启动程序**。

### 后续修改

之后，你可以通过两种方式修改配置：
1.  直接编辑 `config.ini` 文件（程序关闭时）。
2.  在程序运行时，通过菜单栏的 **文件 -> 设置** 打开图形化对话框进行修改。修改并保存后，需要**重启程序**才能使新配置生效。

# 配置qt里的MySQL驱动时得到的dll文件应该放到的位置（根据实际情况调整）
<img width="910" height="463" alt="8a83ba8cf5752eaad8dbca5acaa5b646" src="https://github.com/user-attachments/assets/48e8d113-d1fa-49f9-9ceb-a440b5f095e0" />

获取MySQL dll文件的网址：https://github.com/thecodemonkey86/qt_mysql_driver  请看好版本进行挑选，放到正确的位置

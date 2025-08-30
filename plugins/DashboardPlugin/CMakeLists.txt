# CMake 最低版本要求
cmake_minimum_required(VERSION 3.16)

# 1. 项目名称，这将是最终生成的 .dll 文件的主名
project(DashboardPlugin LANGUAGES CXX)

# 设置C++标准为C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 为Qt的MOC, UIC, RCC工具提供自动化支持
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 查找此插件所需的Qt6依赖包（因为有UI，所以需要Core, Gui, Widgets）
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# 定义项目的源文件和头文件
set(PROJECT_SOURCES
    dashboardplugin.cpp
)
set(PROJECT_HEADERS
    dashboardplugin.h
)

# 2. 【核心】创建库目标，类型为 SHARED (共享库)，即 .dll/.so 文件
add_library(${PROJECT_NAME} SHARED
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
)

# 3. 【核心】包含主程序的头文件目录
#    这行代码至关重要，它让插件项目能找到 plugin_interface_ui.h 等接口文件。
#    路径 "../InventorySystem" 假设您的目录结构是：
#    - MyProject/
#      - InventorySystem/
#      - Plugins/
#        - DashboardPlugin/
#    如果您的目录结构不同，请相应修改此相对路径。
target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "../InventorySystem" 
)

# 4. 链接Qt库到我们的插件项目
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)
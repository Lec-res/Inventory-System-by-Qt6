# CMake 最低版本要求
cmake_minimum_required(VERSION 3.16)

# 项目名称
project(DashboardPlugin LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 自动化 MOC, UIC, RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 查找此插件所需的Qt6依赖包
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# 定义项目的源文件和头文件
set(PROJECT_SOURCES
    dashboardplugin.cpp
)
set(PROJECT_HEADERS
    dashboardplugin.h
)

# 创建共享库目标
add_library(${PROJECT_NAME} SHARED
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
)

# 包含主程序的头文件目录
target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "../../" 
)

# 链接Qt库
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)
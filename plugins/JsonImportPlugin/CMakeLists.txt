# CMake 最低版本要求
cmake_minimum_required(VERSION 3.16)
# 项目名称和语言
project(JsonImportPlugin LANGUAGES CXX)

# 查找Qt6的依赖包
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# 为Qt的MOC, UIC, RCC工具提供自动化支持
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 定义项目的源文件和头文件
set(PROJECT_SOURCES
    jsonimportplugin.cpp
)
set(PROJECT_HEADERS
    jsonimportplugin.h
)

# 【核心修改】将主项目的接口头文件也添加到库的源文件列表中
add_library(${PROJECT_NAME} SHARED
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
    "../../plugin_interface.h"  # <-- 【修正】移除了多余的 "inventorysystem-formal/"
)

# 【核心】包含主程序的头文件目录
target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "../.." # <-- 【修正】包含的目录就是上两级的根目录
)

# 链接Qt库到我们的插件项目
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)
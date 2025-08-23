cmake_minimum_required(VERSION 3.16)

project(InventorySystem VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Qt6组件
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql)

# CMake 最低版本要求
cmake_minimum_required(VERSION 3.16)

# 项目名称和语言
project(InventorySystem LANGUAGES CXX)

# 设置C++标准为C++17，与.pro文件保持一致
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 为Qt的MOC, UIC, RCC工具提供自动化支持
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 查找Qt6的依赖包。如果使用Qt5，请将Qt6改为Qt5
# 需要Widgets（界面）和Sql（数据库）模块
find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

# 定义项目的源文件和头文件
set(PROJECT_SOURCES
    main.cpp
    inventorysystem.cpp
    databasemanager.cpp
    itemformwidget.cpp
)

set(PROJECT_HEADERS
    inventorysystem.h
    databasemanager.h
    itemformwidget.h
    iteminfo.h
)

# 添加可执行文件目标
add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
)

# 链接Qt库到我们的项目
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::Sql
)

# 如果你使用的是MSVC编译器（Windows），可以设置UTF-8编码以避免中文乱码
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /Zc:__cplusplus /utf-8)
endif()

# 安装规则（可选），用于创建安装包
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)

# CMake 最低版本要求
cmake_minimum_required(VERSION 3.16)

# 项目名称和语言
project(InventorySystem LANGUAGES CXX)

# 设置C++标准为C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 为Qt的MOC, UIC, RCC工具提供自动化支持
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 查找Qt6的依赖包 (包含了所有用到的模块)
find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Network)

# 定义项目的源文件、头文件和资源文件
set(PROJECT_SOURCES
    main.cpp
    inventorysystem.cpp
    databasemanager.cpp
    itemformwidget.cpp
    settingsmanager.cpp
    settingsdialog.cpp
    updatechecker.cpp
    resources.qrc
)

set(PROJECT_HEADERS
    inventorysystem.h
    databasemanager.h
    itemformwidget.h
    iteminfo.h
    settingsmanager.h
    settingsdialog.h
    updatechecker.h
    dbconfig.h
    plugin_interface.h
    plugin_interface_ui.h
)

# 添加可执行文件目标
add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
)

# 如果是在 Windows 平台上编译，处理图标资源
if(WIN32)
    # 将目标设置为 WIN32 可执行文件 (GUI程序，而非控制台程序)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE ON)

    # 以安全的方式，将 Windows 资源文件附加到目标上
    target_sources(${PROJECT_NAME} PRIVATE win_icon.rc)
endif()


# 链接Qt库到我们的项目
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::Sql
    Qt6::Network
)

# 如果你使用的是MSVC编译器（Windows），可以设置UTF-8编码以避免中文乱码
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /Zc:__cplusplus /utf-8)
endif()

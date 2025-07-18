# 设置 CMake 的最低版本要求
cmake_minimum_required(VERSION 3.10)

# 设置项目名称
project(QtServer)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)

# 查找 Qt5 相关组件
find_package(Qt5 COMPONENTS Core Gui Widgets Network REQUIRED)

# 设置自动处理 Qt 的 MOC、UIC 和 RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 添加可执行文件
add_executable(${PROJECT_NAME}
    main.cpp
    mainwindow.cpp
    mainwindow.h
    tcpserver.cpp
    tcpserver.h
)

# 链接 Qt5 库
target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Network
)
# Set the minimum version requirement for CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(QtServer)

# Setting the C++ Standard
set(CMAKE_CXX_STANDARD 11)

# Find Qt5 related components
find_package(Qt5 COMPONENTS Core Gui Widgets Network REQUIRED)

# Set up automatic processing of Qt's MOC, UIC and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Adding an executable file
add_executable(${PROJECT_NAME}
    main.cpp
    mainwindow.cpp
    mainwindow.h
    tcpserver.cpp
    tcpserver.h
)

# Linking Qt5 Libraries
target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Network
)

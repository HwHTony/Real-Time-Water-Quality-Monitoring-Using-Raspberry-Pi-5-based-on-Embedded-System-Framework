cmake_minimum_required(VERSION 3.10)
project(WaterQualityMonitor)

set(CMAKE_CXX_STANDARD 11)

# 选项：是否构建测试
option(BUILD_TESTS "Build unit tests" OFF)
option(ENABLE_COVERAGE "Enable test coverage" OFF)

# 源文件列表
set(SOURCES
    ds18b20.cpp
    pcf8591.cpp
    sock.cpp
    tft_freetype.cpp
)

# 头文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# 查找依赖库
find_package(PkgConfig REQUIRED)
pkg_check_modules(FT2 REQUIRED freetype2)
pkg_check_modules(GPIOD REQUIRED libgpiod)

# 添加 FreeType 和 gpiod 头文件
include_directories(${FT2_INCLUDE_DIRS} ${GPIOD_INCLUDE_DIRS})

# 主程序
add_executable(water_quality_monitor
    main.cpp
    ${SOURCES}
)

# 链接主程序库
target_link_libraries(water_quality_monitor
    ${FT2_LIBRARIES}
    ${GPIOD_LIBRARIES}
    pthread
)

# 测试配置
if(BUILD_TESTS)
    # 查找 Google Test
    find_package(GTest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS})
    
    # 测试源文件
    set(TEST_SOURCES
        test/main_test.cpp
    )
    
    # 测试程序
    add_executable(water_quality_monitor_test
        ${TEST_SOURCES}
        ${SOURCES}
    )
    
    # 链接测试库
    target_link_libraries(water_quality_monitor_test
        ${GTEST_LIBRARIES}
        ${GTEST_MAIN_LIBRARIES}
        ${FT2_LIBRARIES}
        ${GPIOD_LIBRARIES}
        pthread
    )
    
    # 测试覆盖率配置
    if(ENABLE_COVERAGE)
        # 确保使用 GCC 或 Clang 编译器
        if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            message(FATAL_ERROR "Coverage is only supported with GCC or Clang")
        endif()
        
        # 添加覆盖率编译选项
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
        
        # 添加覆盖率报告生成目标
        add_custom_target(coverage
            COMMAND ${CMAKE_COMMAND} -E remove -f coverage.info coverage.info.cleaned
            COMMAND ${CMAKE_COMMAND} -E make_directory coverage_report
            COMMAND lcov --capture --directory . --output-file coverage.info
            COMMAND lcov --remove coverage.info '/usr/*' '*/tests/*' --output-file coverage.info.cleaned
            COMMAND genhtml -o coverage_report coverage.info.cleaned
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generate code coverage report"
            VERBATIM
        )
        
        # 确保测试运行后再生成覆盖率报告
        add_dependencies(coverage water_quality_monitor_test)
    endif()
endif()
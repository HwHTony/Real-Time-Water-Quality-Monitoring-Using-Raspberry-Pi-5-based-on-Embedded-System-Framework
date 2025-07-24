cmake_minimum_required(VERSION 3.10)
project(WaterQualityMonitor)

set(CMAKE_CXX_STANDARD 11)

# Option: Whether to conduct the test
option(BUILD_TESTS "Build unit tests" OFF)
option(ENABLE_COVERAGE "Enable test coverage" OFF)

# Source file list
set(SOURCES
    src/app/app.cpp  # Update to the new path
    src/data_collection/ds18b20.cpp
    src/data_collection/pcf8591.cpp
    src/networking/sock.cpp
    src/display/tft_freetype.cpp
    src/data_collection/data_collector.cpp
    src/info_updating/debug_info_updater.cpp
    src/info_updating/tft_info_updater.cpp
    src/info_updating/socket_info_updater.cpp
    src/event_loop/event_loop.cpp
    src/main.cpp
    src/common/water_quality.cpp  # Add the "water_quality" file
)

# Header file directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Search for dependent libraries
find_package(PkgConfig REQUIRED)
pkg_check_modules(FT2 REQUIRED freetype2)
pkg_check_modules(GPIOD REQUIRED libgpiod)

# Add the FreeType and gpiod header files
include_directories(${FT2_INCLUDE_DIRS} ${GPIOD_INCLUDE_DIRS})

# Main program
add_executable(water_quality_monitor
    src/main.cpp
    ${SOURCES}
)

# Link the main program library
target_link_libraries(water_quality_monitor
    ${FT2_LIBRARIES}
    ${GPIOD_LIBRARIES}
    pthread
)

# Test configuration
if(BUILD_TESTS)
    # Search for "Google Test"
    find_package(GTest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS})
    
    # Test source file
    set(TEST_SOURCES
        test/main_test.cpp
    )
    
    # Testing program
    add_executable(water_quality_monitor_test
        ${TEST_SOURCES}
        ${SOURCES}
    )
    
    # Link test library
    target_link_libraries(water_quality_monitor_test
        ${GTEST_LIBRARIES}
        ${GTEST_MAIN_LIBRARIES}
        ${FT2_LIBRARIES}
        ${GPIOD_LIBRARIES}
        pthread
    )
    
    # Test coverage configuration
    if(ENABLE_COVERAGE)
        # Make sure to use the GCC or Clang compiler.
        if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            message(FATAL_ERROR "Coverage is only supported with GCC or Clang")
        endif()
        
        # Add coverage compilation option
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
        
        # Add the goal of generating coverage reports
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

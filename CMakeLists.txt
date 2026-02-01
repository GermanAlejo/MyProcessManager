cmake_minimum_required(VERSION 3.12)
project(MyProcessManager)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Subdirectory organization
add_subdirectory(src/core)
add_subdirectory(src/common)
add_subdirectory(src/process)
add_subdirectory(src/display)

# Add external libraries
find_package(spdlog REQUIRED)

# Main executable
add_executable(MyProcessManager src/main.cpp)
target_include_directories(MyProcessManager PRIVATE include)
target_link_libraries(MyProcessManager PRIVATE display monitor activeProcesses process errors types snapShop common spdlog::spdlog)



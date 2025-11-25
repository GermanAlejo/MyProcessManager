cmake_minimum_required(VERSION 4.0)
project(MyProcessManager)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include folder for headers
# Include directory
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source grouping
file(GLOB_RECURSE PROCESS_SRC "src/process.cpp")

# Include folder (root include path)
add_library(process ${PROCESS_SRC})
target_include_directories(process PUBLIC include)

# Main executable
add_executable(MyProcessManager src/main.cpp
        include/common.h)
target_link_libraries(MyProcessManager PRIVATE process)



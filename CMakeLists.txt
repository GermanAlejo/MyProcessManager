cmake_minimum_required(VERSION 4.0)
project(MyProcessManager)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include folder for headers
# Include directory
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source grouping
file(GLOB_RECURSE ACTIVE_SRC "src/activeProcesses.cpp")
file(GLOB_RECURSE PROCESS_SRC "src/process.cpp")
file(GLOB_RECURSE ERROR_SRC "src/errors.cpp")
file(GLOB_RECURSE TYPES_SRC "src/process_types.cpp")
file(GLOB_RECURSE COMMON_SRC "src/common.cpp")
file(GLOB_RECURSE MONITOR_SRC "src/systemMonitor.cpp")

# Include folder (root include path)
add_library(monitor ${MONITOR_SRC})
target_include_directories(monitor PUBLIC include)

add_library(activeProcesses ${ACTIVE_SRC})
target_include_directories(activeProcesses PUBLIC include)

add_library(process ${PROCESS_SRC})
target_include_directories(process PUBLIC include)

add_library(errors ${ERROR_SRC})
target_include_directories(errors PUBLIC include)

add_library(types ${TYPES_SRC})
target_include_directories(types PUBLIC include)

add_library(common ${COMMON_SRC})
target_include_directories(common PUBLIC include)

# Add external libraries
find_package(spdlog REQUIRED)

# Main executable
add_executable(MyProcessManager src/main.cpp)
target_link_libraries(MyProcessManager PRIVATE monitor activeProcesses process errors types common spdlog::spdlog)



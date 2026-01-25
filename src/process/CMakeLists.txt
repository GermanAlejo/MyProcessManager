add_library(process process.cpp)
target_include_directories(process PUBLIC ../../include)

add_library(types process_types.cpp)
target_include_directories(types PUBLIC ../../include)

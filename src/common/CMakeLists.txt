add_library(common common.cpp)
target_include_directories(common PUBLIC ../../include)

add_library(errors errors.cpp)
target_include_directories(errors PUBLIC ../../include)

add_library(monitor systemMonitor.cpp)
target_include_directories(monitor PUBLIC ../../include)

add_library(snapShop cpuSnapShot.cpp)
target_include_directories(snapShop PUBLIC ../../include)

add_library(activeProcesses activeProcesses.cpp)
target_include_directories(activeProcesses PUBLIC ../../include)

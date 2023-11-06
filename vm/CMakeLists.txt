
set(SOURCES src/chunk.cpp src/debug.cpp src/vm.cpp src/object.cpp)


add_library(vm ${SOURCES})

target_include_directories(vm PUBLIC include)

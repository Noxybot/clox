
set(SOURCES src/scanner.cpp)

add_library(scanner ${SOURCES})

target_include_directories(scanner PUBLIC include)

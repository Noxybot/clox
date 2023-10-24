cmake_minimum_required(VERSION 3.5)  # Minimum CMake version required

project(clox)  # Name your project

# Set C++ standard (e.g., C++11, C++14, C++17, C++20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # Enable export of compile_commands.json

# Optional: Set the build type (Debug, Release, etc.)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Optional: Add additional compiler flags if needed
# add_compile_options(-Wall -Wextra)


# Optional: Include directories (e.g., header files)
include_directories(
    include
)

# Enable AddressSanitizer
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=address")


add_subdirectory(src)
add_subdirectory(test)

# Add any additional libraries or dependencies
# find_package(YourLibrary REQUIRED)

# Create the executable from source files
add_executable(clox src/main.cpp)
target_link_libraries(clox PRIVATE clox_lib)

# Add your test targets


# Link libraries to your executable
# target_link_libraries(YourExecutableName YourLibrary)

# Optionally, specify the installation directories
# install(TARGETS YourExecutableName DESTINATION bin)

# Optionally, create tests using CTest
# enable_testing()
# add_test(NAME YourTestName COMMAND YourExecutableName)

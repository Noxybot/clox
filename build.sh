#!/bin/bash

# Set the directory where the CMakeLists.txt file is located
project_dir=$(pwd)

# Get the desired build type as an argument, default to "Debug"
build_type="${1:-Debug}"

# Optional: Clean the build directory before building
# (Comment out if you don't want to clean the build directory)
#rm -rf "$project_dir/build"

# Create a build directory and navigate to it
mkdir -p "$project_dir/build"
cd "$project_dir/build"

# Configure the project using CMake with Ninja as the generator
cmake -DCMAKE_BUILD_TYPE="$build_type" -G "Ninja" ..

# Build the project
ninja

# Optional: Run tests if you have them
# ninja test

# Optionally, install the project (if specified in CMakeLists.txt)
# ninja install

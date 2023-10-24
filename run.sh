#!/bin/bash

# Set the directory where the CMake-generated executable is located
build_dir="$(pwd)/build"

# Specify the name of the executable
executable_name="clox"

# Navigate to the build directory
cd "$build_dir"

# Check if the executable exists
if [ -f "$executable_name" ]; then
  # Run the executable
  ./"$executable_name"
else
  echo "Executable '$executable_name' not found in '$build_dir'."
fi

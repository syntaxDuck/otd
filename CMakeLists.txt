# Specify the minimum CMake version required
cmake_minimum_required(VERSION 3.19)

# Set the project name and version
project(otd VERSION 0.1 LANGUAGES C)

# Specify the C compiler
set(CMAKE_C_COMPILER clang)

# Generate a compile commands file for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Setup debug
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS_DEBUG "-g")

# Define source files
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add project executable
add_executable(main ${SOURCES})

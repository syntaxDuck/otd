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
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/opt/homebrew/lib")

# Define source files
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBSODIUM REQUIRED libsodium)

# Make sure that the variables are being passed correctly
message(STATUS "LIBSODIUM_LIBRARIES: ${LIBSODIUM_LIBRARIES}")
message(STATUS "LIBSODIUM_INCLUDE_DIRS: ${LIBSODIUM_INCLUDE_DIRS}")
# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add project executable
add_executable(main ${SOURCES})

target_link_libraries(main PRIVATE ${LIBSODIUM_LIBRARIES})
target_include_directories(main PRIVATE ${LIBSODIUM_INCLUDE_DIRS})
